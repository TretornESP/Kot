#pragma once

#include <main/main.h>
#include <core/core.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/storage/device.h>

#define GUIDToListenSize 0x1

void InitializeVolumeListener();
void ListenerEvent(uint64_t VolumeID, struct srv_storage_space_info_t* StorageSpace);