#include "iton.h"
#include "ch.h"
#include "hal.h"
#include "host.h"
#include "host_driver.h"
#include "report.h"
#include "driver.h"
#include "quantum.h"

static uint8_t iton_bt_leds(void) {
    // TODO: Process leds
    return 0;
}

static void iton_bt_mouse(report_mouse_t *report) {
    // Not supported by bt fw
}

static void iton_bt_system(uint16_t data) {
    iton_system_report((uint8_t)data);
}

static void iton_bt_consumer(uint16_t data) {
    iton_consumer_report(data);
}


static void iton_bt_keyboard(report_keyboard_t *report) {
#ifdef NKRO_ENABLE
    if (!keymap_config.nkro) {
#endif
        iton_hid_report(&report->raw[0]);
#ifdef NKRO_ENABLE
        return;
    }
    static uint8_t iton_last_mod_sent = 0;

    if (iton_last_mod_sent != report->nkro.mods) {
        static uint8_t iton_mod_report[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        iton_mod_report[0] = report->nkro.mods;
        iton_hid_report(iton_mod_report);
        iton_last_mod_sent = report->nkro.mods;
    }
    iton_nkro_report(&report->nkro.bits[0]);
#endif
}

static host_driver_t iton_bt_driver = {
    iton_bt_leds,
    iton_bt_keyboard,
    iton_bt_mouse,
    iton_bt_system,
    iton_bt_consumer,
};

static host_driver_t *last_driver = NULL;

bool iton_is_bt() {
    return host_get_driver() == &iton_bt_driver;
}

void iton_switch_driver() {
    clear_keyboard();
    if (host_get_driver() == &iton_bt_driver) {
        host_set_driver(last_driver);
        iton_mode_usb();
        return;
    }
    #ifdef NKRO_ENABLE
    if (!keymap_config.nkro) {
        // Allows more than 5 keys over bt
        keymap_config.nkro = true;
    }
    #endif
    iton_mode_bt();
    last_driver = host_get_driver();
    host_set_driver(&iton_bt_driver);
}
