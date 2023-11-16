#include "radio_device_loader.h"

#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

static void radio_device_loader_power_on() {
    uint8_t attempts = 5;
    while(--attempts > 0) {
        if(furi_hal_power_enable_otg()) break;
    }
    if(attempts == 0) {
        if(furi_hal_power_get_usb_voltage() < 4.5f) {
            FURI_LOG_E(
                "FjoneSubGhz",
                "Error power otg enable. BQ2589 check otg fault = %d",
                furi_hal_power_check_otg_fault() ? 1 : 0);
        }
    }
}

static void radio_device_loader_power_off() {
    if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();
}

const SubGhzDevice* radio_device_loader_set(uint32_t device_ind) {
    const SubGhzDevice* device = NULL;
    FURI_LOG_I("Fjone", "radio_device_loader_set: try to init: %ld",device_ind);
    switch(device_ind) {
    case 1:
        radio_device_loader_power_on();
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_EXT_NAME);
        break;

    default:
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
        break;
    }
    //check if the device is connected
    if(!subghz_devices_is_connect(device)) {
        radio_device_loader_power_off();
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
        device_ind = 0;
    }
    FURI_LOG_I("Fjone", "radio_device_loader_set: the device inited is: %ld",device_ind);
    return device;
}

void radio_device_loader_end(const SubGhzDevice* radio_device) {
    furi_assert(radio_device);
    radio_device_loader_power_off();
    // Code below is not used (and will cause crash) since its called from tx_rx worker end!
    //if(radio_device != subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME)) {
    //    subghz_devices_end(radio_device);
    //}
}