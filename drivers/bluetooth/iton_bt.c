// Copyright 2021 1Conan (@1Conan)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include "gpio.h"
#include "config.h"
#include "SPI.h"
#include "quantum.h"
#include "outputselect.h"

#ifndef ITON_BT_IRQ_PIN
#define ITON_BT_IRQ_PIN A0
#endif

#ifndef ITON_BT_INT_PIN
#define ITON_BT_INT_PIN A1
#endif

#ifndef ITON_BT_MAX_PROFILES
#define ITON_BT_MAX_PROFILES 3
#endif

#ifdef KEYBOARD_SHARED_EP
#define HID_REPORT_OFFSET 1
#else
#define HID_REPORT_OFFSET 0
#endif

#ifndef ITON_BT5
enum iton_bt_cmd {
    // mcu to iton
    hid_report      = 0xA1,
    nkro_report     = 0xA2,
    media_report    = 0xA3,
    system_report   = 0xA4,
    fn_report       = 0xA5,
    control         = 0xA6,
    set_name        = 0xA7,
    set_alarm_volt  = 0xA8,

    // iton to mcu
    led_state       = 0xB1,
    notification    = 0xB6,
};
#else
enum iton_bt_cmd {
    // mcu to iton
    hid_report      = 0x51,
    nkro_report     = 0x52,
    media_report    = 0x53,
    system_report   = 0x54,
    fn_report       = 0x55,
    control         = 0x56,
    set_name        = 0x57,
    set_alarm_volt  = 0x58,

    // iton to mcu
    led_state       = 0x61,
    notification    = 0x66,
};
#endif

enum iton_bt_control_cmd {
    control_usb = 0x58,
    control_bt  = 0x51,
};

enum iton_bt_control_param {
    mode_usb       = 0x01,
    mode_bt        = 0x62,
    reset_pairing  = 0x70,
    enter_pairing  = 0x89,
    switch_profile = 0x81, // add 0-5 for other profiles
    os_mac         = 0x74,
    os_win         = 0x75,
};

enum iton_bt_notification_type {
    battery_notif   = 0x5A,
    bluetooth_notif = 0x51,
};

enum iton_bt_notification_param {
    // battery_notif
    voltage_low             = 0x06,
    exit_low_battery_mode   = 0x0A,
    low_power_shutdown      = 0x07,
    query_working_mode      = 0xA0,
    query_bt_name           = 0xA1,

    // bluetooth_notif
    bt_connection_success   = 0x76,
    bt_entered_pairing      = 0x77,
    bt_disconnected         = 0x78,
    bt_enters_connection    = 0x79,
};

uint8_t iton_bt_tx[35];
uint8_t iton_bt_tx_cnt = 0;
uint8_t iton_bt_tx_ptr = 0;

uint8_t iton_bt_rx[3];
uint8_t iton_bt_rx_ptr = 0;

void iton_bt_init(void) {
    setPinOutput(ITON_BT_IRQ_PIN);
    writePinLow(ITON_BT_IRQ_PIN);
    setPinInput(ITON_BT_INT_PIN);

    SPI0_Init();

#ifndef ITON_BT_WAIT_FOR_SWITCH
    SPI0_Enable();
#endif
}

void iton_bt_write(uint8_t cmd, uint8_t *data, uint8_t len) {
    while (readPin(ITON_BT_IRQ_PIN));
    iton_bt_tx[0] = cmd;
    iton_bt_tx_cnt = len + 1;
    memcpy(&iton_bt_tx[1], data, len);

    writePinHigh(ITON_BT_IRQ_PIN);
    SN_SPI0->DATA = iton_bt_tx[iton_bt_tx_ptr++];
}

void iton_bt_write2(uint8_t cmd, uint8_t b1, uint8_t b2) {
    uint8_t data[] = {b1, b2};
    iton_bt_write(cmd, &data[0], 2);
}

void iton_bt_control(uint8_t cmd, uint8_t param) {
    iton_bt_write2(control, cmd, param);
}

void iton_bt_mode_usb(void) {
    iton_bt_control(control_usb, mode_usb);
}

void iton_bt_mode_bt(void) {
    iton_bt_control(control_bt, mode_bt);
}

void iton_bt_reset_pairing(void) {
    iton_bt_control(control_bt, reset_pairing);
}

void iton_bt_enter_pairing(void) {
    iton_bt_control(control_bt, enter_pairing);
}

// profile - 1 to ITON_BT_MAX_PROFILES
void iton_bt_switch_profile(uint8_t profile) {
    if (profile >= ITON_BT_MAX_PROFILES) {
        profile = ITON_BT_MAX_PROFILES - 1;
    }

    iton_bt_control(control_bt, switch_profile + profile);
}

void iton_bt_os_mac(void) {
    iton_bt_control(control_bt, os_mac);
}

void iton_bt_os_win(void) {
    iton_bt_control(control_bt, os_win);
}

/**
 * Only implemented on newer firmware
 * pin range is 0-3
 * Pin P26 = 0
 * Pin P27 = 1
 * Pin P28 = 2
 * Pin P29 = 3
 *
 * Examples:
 * iton_bt_set_pins(ITON_BT_PIN_MASK(0) | ITON_BT_PIN_ON(0));
 */
