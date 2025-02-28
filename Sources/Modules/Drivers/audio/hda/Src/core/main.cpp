#include <core/main.h>

process_t Proc = NULL;

extern "C" int main(int argc, char* argv[]) {
    Printlog("[AUDIO/HDA] Initialization ...");

    srv_pci_search_parameters_t SearchParameters{
        .vendorID = PCI_SEARCH_NO_PARAMETER,
        .deviceID = PCI_SEARCH_NO_PARAMETER,
        .classID = HDA_CLASS,
        .subClassID = HDA_SUBCLASS,
        .progIF = PCI_SEARCH_NO_PARAMETER,
    };

    Proc = Sys_GetProcess();

    srv_pci_callback_t* Callback = Srv_Pci_CountDevices(&SearchParameters, true);
    uint64_t DevicesNumber = (uint64_t)Callback->Data;
    free(Callback);

    for(uint64_t i = 0; i < DevicesNumber; i++){
        Callback = Srv_Pci_FindDevice(&SearchParameters, i, true);
        PCIDeviceID_t DeviceID = (PCIDeviceID_t)Callback->Data;
        free(Callback);
        new HDAController(DeviceID);
    }

    Printlog("[AUDIO/HDA] Driver initialized successfully");

    return KSUCCESS;
}