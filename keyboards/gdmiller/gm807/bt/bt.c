#include "quantum.h"
#include "driver.h"
#include "spi.h"
#include "bt.h"

static host_driver_t *last_host_driver = NULL;

static void iton_control(uint8_t cmd, uint8_t param) {
    spi_send_init(3);
    spi_send_byte(0xA6);
    spi_send_byte(cmd);
    spi_send_byte(param);
    spi_send_end();
}

void iton_clear_pairing() {
    iton_control(0x51, 0x70);
}

void iton_enter_pairing() {
    iton_control(0x51, 0x89);
}

void iton_switch_profile(uint8_t profile) {
    if (profile > 5) {
        profile = 5;
    } else if (profile < 0) {
        profile = 0;
    }
    iton_control(0x51, 0x81 + profile);
}

void iton_macos_mode() {
    iton_control(0x51, 0x74);
}

void iton_windows_mode() {
    iton_control(0x51, 0x75);
}

void iton_switch_bt() {
    last_host_driver = host_get_driver();
    iton_control(0x51, 0x62);
    clear_keyboard();
    host_set_driver(&iton_bt_driver);
}
void iton_switch_usb() {
    iton_control(0x58, 0x01);
    clear_keyboard();
    host_set_driver(last_host_driver);
    last_host_driver = NULL;
}

void iton_toggle_bt() {
    if (host_get_driver() == &iton_bt_driver) {
        iton_switch_usb();
        return;
    }
    iton_switch_bt();
}

void iton_change_name(char *name, uint8_t len) {
    if (len > 31) {
        return;
    }
    uint16_t checksum = 0;
    for (uint8_t i = 0; i < len; i++) {
        checksum += name[i];
    }
    spi_send_init(4 + len);
    spi_send_byte(0xA7);
    spi_send_byte((uint8_t)(checksum >> 8));
    spi_send_byte((uint8_t)(checksum & 0x00FF));
    spi_send_bytes((uint8_t *)name, len);
    spi_send_end();
}
