#include <event/event.h>

namespace Event{
    static uint64_t mutexEvent;

    uint64_t Create(kevent_t** event, enum EventType Type, uint64_t AdditionnalData){
        kevent_t* self = NULL;

        switch (Type){
            case EventTypeIRQLines: {
                IRQLinekevent_t* event = (IRQLinekevent_t*)kmalloc(sizeof(IRQLinekevent_t));
                self = &event->Header;
                event->IRQLine = (uint8_t)AdditionnalData;
                event->IsEnable = false;
                self->Parameters.arg[0] = AdditionnalData;
                break;
            }                
            case EventTypeIRQ: {
                IRQkevent_t* event = (IRQkevent_t*)kmalloc(sizeof(IRQkevent_t));
                self = &event->header;
                event->IRQ = (uint8_t)AdditionnalData;
                self->Parameters.arg[0] = AdditionnalData;
                break;                
            }
            case EventTypeIPC: {
                IPCkevent_t* event = (IPCkevent_t*)kmalloc(sizeof(IPCkevent_t));
                self = &event->header;
                event->master = (kthread_t*)AdditionnalData;
                self->Parameters.arg[0] = NULL;
                break;                
            }

            default:
                return KFAIL;

        }

        AtomicClearLock(&self->Lock);
        self->Tasks = NULL;
        self->NumTask = 0;
        self->Type = Type;


        *event = self;

        return KSUCCESS;
    }

    uint64_t Bind(kthread_t* task, kevent_t* self, bool IgnoreMissedEvents){
        if(task->IsEvent) return KFAIL;
        AtomicAquire(&self->Lock);

        if(self->Type == EventTypeIRQLines){
            IRQLinekevent_t* event = (IRQLinekevent_t*)self;
            if(!event->IsEnable){
                event->IsEnable = true;
                APIC::IoChangeIrqState(event->IRQLine, 0, event->IsEnable);
            }
        }
        
        self->NumTask++;
        self->Tasks = (kevent_tasks_t**)krealloc(self->Tasks, self->NumTask * sizeof(kevent_tasks_t*));

        kevent_tasks_t* TasksEvent = (kevent_tasks_t*)kcalloc(sizeof(kevent_tasks_t));
        TasksEvent->thread = task;
        TasksEvent->Event = self;

        if(!task->IsEvent){
            task->EventDataNode = (event_data_node_t*)kcalloc(sizeof(event_data_node_t));
            if(self->Type == EventTypeIRQLines){
                IRQLinekevent_t* event = (IRQLinekevent_t*)self;
                if(event->IRQLine == 12){
                    Message("%x", task->EventDataNode);
                }
            }
            task->EventDataNode->Event = self;
            task->EventDataNode->LastData = (event_data_t*)kmalloc(sizeof(event_data_t));
            task->EventDataNode->LastData->Next = (event_data_t*)kmalloc(sizeof(event_data_t));
            task->EventDataNode->CurrentData = task->EventDataNode->LastData;
        }
        TasksEvent->DataNode = task->EventDataNode;

        TasksEvent->IgnoreMissedEvents = IgnoreMissedEvents;
        
        task->IsEvent = true;   

        self->Tasks[self->NumTask - 1] = TasksEvent;

        AtomicRelease(&self->Lock);
        return KSUCCESS;
    }

    uint64_t Unbind(kthread_t* task, kevent_t* self){
        if(self->NumTask <= 0) return KFAIL;
        AtomicAquire(&self->Lock);

        self->NumTask--;
        for(size64_t i = 0; i < self->NumTask; i++){
            if(self->Tasks[i]->thread == task){
                uintptr_t newPos = kmalloc(self->NumTask * sizeof(kevent_tasks_t));
                memcpy(newPos, self->Tasks[i], sizeof(kevent_tasks_t) * i);
                i++;
                memcpy((uintptr_t)((uint64_t)newPos + sizeof(kevent_tasks_t) * (i - 1)), (uintptr_t)((uint64_t)self->Tasks[i] + sizeof(kevent_tasks_t) * i), sizeof(kevent_tasks_t) * i);
                self->Tasks = (kevent_tasks_t**)newPos;
            }
        }

        if(self->Type == EventTypeIRQLines && self->NumTask == 0){
            IRQLinekevent_t* event = (IRQLinekevent_t*)self;
            if(event->IsEnable){
                event->IsEnable = false;
                APIC::IoChangeIrqState(event->IRQLine, 0, event->IsEnable);
            }
        }
        AtomicRelease(&self->Lock);
        return KSUCCESS;
    }
    
