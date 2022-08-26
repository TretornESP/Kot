#pragma once
#include <core/main.h>
#include <kot/uisd/srvs/system.h>

#define System_Srv_Version 0x1

void InitializeSrv(struct KernelInfo* kernelInfo);

KResult GetFrameBuffer(thread_t Callback, uint64_t CallbackArg);
KResult ReadFileFromInitrd(thread_t Callback, uint64_t CallbackArg, char* Name);
KResult GetTableInRootSystemDescription(thread_t Callback, uint64_t CallbackArg, char* Name);
KResult GetSystemManagementBIOSTable(thread_t Callback, uint64_t CallbackArg);
KResult BindIRQLine(thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, thread_t Target, bool IgnoreMissedEvents);
KResult BindFreeIRQ(thread_t Callback, uint64_t CallbackArg);

struct SrvInfo_t{
    srv_system_framebuffer_t* Framebuffer;

    uintptr_t Smbios;

    uintptr_t Rsdp;

    uint8_t IRQLineStart;
    uint8_t IRQLineSize;
    
    size64_t IRQSize;
    event_t* IRQEvents; 
    bool* IsIRQEventsFree;
}__attribute__((packed));