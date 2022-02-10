// Copyright 2021 1Conan (@1Conan)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "quantum.h"

void iton_bt_init(void);

// control
void iton_bt_mode_usb(void);
void iton_bt_mode_bt(void);
void iton_bt_reset_pairing(void);
void iton_bt_enter_pairing(void);
void iton_bt_switch_profile(uint8_t profile);
void iton_bt_os_mac(void);
void iton_bt_os_win(void);
void iton_bt_set_name(char *name);

void iton_bt_send_keyboard(report_keyboard_t *report);
void iton_bt_send_consumer(uint16_t data);
void iton_bt_send_system(uint16_t data);
