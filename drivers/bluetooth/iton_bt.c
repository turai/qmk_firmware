// Copyright 2021 1Conan (@1Conan)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include "gpio.h"
#include "config.h"
#include "SPI.h"
#include "iton_bt.h"
#include "quantum.h"

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

void iton_bt_init() {
    setPinOutput(ITON_BT_IRQ_PIN);
    writePinLow(ITON_BT_IRQ_PIN);
    setPinInput(ITON_BT_INT_PIN);

    SPI0_Init();

#ifndef ITON_BT_WAIT_FOR_SWITCH
    SPI0_Enable();
#endif
}

void iton_bt_write(uint8_t cmd, uint8_t *data, uint8_t len) {
    writePinHigh(ITON_BT_IRQ_PIN);

    SPI0_Write1(cmd);
    SPI0_Write(data, len);
    SPI0_Write_End();

    writePinLow(ITON_BT_IRQ_PIN);
}

void iton_bt_write2(uint8_t cmd, uint8_t b1, uint8_t b2) {
    uint8_t data[] = {b1, b2};
    iton_bt_write(cmd, &data[0], 2);
}

void iton_bt_control(uint8_t cmd, uint8_t param) {
    iton_bt_write2(control, cmd, param);
}

void iton_bt_mode_usb() {
    iton_bt_control(control_usb, mode_usb);
}

void iton_bt_mode_bt() {
    iton_bt_control(control_bt, mode_bt);
}

void iton_bt_reset_pairing() {
    iton_bt_control(control_bt, reset_pairing);
}

void iton_bt_enter_pairing() {
    iton_bt_control(control_bt, enter_pairing);
}

// profile - 1 to ITON_BT_MAX_PROFILES
void iton_bt_switch_profile(uint8_t profile) {
    if (profile >= ITON_BT_MAX_PROFILES) {
        profile = ITON_BT_MAX_PROFILES - 1;
    }

    iton_bt_control(control_bt, switch_profile + profile);
}

void iton_bt_os_mac() {
    iton_bt_control(control_bt, os_mac);
}

void iton_bt_os_win() {
    iton_bt_control(control_bt, os_win);
}

void iton_bt_set_name(char *name) {
    uint8_t len = sizeof(name);
    uint16_t checksum = 0;

    for (uint8_t i = 0; i < len; i++) {
        checksum += (uint16_t)name[i];
    }

    uint8_t buffer[34]; // 2 + 1 + 31
    memcpy(&buffer[0], &checksum, 2);
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
