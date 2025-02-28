#include <core/main.h>

void GetMemory(){
    Printlog("[Test] Call another function");
    ksmem_t MemoryShare = NULL;
    uintptr_t address = GetFreeAlignedSpace(0x1000);
    ksmem_t key = NULL;
    process_t proc = Sys_GetProcess();
    Sys_CreateMemoryField(proc, 0x1000, &address, &key, MemoryFieldTypeShareSpaceRO);
    char* testchar = "Share from another process\0";
    memcpy(address, testchar, strlen(testchar) + 1);
    ksmem_t KeyShare = NULL;
    Sys_Keyhole_CloneModify(key, &KeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);
    SYS_Close(KeyShare);
}

extern "C" int main(int argc, char* argv[]){
    Printlog("[Test] Hello world");

    process_t proc = Sys_GetProcess();

    thread_t GetMemoryThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&GetMemory, PriviledgeApp, NULL, &GetMemoryThread);

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_test_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_test_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_test_t* TestSrv = (uisd_test_t*)address;
    TestSrv->ControllerHeader.IsReadWrite = false;
    TestSrv->ControllerHeader.Version = Test_Srv_Version;
    TestSrv->ControllerHeader.VendorID = Kot_VendorID;
    TestSrv->ControllerHeader.Type = ControllerTypeEnum_Test;

    TestSrv->GetMemory = MakeShareableThread(GetMemoryThread, PriviledgeApp);

    CreateControllerUISD(ControllerTypeEnum_Test, key, true);

    Printlog("[Test] End");
}