#ifndef _SRV_SYSTEM_H
#define _SRV_SYSTEM_H 1

#include <kot/types.h>
#include <kot/uisd.h>
#include <kot/sys.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*SystemCallbackHandler)(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef struct {
    uint64_t Address;
    uint64_t Width;
    uint64_t Height;
    uint64_t Pitch;
    uint64_t Bpp;
} srv_system_framebuffer_t;

struct srv_system_sdtheader_t {
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
}__attribute__((packed));

struct srv_system_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    SystemCallbackHandler Handler;
};

void Srv_System_Initialize();

void Srv_System_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_system_callback_t* Srv_System_LoadExecutable(uint64_t Priviledge, char* Path, bool IsAwait);
struct srv_system_callback_t* Srv_System_GetFramebuffer(bool IsAwait);
struct srv_system_callback_t* Srv_System_ReadFileInitrd(char* Name,  bool IsAwait);
struct srv_system_callback_t* Srv_System_GetTableInRootSystemDescription(char* Name, bool IsAwait);
struct srv_system_callback_t* Srv_System_GetSystemManagementBIOSTable(bool IsAwait);
struct srv_system_callback_t* Srv_System_BindIRQLine(uint8_t IRQLineNumber, thread_t Target, bool IgnoreMissedEvents, bool IsAwait);
struct srv_system_callback_t* Srv_System_UnbindIRQLine(uint8_t IRQLineNumber, thread_t Target, bool IsAwait);
struct srv_system_callback_t* Srv_System_BindFreeIRQ(thread_t Target, bool IgnoreMissedEvents, bool IsAwait);
struct srv_system_callback_t* Srv_System_UnbindIRQ(uint8_t Vector, thread_t Target, bool IsAwait);

#if defined(__cplusplus)
}
#endif

#endif