void iton_bt_set_pins(uint8_t cfg) {
    iton_bt_control(control_bt, cfg);
}

// Only implemented on newer firmware
void iton_bt_set_name(char *name) {
    uint8_t buffer[34]; // 2 + 1 + 31
    uint8_t len = sizeof(name);
    uint16_t checksum = 0;

    if (len >= 32) {
        return;
    }

    for (uint8_t i = 0; i < len; i++) {
        checksum += (uint16_t)name[i];
    }

    buffer[0] = (uint8_t)(checksum >> 8);
    buffer[1] = (uint8_t)(checksum & 0xFF);
    buffer[2] = len;

    memcpy(&buffer[3], name, len);

    iton_bt_write(set_name, &buffer[0], len + 3);
}

void iton_bt_send_keyboard(report_keyboard_t *report) {
#ifdef NKRO_ENABLE
    if (!keymap_config.nkro) {
#endif
        iton_bt_write(hid_report, &report->raw[HID_REPORT_OFFSET], 8);
#ifdef NKRO_ENABLE
        return;
    }

    static uint8_t iton_last_mod_sent = 0;
    if (iton_last_mod_sent != report->nkro.mods) {
        uint8_t iton_mod_report[8] = {report->nkro.mods, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        iton_bt_write(hid_report, &iton_mod_report[0], 8);
        iton_last_mod_sent = report->nkro.mods;
    }
    iton_bt_write(nkro_report, &report->nkro.bits[0], 16);
#endif
}

void iton_bt_send_consumer(uint16_t data) {
    iton_bt_write2(media_report, (uint8_t)(data >> 8), (uint8_t)(data & 0x00ff));
}

void iton_bt_send_system(uint16_t data) {
    uint8_t key = (uint8_t)(data >> 8);
    iton_bt_write(system_report, &key, 1);
}

uint8_t iton_bt_keyboard_led_state  = 0x00;

bool iton_bt_is_connected = false;
bool iton_bt_in_pairing = false;
bool iton_bt_low_battery = false;

__attribute__((weak)) void iton_bt_exit_low_battery_mode(void) {}
__attribute__((weak)) void iton_bt_low_battery_notification(void) {};
__attribute__((weak)) void iton_bt_low_power_shutdown(void) {}

__attribute__((weak)) void iton_bt_bt_connection_success(void) {}
__attribute__((weak)) void iton_bt_bt_entered_pairing(void) {}
__attribute__((weak)) void iton_bt_bt_disconnected(void) {}

static inline void iton_bt_battery_notif(uint8_t param) {
    switch (param) {
        case voltage_low:
            iton_bt_low_battery = true;
            iton_bt_low_battery_notification();
            break;
        case exit_low_battery_mode:
            iton_bt_exit_low_battery_mode();
            break;
        case low_power_shutdown:
            iton_bt_low_power_shutdown();
            break;
    }
}

static inline void iton_bt_bluetooth_notif(uint8_t param) {
    switch (param) {
        case bt_connection_success:
            iton_bt_in_pairing = false;
            iton_bt_is_connected = true;
            iton_bt_bt_connection_success();
            break;
        case bt_entered_pairing:
            iton_bt_in_pairing = true;
            iton_bt_is_connected = false;
            iton_bt_bt_entered_pairing();
            break;
        case bt_disconnected:
            iton_bt_is_connected = false;
            iton_bt_bt_disconnected();
            break;
        case bt_enters_connection:
            break;
    }
}

OSAL_IRQ_HANDLER(SN32_SPI0_HANDLER) {
    OSAL_IRQ_PROLOGUE();
    if (SN_SPI0->RIS_b.RXFIFOTHIF != 0) {
        if (readPin(ITON_BT_INT_PIN)) {
            uint8_t data = SN_SPI0->DATA;
            if ((data == 0 && iton_bt_rx_ptr != 0) || data != 0) {
                iton_bt_rx[iton_bt_rx_ptr++] = data;
            }
            if (iton_bt_rx_ptr == 2 && iton_bt_rx[0] == led_state) {
                iton_bt_keyboard_led_state = iton_bt_rx[1];
                iton_bt_rx_ptr = 0;
            } else if (iton_bt_rx_ptr == 3 && iton_bt_rx[0] == notification) {
                switch (iton_bt_rx[1]) {
                    case battery_notif:
                        iton_bt_battery_notif(iton_bt_rx[2]);
                        break;
                    case bluetooth_notif:
                        iton_bt_bluetooth_notif(iton_bt_rx[2]);
                        break;
                }
                iton_bt_rx_ptr = 0;
            } else if (iton_bt_rx_ptr >= 3) {
                iton_bt_rx_ptr = 0;
            }
        } else {
            if (iton_bt_tx_ptr < iton_bt_tx_cnt) {
                SN_SPI0->DATA = iton_bt_tx[iton_bt_tx_ptr++];
            } else {
                writePinLow(ITON_BT_IRQ_PIN);
                iton_bt_tx_ptr = 0;
            }
            uint8_t dummyrx = SN_SPI0->DATA;
            (void)dummyrx;
        }
        SN_SPI0->IC_b.RXFIFOTHIC = 1;
    }
    OSAL_IRQ_EPILOGUE();
}
