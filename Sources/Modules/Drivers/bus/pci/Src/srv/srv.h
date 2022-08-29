#pragma once
#include <core/main.h>

void InitSrv(struct PCIDeviceArrayInfo_t* DevicesArray);

KResult CountDevices(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters);
KResult FindDevice(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters, uint64_t Index);

KResult GetInfoDevice(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device);
KResult GetBARDevice(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device, uint8_t BarIndex);