#pragma once

#include <kot/types.h>
#include <kot/memory.h>
#include <kot/cstring.h>
#include <kot/uisd/srvs/system.h>

struct RSDP2{
    uint8_t Signature[8];
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t Revision;
    uint32_t RSDTAddress;
    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
}__attribute__((packed));

struct RSDT{
    struct srv_system_sdtheader_t header;
    uint32_t SDTPointer[];
}__attribute__((packed));

struct XSDT{
    struct srv_system_sdtheader_t header;
    uint64_t SDTPointer[];
}__attribute__((packed));

uintptr_t ParseRSDP(uintptr_t rsdpPhysical);

uint64_t FindTableIndex(char* signature);
uintptr_t GetTablePhysicalAddress(uint64_t index);
size64_t GetTableSize(uint64_t index);