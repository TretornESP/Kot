#pragma once

#define Pci_Srv_Version 0x1

#define PCI_CONFIGURATION_SPACE_PCI     0x100
#define PCI_CONFIGURATION_SPACE_PCIE    0x1000

#define PCI_VENDOR_ID_OFFSET 		0x0
#define PCI_DEVICE_ID_OFFSET 		0x2
#define PCI_COMMAND_OFFSET 			0x4
#define PCI_STATUS_OFFSET 			0x6
#define PCI_REVISION_ID_OFFSET 		0x8
#define PCI_PROG_IF_OFFSET 			0x9
#define PCI_SUBCLASS_OFFSET 		0xA
#define PCI_CLASS_CODE_OFFSET 		0xB
#define PCI_CACHE_SIZE_OFFSET 		0xC
#define PCI_LATENCY_TIMER_OFFSET 	0xD
#define PCI_HEADER_TYPE_OFFSET 		0xE
#define PCI_BIST_OFFSET 			0xF

#define PCI_COMMAND_IO_SPACE            (1 << 0x0)
#define PCI_COMMAND_MEMORY_SPACE        (1 << 0x1)
#define PCI_COMMAND_BUS_MASTERING       (1 << 0x2)
#define PCI_COMMAND_INTERRUPT_DISABLE   (1 << 0xA)

#include <kot/sys.h>
#include <kot/arch.h>
#include <kot/heap.h>
#include <kot/utils.h>
#include <kot/memory.h>
#include <kot/cstring.h>
#include <kot++/stack.h>
#include <kot++/printf.h>
#include <kot/uisd/srvs/pci.h>
#include <kot/uisd/srvs/system.h>

#include <srv/srv.h>
#include <pci/pci.h>
#include <pcie/pcie.h>

/* PCI header */

struct PCIDeviceHeader_t{
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgIF;
    uint8_t Subclass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
}__attribute__((packed));

struct PCIHeader0_t{
    PCIDeviceHeader_t Header;
    uint32_t BAR[6];
    uint32_t CardBusCISPtr;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemID;
    uint32_t ExpansionRomBaseAddr;
    uint8_t CapabilitiesPtr;
    uint8_t  Reserved0;
    uint16_t Reserved1;
    uint32_t Reserved2;
    uint8_t IntreruptLine;
    uint8_t IntreruptPin;
    uint8_t MinGrant;
    uint8_t MaxLatency;
}__attribute__((packed));

struct PCIHeader1_t{
    PCIDeviceHeader_t Header;
    /* TODO */
}__attribute__((packed));

/* Capabilities */

enum PCICapabilities{
    PCICapabilitiesMSI = 0x5,
    PCICapabilitiesMSIX = 0x11,
};

struct PCICapabilityMSI_t{
    uint16_t Control;
    uint64_t Address;
    uint16_t Data;
    uint16_t Reserved;
    uint32_t Mask;
    uint32_t Pending;
}__attribute__((packed));

struct PCIMSIXTable_t{
    uint64_t Address;
    uint32_t Data;
    uint32_t Control;
}__attribute__((packed));

struct PCICapabilityMSIX_t{
    uint16_t Control;
    uint8_t BIR:2;
    uint32_t TableOffset:30;
    uint8_t PendingBitBIR:2;
    uint32_t PendingBitOffset:30;
}__attribute__((packed));

struct PCICapability_t{
    uint8_t CapabilityID;
    uint8_t CapabilityNext;

    union{
        PCICapabilityMSI_t MSI;
        PCICapabilityMSIX_t MSIX;
    };
}__attribute__((packed));

/* Internal driver headers */

struct PCIDeviceListInfo_t{
    std::Stack* Devices;
    size64_t DevicesNum;
};

struct PCIDeviceArrayInfo_t{
    struct PCIDevice_t** Devices;
    PCIDeviceID_t DevicesNum;
};

struct PCIBar_t{
    uint64_t Base;
    uint64_t Size;
    uint8_t Type;
};

struct PCIDevice_t{
    bool IsPCIe;
    uint64_t Address; // PCI only
    uintptr_t ConfigurationSpaceBack; // PCI only
    uintptr_t ConfigurationSpace;

    /* Device functions */
    uintptr_t GetBarAddress(uint8_t index);
    size64_t GetBarSizeWithAddress(uintptr_t address);
    size64_t GetBarSize(uint8_t index);
    uint8_t GetBarType(uint8_t index);
    KResult BindMSI(uint8_t IRQVector, uint8_t processor, uint16_t localDeviceVector, uint64_t* version);
    KResult UnbindMSI(uint16_t localDeviceVector);

    /* Configuration space */
    KResult ConfigReadWord(uint16_t Offset, uint16_t* Value);
    KResult ConfigWriteWord(uint16_t Offset, uint16_t Value);

    /* Version specific */
    void ReceiveConfigurationSpace();
    void SendConfigurationSpace();
};

/* List functions */
PCIDeviceListInfo_t* InitPCIList();
void AddPCIDevice(PCIDeviceListInfo_t* DevicesList, struct PCIDevice_t* Device);
void ConvertListToArray(PCIDeviceListInfo_t* DevicesList, PCIDeviceArrayInfo_t* DevicesArray);

bool CheckDevice(PCIDeviceArrayInfo_t* DevicesArray, PCIDeviceID_t device);
struct PCIDevice_t* GetDeviceFromIndex(PCIDeviceArrayInfo_t* DevicesArray, PCIDeviceID_t device);
uint64_t Search(PCIDeviceArrayInfo_t* DevicesArray, uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIF);
PCIDeviceID_t GetDevice(PCIDeviceArrayInfo_t* DevicesArray, uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIF, PCIDeviceID_t index);