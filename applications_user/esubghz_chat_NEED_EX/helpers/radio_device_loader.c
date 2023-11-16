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
                "SubGhz",
                "Error power otg enable. BQ2589 check otg fault = %d",
                furi_hal_power_check_otg_fault() ? 1 : 0);
        }
    }
}

static void radio_device_loader_power_off() {
    if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();
}

bool radio_device_loader_is_connect_external(const char* name) {
    bool is_connect = false;
    bool is_otg_enabled = furi_hal_power_is_otg_enabled();

    if(!is_otg_enabled) {
        radio_device_loader_power_on();
    }

    const SubGhzDevice* device = subghz_devices_get_by_name(name);
    if(device) {
        is_connect = subghz_devices_is_connect(device);
    }

    if(!is_otg_enabled) {
        radio_device_loader_power_off();
    }
    return is_connect;
}

const SubGhzDevice* radio_device_loader_set(const SubGhzDevice* current_radio_device, SubGhzRadioDeviceType radio_device_type) {
    if(radio_device_type == SubGhzRadioDeviceTypeExternalCC1101 &&
       radio_device_loader_is_connect_external(SUBGHZ_DEVICE_CC1101_EXT_NAME)) {
        radio_device_loader_power_on();
        current_radio_device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_EXT_NAME);
        subghz_devices_begin(current_radio_device);
    }else {
        radio_device_loader_power_off();
        current_radio_device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
    }

    return current_radio_device;
}

void radio_device_loader_end(const SubGhzDevice* radio_device) {
    furi_assert(radio_device);
    radio_device_loader_power_off();
    // Code below is not used (and will cause crash) since its called from tx_rx worker end!
    //if(radio_device != subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME)) {
    //    subghz_devices_end(radio_device);
    //}
}