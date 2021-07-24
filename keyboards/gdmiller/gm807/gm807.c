#include "ch.h"
#include "hal.h"
#include "gm807.h"

void __attribute__((noinline)) keyboard_pre_init_kb(void)  {
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

bool __attribute__((noinline)) process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch(keycode) {
            case KEY_USB_LED:
                writePin(LED_USB_PORT, !readPin(LED_USB_PORT));
                return false;
            default:
                break;
        }
    }
    return process_record_user(keycode, record);
}
