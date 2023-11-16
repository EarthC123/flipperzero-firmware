#pragma once

#include <lib/subghz/devices/devices.h>

/** SubGhzRadioDeviceType */
typedef enum {
    SubGhzRadioDeviceTypeInternal,
    SubGhzRadioDeviceTypeExternalCC1101,
} SubGhzRadioDeviceType;

const SubGhzDevice* radio_device_loader_set(uint32_t device_ind);

void radio_device_loader_end(const SubGhzDevice* radio_device);