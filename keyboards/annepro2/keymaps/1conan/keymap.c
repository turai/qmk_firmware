#include <stdint.h>
#include "annepro2.h"
#include "qmk_ap2_led.h"
#include "config.h"

enum anne_pro_layers {
    _BASE,
    _FN1,
    _FN2,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = KEYMAP(
        KC_ESC,   KC_1,     KC_2,    KC_3, KC_4, KC_5, KC_6,   KC_7, KC_8,    KC_9,   KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,   KC_Q,     KC_W,    KC_E, KC_R, KC_T, KC_Y,   KC_U, KC_I,    KC_O,   KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        MO(_FN1), KC_A,     KC_S,    KC_D, KC_F, KC_G, KC_H,   KC_J, KC_K,    KC_L,            KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT,  KC_Z,     KC_X,    KC_C, KC_V, KC_B, KC_N,   KC_M, KC_COMM, KC_DOT,          KC_SLSH,          KC_UP,
        KC_LCTL,  MO(_FN2), KC_LALT,                   KC_SPC,                        KC_RALT, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_FN1] = KEYMAP(
        KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        _______,  _______,  KC_UP,   _______, _______, _______, _______, _______, KC_UP,   _______, KC_PSCR, KC_HOME, KC_END,  _______,
        MO(_FN1), KC_LEFT,  KC_DOWN, KC_RGHT, _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGUP, KC_PGDN,          _______,
        _______,  _______,  _______, _______, _______, _______, _______, _______, _______, KC_INS,  KC_DEL,                    _______,
        _______,  MO(_FN2), _______,                            _______,                            _______, _______, _______, _______
    ),
    [_FN2] = KEYMAP(
        KC_GRV,   KC_AP2_BT1, KC_AP2_BT2, KC_AP2_BT3, KC_AP2_BT4, KC_AP2_USB, _______, _______, KC_AP_LED_OFF, KC_AP_LED_ON, KC_AP_LED_NEXT_INTENSITY, KC_AP_LED_SPEED, _______, _______,
        _______,  _______,    KC_UP,      _______,    _______,    _______,    _______, _______, KC_UP,         _______,      KC_PSCR,                  KC_HOME,         KC_END,  _______,
        MO(_FN1), KC_LEFT,    KC_DOWN,    KC_RGHT,    _______,    _______,    _______, KC_LEFT, KC_DOWN,       KC_RGHT,      KC_PGUP,                  KC_PGDN,                  _______,
        _______,  KC_VOLD,    KC_VOLU,    KC_MUTE,    _______,    _______,    _______, _______, _______,       KC_INS,       KC_DEL,                                             _______,
        KC_MPRV,  MO(_FN2),   KC_MNXT,                                        KC_MPLY,                                       _______,                  _______,         _______, _______
    ),
};

const uint16_t keymaps_size = sizeof(keymaps);

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
}

layer_state_t layer_state_set_user(layer_state_t layer) {
    return layer;
}
