/* Copyright 2021 Harrison Chan (Xelus)
 * Copyright 2022 1Conan <me@1conan.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "gm807.h"
#ifdef BLUETOOTH_ENABLE
#include "outputselect.h"
#endif

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _FN,
};

enum keycodes {
    BT_TOG = GM807_SAFE_RANGE,
    RGB_BRI_CTRL_TOG,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*      +--------------------------------------------------------------------------+----------------+
        | ESC |  | F1 | F2 | F3 | F4 | | F5 | F6 | F7 | F8 | | F9| F10| F11| F12|  | |PSCR|????|PAUS|
        +--------------------------------------------------------------------------+------|----|----|
        |  ~  |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = | BACKSP | |INS |SCRL|PGUP|
        +--------------------------------------------------------------------------+------|----|----|
        |  TAB  |  Q |  W |  E |  R |  T |  Y |  U |  I |  O |  P |  [ |  ] |   \  | |DEL |END |PGDN|
        +--------------------------------------------------------------------------+------|----|----|
        | CAPSLCK  |  A |  S |  D |  F |  G |  H |  J |  K |  L | ; | ' |  RETURN  |  K5  | K4 |    |
        +--------------------------------------------------------------------------+------|----|    |
        | LSHIFT     |  Z |  X |  C |  V |  B |  N |  M | , | . |  / |   RSHIFT    |      | UP |    |
        +--------------------------------------------------------------------------+------|----|----|
        |LCTRL| LGUI| LALT |            SPACE            | RALT| RGUI | FN | RCTRL | |LFT |DWN |RGT |
        +--------------------------------------------------------------------------+----------------+
*/

    [_BASE] = LAYOUT(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,      KC_PSCR, KC_SLCK, KC_PAUS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,     KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,     KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,      GUI_TOG, RGB_BRI_CTRL_TOG,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,                       KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                                      KC_RALT, MO(_FN), KC_APP,  KC_RCTL,     KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_FN] = LAYOUT(
        RESET,            _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______, _______, KEY_USB_LED,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     RGB_SPI, RGB_VAI, RGB_HUI,
        BT_TOG,  BT_PRO1, BT_PRO2, BT_PRO3, _______, _______, _______, _______, _______, _______, BT_PAIR, _______, _______, _______,     RGB_SPD, RGB_VAD, RGB_HUD,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,     _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,                       RGB_VAI,
        _______, _______, _______,                   _______,                                     _______, _______, _______, _______,     RGB_RMOD,RGB_VAD, RGB_MOD
    )
};

bool control_brightness = false;

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        if (control_brightness) {
            rgb_matrix_increase_val();
        } else {
            tap_code(KC_VOLU);
        }
    } else {
        if (control_brightness) {
            rgb_matrix_decrease_val();
        } else {
            tap_code(KC_VOLD);
        }
    }

    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case BT_TOG:
#ifdef BLUETOOTH_ENABLE
            if (record->event.pressed) {
                set_output(where_to_send() == OUTPUT_USB ? OUTPUT_BLUETOOTH : OUTPUT_USB);
            }
#endif
            return false;
        case RGB_BRI_CTRL_TOG:
            if (record->event.pressed) {
                control_brightness = !control_brightness;
                writePin(LED_K4, !control_brightness);
            }
            return false;
        case MAGIC_TOGGLE_GUI:
            writePin(LED_K5, keymap_config.no_gui);
            return false;
        default:
            return true;
    }
}

void keyboard_post_init_user() {
    writePinHigh(LED_USB_PORT);
    writePinHigh(LED_K4);
}
