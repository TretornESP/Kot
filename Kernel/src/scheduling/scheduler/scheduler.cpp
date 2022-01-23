#include "scheduler.h"

TaskManager* globalTaskManager;

static uint64_t mutexScheduler;

void TaskManager::Scheduler(InterruptStack* Registers, uint8_t CoreID){  
    if(IsSchedulerEnable[CoreID]){  
        Atomic::atomicSpinlock(&mutexScheduler, 1);
        Atomic::atomicLock(&mutexScheduler, 1);

        uint64_t actualTime = HPET::GetTime();
        thread_t* ThreadEnd = ThreadExecutePerCore[CoreID];
        if(ThreadExecutePerCore[CoreID] != NULL){
            /* Save & enqueu thread */
            ThreadEnd->SaveContext(Registers, CoreID);
            EnqueueTask(ThreadEnd);
        }

        /* Update time */
        TimeByCore[CoreID] = actualTime;

        /* Find & load new task */
        thread_t* ThreadStart = GetTread();
        ThreadStart->CreatContext(Registers, CoreID);

        Atomic::atomicUnlock(&mutexScheduler, 1);
    }
}

void TaskManager::EnqueueTask(thread_t* thread){
    if(thread->IsInQueue) return;
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);
    
    if(FirstNode == NULL) FirstNode = thread;

    if(LastNode != NULL){
        LastNode->Next = thread;
        thread->Last = LastNode;
    }

    LastNode = thread;
    thread->Next = FirstNode;

    thread->IsInQueue = true;
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

void TaskManager::DequeueTask(thread_t* thread){
    if(!thread->IsInQueue) return;
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);
    if(FirstNode == thread){
        if(FirstNode != thread->Next){
           FirstNode = thread->Next; 
        }else{
            FirstNode = NULL;
        }
    }
    if(LastNode == thread){
        if(thread->Last != NULL){
            LastNode = thread->Last; 
        }else{
            LastNode = FirstNode;
        }
        
    }      

    if(LastNode != NULL){
        LastNode->Next = FirstNode;   
    }
   

    if(thread->Last != NULL) thread->Last->Next = thread->Next;
    thread->Next->Last = thread->Last;
    thread->Last = NULL;
    thread->Next = NULL;


    thread->IsInQueue = false;
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

void TaskManager::DequeueTaskWithoutLock(thread_t* thread){
    if(FirstNode == thread){
        if(FirstNode != thread->Next){
           FirstNode = thread->Next; 
        }else{
            FirstNode = NULL;
        }
    }
    if(LastNode == thread){
        if(thread->Last != NULL){
            LastNode = thread->Last; 
        }else{
            LastNode = FirstNode;
        }
        
    }      

    if(LastNode != NULL){
        LastNode->Next = FirstNode;   
    }
   

    if(thread->Last != NULL) thread->Last->Next = thread->Next;
    thread->Next->Last = thread->Last;
    thread->Last = NULL;
    thread->Next = NULL;


    thread->IsInQueue = false;
}

thread_t* TaskManager::GetTread(){
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);

    thread_t* ReturnValue = FirstNode;

    DequeueTaskWithoutLock(ReturnValue);

    Atomic::atomicUnlock(&mutexScheduler, 0);    
    return ReturnValue;
}

bool TaskManager::ExecThread(thread_t* self, Parameters* FunctionParameters){
    self->Launch(FunctionParameters);
    return true;
}

process_t* TaskManager::CreatProcess(uint8_t priviledge, void* externalData){
    process_t* proc = (process_t*)calloc(sizeof(process_t));

    if(ProcessList == NULL){
        ProcessList = CreatNode((void*)0);
        proc->NodeParent = ProcessList->Add(proc);
    }else{
        proc->NodeParent = ProcessList->Add(proc);
    }

    /* Setup default paging */
    proc->SharedPaging = globalPageTableManager[GetCoreID()].SetupProcessPaging();

    /* Setup default priviledge */
    proc->DefaultPriviledge = priviledge;

    /* Save time */
    proc->CreationTime = HPET::GetTime();

    /* Other data */
    proc->TaskManagerParent = this;
    proc->externalData = externalData;

    proc->PID = PID; 
    PID++;
    NumberProcessTotal++;

    return proc;
}

thread_t* process_t::CreatThread(uint64_t entryPoint, void* externalData){
    return CreatThread(entryPoint, DefaultPriviledge, externalData);
}

