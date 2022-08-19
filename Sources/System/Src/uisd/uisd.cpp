#include <uisd/uisd.h>

thread UISDHandlerThread;

controller_info_t** UISDControllers;

thread UISDInitialize(process_t* process) {
    thread UISDthreadKey;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypethreadIsExecutableWithQueue, true);

    process_t proc;
    Sys_GetProcessKey(&proc);

    Sys_Createthread(proc, (uintptr_t)UISDHandler, PriviledgeService, NULL, &UISDHandlerThread);
    Sys_Keyhole_CloneModify(UISDHandlerThread, &UISDthreadKey, NULL, UISDKeyFlags);

    UISDControllers = (controller_info_t**)calloc(sizeof(controller_info_t) * UISDMaxController);

    UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Sys_Keyhole_CloneModify(proc, process, NULL, UISDKeyFlags);

    return UISDthreadKey;

}

void UISDAddToQueu(enum ControllerTypeEnum Controller, thread Callback, uint64_t Callbackarg, process_t Self, uintptr_t Address){
    if(!UISDControllers[Controller]){
        UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
        UISDControllers[Controller]->IsLoad = false;
        UISDControllers[Controller]->WaitingTasks = new std::Stack(0x50);
        UISDControllers[Controller]->NumberOfWaitingTasks = NULL;
    }
    
    callbackget_info_t* callbackInfo = (callbackget_info_t*)malloc(sizeof(callbackget_info_t));
    callbackInfo->Controller = Controller;
    callbackInfo->Self = Self;
    callbackInfo->Address = Address;
    callbackInfo->Callback = Callback;
    callbackInfo->Callbackarg = Callbackarg;
    UISDControllers[Controller]->WaitingTasks->push64((uint64_t)callbackInfo);
    UISDControllers[Controller]->NumberOfWaitingTasks++;
}

void UISDAccept(callbackget_info_t* callback){
    KResult Statu = Sys_AcceptMemoryField(callback->Self, UISDControllers[callback->Controller]->DataKey, &callback->Address);
    arguments_t parameters{
        .arg[0] = UISDGetTask,
        .arg[1] = (uint64_t)Statu,
        .arg[2] = callback->Callbackarg,
        .arg[3] = (uint64_t)callback->Address,
    };
    Sys_Execthread(callback->Callback, &parameters, ExecutionTypeQueu, NULL);        
}

void UISDAcceptAll(enum ControllerTypeEnum Controller){
    for(uint64_t i = 0; i < UISDControllers[Controller]->NumberOfWaitingTasks; i++){
        callbackget_info_t* callback = (callbackget_info_t*)UISDControllers[Controller]->WaitingTasks->pop64();
        UISDAccept(callback);
        free(callback);
    }
}

KResult UISDCreate(enum ControllerTypeEnum Controller, thread callback, uint64_t callbackarg, ksmem_t DataKey) {
    KResult Statu = KFAIL;
    if(UISDControllers[Controller] == NULL || !UISDControllers[Controller]->IsLoad){
        enum MemoryFieldType Type;
        size_t Size = NULL;
        process_t Target = NULL;
        uint64_t Flags = NULL;
        if(Sys_Keyhole_Verify(DataKey, DataTypeSharedMemory, &Target, &Flags) != KSUCCESS) return KKEYVIOLATION;
        
        if(Sys_GetInfoMemoryField(DataKey, (uint64_t*)&Type, &Size) == KSUCCESS){
            if(Type == MemoryFieldTypeSendSpaceRO){
                if(!UISDControllers[Controller]){
                    UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
                    UISDControllers[Controller]->NumberOfWaitingTasks = NULL;
                }
                UISDControllers[Controller]->DataKey = DataKey;
                UISDControllers[Controller]->Data = getFreeAlihnedSpace(Size);
                if(Sys_AcceptMemoryField(proc, DataKey, (uintptr_t*)&UISDControllers[Controller])){
                    UISDControllers[Controller]->IsLoad = true;
                    UISDAcceptAll(Controller);
                    Statu = KSUCCESS;
                }
            }
        }
    }
    arguments_t parameters{
        .arg[0] = UISDCreateTask,
        .arg[1] = (uint64_t)Statu,
        .arg[2] = callbackarg,
    };
    Sys_Execthread(callback, &parameters, ExecutionTypeQueu, NULL);    
    return Statu;
}

KResult UISDGet(enum ControllerTypeEnum Controller, thread Callback, uint64_t Callbackarg, process_t Self, uintptr_t Address) {
    process_t Target = NULL;
    uint64_t Flags = NULL;
    if(Sys_Keyhole_Verify(Self, DataTypeSharedMemory, &Target, &Flags) != KSUCCESS) return NULL;
    if(!Keyhole_GetFlag(Flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return NULL;
    if(UISDControllers[Controller] == NULL){
        if(UISDControllers[Controller]->IsLoad){
            struct callbackget_info_t info = (struct callbackget_info_t){
                .Controller = Controller,
                .Self = Self,
                .Address = Address,
                .Callback = Callback,
                .Callbackarg = Callbackarg,
            };
            UISDAccept(&info);
            return KSUCCESS;
        }else{
            UISDAddToQueu(Controller, Callback, Callbackarg, Self, Address);
        }
    }else{
        UISDAddToQueu(Controller, Callback, Callbackarg, Self, Address);
    }
    return NULL;
}

void UISDHandler(uint64_t IPCTask, enum ControllerTypeEnum Controller, thread Callback, uint64_t Callbackarg, uint64_t GP0, uint64_t GP1) {
    uint64_t ReturnValue = NULL;
    switch (IPCTask) {
    case UISDCreateTask:
        ReturnValue = (uint64_t)UISDCreate(Controller, Callback, Callbackarg, (ksmem_t)GP0);
        break;
    case UISDGetTask:
        ReturnValue = (uint64_t)UISDGet(Controller, Callback, Callbackarg, (process_t)GP0, (uintptr_t)GP1);
        break;
    case UISDFreeTask:
        break;
    }
    SYS_Exit(NULL, ReturnValue);
}