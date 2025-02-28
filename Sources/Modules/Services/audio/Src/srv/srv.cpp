#include <srv/srv.h>

uisd_audio_t* SrvData;

KResult InitialiseServer(){
    process_t proc = Sys_GetProcess();

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_audio_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_audio_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_audio_t*)address;
    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Audio_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Audio;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);


    /* AddDevice */
    thread_t AddDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&AddDevice, PriviledgeApp, NULL, &AddDeviceThread);
    SrvData->AddDevice = MakeShareableThread(AddDeviceThread, PriviledgeDriver);
    
    CreateControllerUISD(ControllerTypeEnum_Audio, key, true);
    return KSUCCESS;
}

KResult AddDevice(thread_t Callback, uint64_t CallbackArg, srv_audio_device_t* Device){
    KResult Status = KFAIL;

    if(Device != NULL){
        // TODO : in
        switch(Device->Type){
            case AudioDeviceTypeOut:{
                Status = AddOutputDevice(Device);
                break;
            }
            default:{
                break;
            }
        }
    }

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

thread_t ChangeStatusCallbackThread = NULL;

void ChangeStatusCallback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Status;
    Sys_Unpause(Callback->Self);
    Sys_Close(KSUCCESS);
}

CallbackAudio* ChangeStatus(srv_audio_device_t* Device, enum AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(!ChangeStatusCallbackThread){
        thread_t AudioThreadKeyCallback = NULL;
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&ChangeStatusCallback, PriviledgeDriver, NULL, &AudioThreadKeyCallback);
        ChangeStatusCallbackThread = MakeShareableThread(AudioThreadKeyCallback, PriviledgeDriver);
    }

    CallbackAudio* Callback = (CallbackAudio*)malloc(sizeof(CallbackAudio));
    Callback->Self = Sys_Getthread();
    Callback->Status = KBUSY;

    struct arguments_t parameters{
        .arg[0] = ChangeStatusCallbackThread,
        .arg[1] = (uint64_t)Callback,
        .arg[2] = Function,
        .arg[3] = GP0,
        .arg[4] = GP1,
        .arg[5] = GP2,
    };

    KResult Status = Sys_ExecThread(Device->ChangeStatus, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS){
        Sys_Pause(false);
    }
    return Callback;
}

KResult SetVolume(srv_audio_device_t* Device, uint8_t Volume){
    CallbackAudio* Callback = ChangeStatus(Device, AudioSetStatusVolume, Volume, 0, 0);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}