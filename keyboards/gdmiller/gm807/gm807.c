#include "ch.h"
#include "hal.h"
#include "gm807.h"
#include "bt/spi.h"
#include "bt/bt.h"

void __attribute__((noinline)) keyboard_pre_init_kb(void)  {
    // IRQ
    setPinOutput(A0);
    writePinLow(A0);

    // turn off LEDs on two buttons near encoder
    setPinOutput(LED_K4);
    writePinLow(LED_K4);
    setPinOutput(LED_K5);
    writePinLow(LED_K5);

    // Turn off LED on the USB PORT of the USB hub
    setPinOutput(LED_USB_PORT);
    writePinLow(LED_USB_PORT);

    // Turn off output for encoder LEDs
    setPinOutput(LED_ENCODER);
    writePinLow(LED_ENCODER);

    keyboard_pre_init_user();
}

void __attribute__((noinline)) keyboard_post_init_kb(void)  {
    spi_init();
    keyboard_post_init_user();
}

bool __attribute__((noinline)) process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch(keycode) {
            case KEY_USB_LED:
                writePin(LED_USB_PORT, !readPin(LED_USB_PORT));
                return false;
            case KEY_TOGGLE_BT:
                iton_toggle_bt();
                return false;
            case KEY_BT_MODE:
                iton_switch_bt();
                return false;
            case KEY_USB_MODE:
                iton_switch_usb();
                return false;
            case KEY_BT_PAIR:
                iton_enter_pairing();
                return false;
            case KEY_BT_PROF1:
                iton_switch_profile(0);
                return false;
            case KEY_BT_PROF2:
                iton_switch_profile(1);
                return false;
            case KEY_BT_PROF3:
                iton_switch_profile(2);
                return false;
            case KEY_BT_PROF4:
                iton_switch_profile(3);
                return false;
            case KEY_BT_PROF5:
                iton_switch_profile(4);
                return false;
            case KEY_BT_PROF6:
                iton_switch_profile(5);
                return false;
            case KEY_BT_MACOS_MODE:
                iton_macos_mode();
                return false;
            case KEY_BT_WINDOWS_MODE:
                iton_windows_mode();
                return false;
            default:
                break;
        }
    }
    return process_record_user(keycode, record);
}
