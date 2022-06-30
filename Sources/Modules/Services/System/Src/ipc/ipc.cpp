#include <ipc/ipc.h>

kthread_t threadIPC;

kthread_t IPCInitialize(){
    kthread_t IPCThreadKey;
    uint64_t IPCKeyFlags = NULL;
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypeThreadIsExecutableAsCIP, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypeThreadIsDuplicable, true);

    kprocess_t process;
    SYS_GetProcessKey(&process);

    Sys_CreateThread(process, (uintptr_t)IPCHandler, PriviledgeService, NULL, &threadIPC);
    Sys_Keyhole_CloneModify(threadIPC, &IPCThreadKey, NULL, IPCKeyFlags);

    return IPCThreadKey;
}

KResult IPCHandler(){
    Printlog("IPC");
    SYS_Exit(NULL, KSUCCESS);
}