#include <kot/sys.h>

__attribute__((section(".KotSpecificData"))) struct KotSpecificData_t KotSpecificData;

KResult Sys_CreateMemoryField(process_t self, size64_t size, uintptr_t* virtualAddressPointer, ksmem_t* keyPointer, enum MemoryFieldType type){
    return Syscall_40(KSys_CreateMemoryField, self, size, virtualAddressPointer, keyPointer, type);
}

KResult Sys_AcceptMemoryField(process_t self, ksmem_t key, uintptr_t* virtualAddressPointer){
    return Syscall_24(KSys_AcceptMemoryField, self, key, virtualAddressPointer);
}

KResult Sys_CloseMemoryField(process_t self, ksmem_t key, uintptr_t address){
    return Syscall_24(KSys_CloseMemoryField, self, key, address);
}

KResult Sys_GetInfoMemoryField(ksmem_t key, uint64_t* typePointer, size64_t* sizePointer){
    return Syscall_24(KSys_GetTypeMemoryField, key, typePointer, sizePointer);
}

KResult Sys_CreateProc(process_t* key, enum Priviledge privilege, uint64_t data){
    return Syscall_24(KSys_CreateProc, key, privilege, data);
}

KResult Sys_CloseProc(){
    return Syscall_0(KSys_CloseProc);
}

KResult Sys_Close(uint64_t errorCode){
    return Syscall_8(KSys_Close, errorCode);
}

KResult Sys_Exit(uint64_t errorCode){
    return Syscall_8(KSys_Exit, errorCode);
}

KResult Sys_Pause(bool force){
    return Syscall_8(KSys_Pause, force);
}

KResult Sys_Unpause(thread_t self){
    return Syscall_8(KSys_UnPause, self);
}

KResult Sys_Map(process_t self, uint64_t* addressVirtual, enum AllocationType type, uintptr_t* addressPhysical, size64_t* size, bool findFree){
    return Syscall_48(KSys_Map, self, addressVirtual, type, addressPhysical, size, findFree);
}

KResult Sys_Unmap(thread_t self, uintptr_t addressVirtual, size64_t size){
    return Syscall_24(KSys_Unmap, self, addressVirtual, size);
}

uintptr_t Sys_GetPhysical(uintptr_t addressVirtual){
    return Syscall_8(KSys_GetPhysical, addressVirtual);
}

KResult Sys_Event_Create(event_t* self){
    return Syscall_8(KSys_Event_Create, self);
}

KResult Sys_Event_Bind(event_t self, thread_t task, bool IgnoreMissedEvents){
    return Syscall_24(KSys_Event_Bind, self, task, IgnoreMissedEvents);
}

KResult Sys_Event_Unbind(event_t self, thread_t task){
    return Syscall_16(KSys_Event_Unbind, self, task);
}

KResult Sys_Event_Trigger(event_t self, struct arguments_t* parameters){
    return Syscall_16(KSys_Event_Trigger, self, parameters);
}

KResult Sys_Event_Close(){
    return Syscall_0(KSys_Event_Close);
}

KResult Sys_CreateThread(process_t self, uintptr_t entryPoint, enum Priviledge privilege, uint64_t externalData, thread_t* result){
    return Syscall_40(KSys_CreateThread, self, entryPoint, privilege, externalData, result);
}

KResult Sys_Duplicatethread(process_t parent, thread_t source, thread_t* self){
    return Syscall_24(KSys_DuplicateThread, parent, source, self);
}

KResult Sys_ExecThread(thread_t self, struct arguments_t* parameters, enum ExecutionType type, struct ShareDataWithArguments_t* data){
    return Syscall_32(KSys_ExecThread, self, parameters, type, data);
}

KResult Sys_Keyhole_CloneModify(key_t source, key_t* destination, process_t target, uint64_t flags, enum Priviledge privilidge){
    return Syscall_40(KSys_Keyhole_CloneModify, source, destination, target, flags, privilidge);
}

KResult Sys_Keyhole_Verify(key_t self, enum DataType type, process_t* target, uint64_t* flags, uint64_t* priviledge){
    return Syscall_40(KSys_Keyhole_Verify, self, type, target, flags, priviledge);
}

KResult Sys_Logs(char* message, size64_t size){
    return Syscall_16(KSys_Logs, message, size);
}

void Sys_Schedule(){
    asm("int $0x41");
}


thread_t Sys_Getthread(){
    /* Get Self Data */
    uint64_t self = NULL;
    asm("mov %%gs:0x0, %0":"=r"(self));
    return self;
}

process_t Sys_GetProcess(){
    /* Get Self Data */
    uint64_t self;
    asm("mov %%gs:0x8, %0":"=r"(self));
    return self;
}

uint64_t Sys_GetPID(){
    /* Get Self Data */
    uint64_t PID = NULL;
    asm("mov %%gs:0x10, %0":"=r"(PID));
    return PID;
}

uint64_t Sys_GetTID(){
    /* Get Self Data */
    uint64_t TID = NULL;
    asm("mov %%gs:0x18, %0":"=r"(TID));
    return TID;
}

uint64_t Sys_GetExternalDataThread(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x20, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t Sys_GetExternalDataProcess(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x28, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t Sys_GetPriviledgeThread(){
    /* Get Self Data */
    uint64_t Priviledge = NULL;
    asm("mov %%gs:0x30, %0":"=r"(Priviledge));
    return Priviledge;
}

uint64_t Sys_GetPIDThreadCreator(){
    /* Get Self Data */
    uint64_t PID = NULL;
    asm("mov %%gs:0x38, %0":"=r"(PID));
    return PID;
}

uint64_t Sys_GetTIDThreadCreator(){
    /* Get Self Data */
    uint64_t TID = NULL;
    asm("mov %%gs:0x40, %0":"=r"(TID));
    return TID;
}

uint64_t Sys_GetExternalDataProcessCreator(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x48, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t Sys_GetPriviledgeThreadCreator(){
    /* Get Self Data */
    uint64_t Priviledge = NULL;
    asm("mov %%gs:0x50, %0":"=r"(Priviledge));
    return Priviledge;
}

uint64_t Sys_GetPIDThreadLauncher(){
    /* Get Self Data */
    uint64_t PID = NULL;
    asm("mov %%gs:0x58, %0":"=r"(PID));
    return PID;
}

uint64_t Sys_GetTIDThreadLauncher(){
    /* Get Self Data */
    uint64_t TID = NULL;
    asm("mov %%gs:0x60, %0":"=r"(TID));
    return TID;
}

uint64_t Sys_GetExternalDataProcessLauncher(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x68, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t Sys_GetPriviledgeThreadLauncher(){
    /* Get Self Data */
    uint64_t Priviledge = NULL;
    asm("mov %%gs:0x70, %0":"=r"(Priviledge));
    return Priviledge;
}


KResult Printlog(char* message){
    return Sys_Logs(message, strlen(message));
}