thread_t* process_t::CreatThread(uint64_t entryPoint, uint8_t priviledge, void* externalData){
    thread_t* thread = (thread_t*)calloc(sizeof(thread_t));
    if(Childs == NULL){
        Childs = CreatNode((void*)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = globalPageTableManager[GetCoreID()].SetupThreadPaging(this->SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Setup priviledge */
    uint8_t RingPriviledge = priviledge & 0b11;
    thread->RingPL = priviledge;

    /* Setup registers */
    thread->Regs->rip = (void*)entryPoint;
    thread->Regs->cs = (void*)(GDTInfoSelectorsRing[RingPriviledge].Code | RingPriviledge);
    thread->Regs->ss = (void*)(GDTInfoSelectorsRing[RingPriviledge].Data | RingPriviledge);
    thread->Regs->rflags.Reserved0 = true;
    thread->Regs->rflags.IF = true;
    thread->Regs->rflags.IOPL = 0;
    
   
    /* Other data */
    thread->externalData = externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->Parent = this;


    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfThread++;

    return thread;
}

thread_t* process_t::DuplicateThread(thread_t* source){
    thread_t* thread = (thread_t*)calloc(sizeof(thread_t));
    if(Childs == NULL){
        Childs = CreatNode((void*)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = globalPageTableManager[GetCoreID()].SetupThreadPaging(source->Parent->SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Setup registers */
    thread->Regs->rip = source->EntryPoint;
    thread->Regs->cs = source->Regs->cs; 
    thread->Regs->ss = source->Regs->ss; 
    thread->Regs->rflags = source->Regs->rflags;
    
    /* Setup priviledge */
    thread->RingPL = source->RingPL;

    /* Other data */
    thread->externalData = source->externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->Parent = this;

    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfThread++;

    return thread;
}


void thread_t::SetupStack(){
    uint64_t StackLocation = GetVirtualAddress(0x100, 0, 0, 0);
    this->Regs->rsp = (void*)StackLocation;
    this->Stack = (StackInfo*)malloc(sizeof(StackInfo));
    this->Stack->StackStart = StackLocation;
    this->Stack->StackEnd = StackLocation;
    this->Stack->StackEndMax = GetVirtualAddress(0xff, 0, 0, 0);

    /* Clear stack */
    PageTable* PML4VirtualAddress = (PageTable*)globalPageTableManager[GetCoreID()].GetVirtualAddress((void*)Paging->PML4);
    PML4VirtualAddress->entries[0xff].Value = NULL;
}



void TaskManager::SwitchTask(InterruptStack* Registers, uint8_t CoreID, thread_t* task){
    if(task == NULL) return;

    Atomic::atomicSpinlock(&mutexScheduler, 1);
    Atomic::atomicLock(&mutexScheduler, 1);

    uint64_t actualTime = HPET::GetTime();
    thread_t* TaskEnd = ThreadExecutePerCore[CoreID];
    if(ThreadExecutePerCore[CoreID] != NULL){
        //Save & enqueu task
        TaskEnd->SaveContext(Registers, CoreID);
        EnqueueTask(TaskEnd);
    }

    //Update time
    TimeByCore[CoreID] = actualTime;

    //Load new task
    task->CreatContext(Registers, CoreID);

    Atomic::atomicUnlock(&mutexScheduler, 1);
}

void TaskManager::CreatIddleTask(){
    if(IddleProc == NULL){
        IddleProc = CreatProcess(0xf, 0);
    }
    thread_t* thread = IddleProc->CreatThread(0, 0);

    IdleNode[IddleTaskNumber] = thread;
    IddleTaskNumber++;

    void* physcialMemory = globalAllocator.RequestPage();
    thread->Paging->MapMemory(0x0, physcialMemory);
    thread->Paging->MapUserspaceMemory(0x0);
    void* virtualMemory = globalPageTableManager[GetCoreID()].GetVirtualAddress(physcialMemory);
    memcpy(virtualMemory, (void*)&IdleTask, 0x1000);

    thread->Launch();
}
void TaskManager::InitScheduler(uint8_t NumberOfCores){
    for(int i = 0; i < NumberOfCores; i++){
        CreatIddleTask();
    } 

    TaskManagerInit = true;
}

void TaskManager::EnabledScheduler(uint8_t CoreID){ 
    if(TaskManagerInit){
        Atomic::atomicSpinlock(&mutexScheduler, 0);
        Atomic::atomicLock(&mutexScheduler, 0); 

        ThreadExecutePerCore[CoreID] = NULL;
        
        SaveTSS((uint64_t)CoreID);

        IsSchedulerEnable[CoreID] = true;
        Atomic::atomicUnlock(&mutexScheduler, 0);
        globalLogs->Successful("Scheduler is enabled for the processor : %u", CoreID);
    }
}

thread_t* TaskManager::GetCurrentThread(uint8_t CoreID){
    return ThreadExecutePerCore[CoreID];
}

void thread_t::SaveContext(InterruptStack* Registers, uint8_t CoreID){
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - Parent->TaskManagerParent->TimeByCore[CoreID];
    memcpy(Regs, Registers, sizeof(ContextStack));
}

void thread_t::CreatContext(InterruptStack* Registers, uint8_t CoreID){
    this->CoreID = CoreID;
    Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = this;
    memcpy(Registers, Regs, sizeof(ContextStack));

    asm("mov %0, %%cr3" :: "r" (Paging->PML4));
}

void thread_t::SetParameters(Parameters* FunctionParameters){
    Regs->rdi = (void*)FunctionParameters->Parameter0;
    Regs->rsi = (void*)FunctionParameters->Parameter1;
    Regs->rdx = (void*)FunctionParameters->Parameter2;
    Regs->rcx = (void*)FunctionParameters->Parameter3;
    Regs->r8 = (void*)FunctionParameters->Parameter4;
    Regs->r9 = (void*)FunctionParameters->Parameter5;
}

void thread_t::CopyStack(thread_t* source){
    PageTable* PML4VirtualAddressSource = (PageTable*)globalPageTableManager[GetCoreID()].GetVirtualAddress((void*)source->Paging->PML4);
    PageTable* PML4VirtualAddressDestination = (PageTable*)globalPageTableManager[GetCoreID()].GetVirtualAddress((void*)Paging->PML4);
    PML4VirtualAddressSource->entries[0xff] = PML4VirtualAddressDestination->entries[0xff];
    Stack = source->Stack;
}

bool thread_t::ExtendStack(uint64_t address){
    if(this->Stack == NULL) return false;

    address -= address % 0x1000;
    if(this->Stack->StackStart <= address) return false;
    if(address <= this->Stack->StackEndMax) return false;

    size_t SizeToAdd = this->Stack->StackEnd - address;
    uint64_t NumberOfPage = Divide(SizeToAdd, 0x1000);
    
    uint64_t VirtualAddressIterator = this->Stack->StackEnd;
    bool IsUser = false;
    if(this->RingPL == UserAppRing) IsUser = true;
    for(uint64_t i = 0; i < NumberOfPage; i++){
        VirtualAddressIterator -= 0x1000;
        MemoryAllocated += 0x1000;
        Paging->MapMemory((void*)VirtualAddressIterator, globalAllocator.RequestPage());
        if(IsUser) Paging->MapUserspaceMemory((void*)VirtualAddressIterator);
    }

    this->Stack->StackEnd = VirtualAddressIterator;
    return true;
}

bool thread_t::Fork(InterruptStack* Registers, uint8_t CoreID, thread_t* thread, Parameters* FunctionParameters){
    if(FunctionParameters != NULL){
        thread->SetParameters(FunctionParameters);
    }
    Fork(Registers, CoreID, thread);
}
    
bool thread_t::Fork(InterruptStack* Registers, uint8_t CoreID, thread_t* thread){
    Atomic::atomicSpinlock(&mutexScheduler, 1);
    Atomic::atomicLock(&mutexScheduler, 1);

    thread->IsForked = true;
    thread->ForkedThread = this;

    //Save context
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    //Load new task
    thread->CreatContext(Registers, CoreID);

    Atomic::atomicUnlock(&mutexScheduler, 1);

    return true;
}

bool thread_t::Launch(Parameters* FunctionParameters){
    if(FunctionParameters != NULL){
        SetParameters(FunctionParameters);
    }
    Launch();
    return true;
}

bool thread_t::Launch(){
    IsBlock = false;
    Parent->TaskManagerParent->EnqueueTask(this);
    return true;
}

bool thread_t::Pause(InterruptStack* Registers, uint8_t CoreID){
    //Save context
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = NULL;

    IsBlock = true;

    Parent->TaskManagerParent->Scheduler(Registers, CoreID);

    return true;
}

bool thread_t::Exit(InterruptStack* Registers, uint8_t CoreID){
    if(IsForked){
        void* ReturnValue = Registers->rdi;
        Parent->TaskManagerParent->SwitchTask(Registers, CoreID, ForkedThread);
        Registers->rdi = ReturnValue;
    }else{
        Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = NULL;
        Parent->TaskManagerParent->Scheduler(Registers, CoreID);        
    }

    ThreadNode->Delete();

    //TODO : free stack 

    free(this->Regs);
    free(this);
    return true;
}

bool thread_t::SetIOPriviledge(ContextStack* Registers, uint8_t IOPL){
    Registers->rflags.IOPL = IOPL & 0b11;
    this->IOPL = IOPL & 0b11;
    return true;
}