    uint64_t Trigger(kevent_t* self, arguments_t* parameters){
        AtomicAquire(&self->Lock);
        for(size64_t i = 0; i < self->NumTask; i++){
            kevent_tasks_t* task = self->Tasks[i];
            AtomicAquire(&task->thread->EventLock);
            if(task->thread->IsClose){
                task->DataNode->CurrentData->Task = task;
                task->thread->Launch(parameters);
            }else{
                if(!task->IgnoreMissedEvents){
                    task->DataNode->LastData = task->DataNode->LastData->Next;
                    task->DataNode->LastData->Next = (event_data_t*)kmalloc(sizeof(event_data_t));
                    task->DataNode->LastData->Task = task;
                    if(parameters != NULL){
                        memcpy(&task->DataNode->LastData->Parameters, parameters, sizeof(arguments_t));
                    }else{
                        memset(&task->DataNode->LastData->Parameters, 0, sizeof(arguments_t));
                    }

                    task->DataNode->NumberOfMissedEvents++;
                }
            }
            AtomicRelease(&task->thread->EventLock);
        }
        AtomicRelease(&self->Lock);

        return KSUCCESS;
    } 
    
    uint64_t TriggerIRQ(kevent_t* self){
        AtomicAquire(&self->Lock);
        for(size64_t i = 0; i < self->NumTask; i++){
            kevent_tasks_t* task = self->Tasks[i];
            AtomicAquire(&task->thread->EventLock);
            if(task->thread->IsClose){
                AtomicAquire(&globalTaskManager->SchedulerLock);
                task->thread->Launch_WL(&task->DataNode->Event->Parameters);
                AtomicRelease(&globalTaskManager->SchedulerLock);
            }else{
                if(!task->IgnoreMissedEvents){
                    task->DataNode->IRQNumberOfMissedEvents++;
                }
            }
            AtomicRelease(&task->thread->EventLock);
        }
        AtomicRelease(&self->Lock);

        return KSUCCESS;
    } 

    uint64_t Close(ContextStack* Registers, kthread_t* Thread){
        AtomicAquireCli(&Thread->EventLock);

        if(Thread->EventDataNode->IRQNumberOfMissedEvents){
            Thread->EventDataNode->IRQNumberOfMissedEvents--;
            globalTaskManager->AcquireScheduler();
            Thread->ResetContext(Thread->Regs);
            Thread->Launch_WL(&Thread->EventDataNode->Event->Parameters);
            AtomicRelease(&Thread->EventLock);
            ForceSelfDestruction();
        }else if(Thread->EventDataNode->NumberOfMissedEvents){
            event_data_t* Next = Thread->EventDataNode->CurrentData->Next;
            kfree(Thread->EventDataNode->CurrentData);
            Thread->EventDataNode->CurrentData = Next;
            Thread->EventDataNode->NumberOfMissedEvents--;
            globalTaskManager->AcquireScheduler();
            Thread->ResetContext(Thread->Regs);
            Thread->Launch_WL(&Thread->EventDataNode->CurrentData->Parameters);
            AtomicRelease(&Thread->EventLock);
            ForceSelfDestruction();
        }else{
            globalTaskManager->AcquireScheduler();
            Thread->ResetContext(Thread->Regs);
            Thread->IsBlock = true;
            Thread->IsClose = true;
            Thread->IsPause = false;
            AtomicRelease(&Thread->EventLock);
            ForceSelfDestruction();
        }

        return KSUCCESS;
    }
}