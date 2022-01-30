#include "ch.h"
#include "hal.h"
#include "gm807.h"
#include "config_led.h"
#include "bt/spi.h"
#include "bt/iton.h"
#include "bt/driver.h"

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

    // Bluetooth
    setPinOutput(SPI_IRQ_PIN);
    setPinInput(SPI_INT_PIN);

    spi_init();

    keyboard_pre_init_user();
}

bool __attribute__((noinline)) process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch(keycode) {
            case KEY_USB_LED:
                writePin(LED_USB_PORT, !readPin(LED_USB_PORT));
                return false;
            case KEY_BT_TOGGLE:
                iton_switch_driver();
                break;
            case KEY_BT_MODE:
                if (!iton_is_bt()) iton_switch_driver();
                break;
            case KEY_USB_MODE:
                if (iton_is_bt()) iton_switch_driver();
                break;
            case KEY_BT_PAIR:
                iton_pairing_mode();
                break;
            case KEY_BT_PROF1:
                iton_profile(0);
                break;
            case KEY_BT_PROF2:
                iton_profile(1);
                break;
            case KEY_BT_PROF3:
                iton_profile(2);
                break;
            default:
                break;
        }
    }
    return process_record_user(keycode, record);
}
