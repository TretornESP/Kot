#include <kot/uisd.h>

uintptr_t ControllerList[ControllerCount];

size64_t ControllerTypeSize[ControllerCount] = {
    sizeof(uisd_system_t),
    sizeof(uisd_time_t),
    sizeof(uisd_hid_t),
    sizeof(uisd_graphics_t),
    sizeof(uisd_storage_t),
    sizeof(uisd_audio_t),
    sizeof(uisd_usb_t),
    sizeof(uisd_pci_t)
};

thread_t CallBackUISDThread = NULL;
process_t ProcessKeyForUISD = NULL;

KResult CallbackUISD(uint64_t Task, KResult Status, uisd_callbackInfo_t* Info, uint64_t GP0, uint64_t GP1);

KResult InitializeUISD(){
    thread_t UISDthreadKeyCallback;
    uint64_t UISDKeyFlags = NULL;

    process_t Proc = Sys_GetProcess();

    Sys_CreateThread(Proc, &CallbackUISD, PriviledgeApp, NULL, &UISDthreadKeyCallback);
    CallBackUISDThread = MakeShareableThreadToProcess(UISDthreadKeyCallback, KotSpecificData.UISDHandlerProcess);

    Sys_Keyhole_CloneModify(Proc, &ProcessKeyForUISD, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent | KeyholeFlagDataTypeProcessMemoryAccessible, PriviledgeApp);
    
    memset(&ControllerList, NULL, ControllerCount * sizeof(uintptr_t));
    
    return KSUCCESS;
}

KResult CallbackUISD(uint64_t Task, KResult Status, uisd_callbackInfo_t* Info, uint64_t GP0, uint64_t GP1){
    if(Task == UISDGetTask){
        ControllerList[Info->Controller] = (uintptr_t)GP0;
        Info->Location = GP0;
    } 
    Info->Status = Status;
    if(Info->AwaitCallback){
        Sys_Unpause(Info->Self);
    }
    Sys_Close(KSUCCESS);
}

uisd_callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback){
    if(!CallBackUISDThread) InitializeUISD();
    thread_t Self = Sys_Getthread();
    uisd_callbackInfo_t* Info = (uisd_callbackInfo_t*)malloc(sizeof(uisd_callbackInfo_t));
    Info->Self = Self;
    Info->Controller = Controller;
    Info->AwaitCallback = AwaitCallback;
    Info->Location = NULL;
    Info->Status = KBUSY;

    struct arguments_t parameters;
    parameters.arg[0] = UISDGetTask;
    parameters.arg[1] = Controller;
    parameters.arg[2] = CallBackUISDThread;
    parameters.arg[3] = Info;
    parameters.arg[4] = ProcessKeyForUISD;
    parameters.arg[5] = (uint64_t)*Location;
    KResult Status = Sys_ExecThread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && AwaitCallback){
        Sys_Pause(false);
        *Location = Info->Location;
        return Info;
    }
    return Info;
}

uisd_callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, ksmem_t MemoryField, bool AwaitCallback){
    if(!CallBackUISDThread) InitializeUISD();
    thread_t Self = Sys_Getthread();
    uisd_callbackInfo_t* Info = malloc(sizeof(uisd_callbackInfo_t));
    Info->Self = Self;
    Info->Controller = Controller;
    Info->AwaitCallback = AwaitCallback;
    Info->Status = KBUSY;

    ksmem_t MemoryFieldKey = NULL;
    Sys_Keyhole_CloneModify(MemoryField, &MemoryFieldKey, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent, PriviledgeApp);

    struct arguments_t parameters;
    parameters.arg[0] = UISDCreateTask;
    parameters.arg[1] = Controller;
    parameters.arg[2] = CallBackUISDThread;
    parameters.arg[3] = Info;
    parameters.arg[4] = MemoryFieldKey;
    KResult Status = Sys_ExecThread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && AwaitCallback){
        Sys_Pause(false);
        return Info;
    }
    return Info;
}

/* Useful functions */

thread_t MakeShareableThread(thread_t Thread, enum Priviledge priviledgeRequired){
    thread_t ReturnValue;
    Sys_Keyhole_CloneModify(Thread, &ReturnValue, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

thread_t MakeShareableThreadUISDOnly(thread_t Thread){
    thread_t ReturnValue;
    Sys_Keyhole_CloneModify(Thread, &ReturnValue, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

thread_t MakeShareableThreadToProcess(thread_t Thread, process_t Process){
    thread_t ReturnValue;
    Sys_Keyhole_CloneModify(Thread, &ReturnValue, Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

thread_t MakeShareableSpreadThreadToProcess(thread_t Thread, process_t Process){
    thread_t ReturnValue;
    Sys_Keyhole_CloneModify(Thread, &ReturnValue, Process, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

process_t ShareProcessKey(process_t Process){
    process_t ReturnValue;
    Sys_Keyhole_CloneModify(Process, &ReturnValue, NULL, KeyholeFlagPresent, PriviledgeApp);
    return ReturnValue;
}

uintptr_t GetControllerLocationUISD(enum ControllerTypeEnum Controller){
    return ControllerList[Controller];
}

uintptr_t FindControllerUISD(enum ControllerTypeEnum Controller){
    uintptr_t ControllerData = GetControllerLocationUISD(Controller);
    if(!ControllerData){
        ControllerData = GetFreeAlignedSpace(ControllerTypeSize[Controller]);
        uisd_callbackInfo_t* Info = GetControllerUISD(Controller, &ControllerData, true);
        free(Info);
    }
    return ControllerData;
}

