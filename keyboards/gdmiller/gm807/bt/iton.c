/* Copyright 2022 1Conan
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
#include "spi.h"
#include "iton.h"


#ifdef ITON_BT5
#define ITON_HID_REPORT 0x51
#define ITON_NKRO_REPORT 0x52
#define ITON_CONSUMER_REPORT 0x53
#define ITON_SYSTEM_REPORT 0x54
#define ITON_FN_REPORT 0x55
#define ITON_CONTROL 0x56
#define ITON_MODIFY_NAME 0x57
#else
#define ITON_HID_REPORT 0xA1
#define ITON_NKRO_REPORT 0xA2
#define ITON_CONSUMER_REPORT 0xA3
#define ITON_SYSTEM_REPORT 0xA4
#define ITON_FN_REPORT 0xA5
#define ITON_CONTROL 0xA6
#define ITON_MODIFY_NAME 0xA7
#endif

void iton_hid_report(uint8_t *data) {
    spi_send_init(9); // 1 byte + 8 bytes
    spi_send_byte(ITON_HID_REPORT);
    spi_send_bytes(data, 8);
    spi_send_end();
}

void iton_nkro_report(uint8_t *data) {
    spi_send_init(17); // 1 byte + 16 bytes
    spi_send_byte(ITON_NKRO_REPORT);
    spi_send_bytes(data, 16);
    spi_send_end();
}

void iton_consumer_report(uint16_t data) {
    spi_send_init(3); // 1 byte + 2 bytes
    spi_send_byte(ITON_CONSUMER_REPORT);
    spi_send_byte((uint8_t)(data >> 8));
    spi_send_byte((uint8_t)data);
    spi_send_end();
}

// 0xA8 - Shutdown
// 0xA9 - Sleep
// 0xAA - Wake
void iton_system_report(uint8_t data) {
    spi_send_init(2); // 1 byte + 1 byte
    spi_send_byte(ITON_SYSTEM_REPORT);
    spi_send_byte(data);
    spi_send_end();
}

// 0xA3 - Fn keypress
// 0x00 - Fn key release
void iton_fn_report(uint8_t data) {
    spi_send_init(2); // 1 byte + 1 byte
    spi_send_byte(ITON_FN_REPORT);
    spi_send_byte(data);
    spi_send_end();
}

void iton_control(uint8_t cmd, uint8_t data) {
    spi_send_init(3);
    spi_send_byte(ITON_CONTROL);
    spi_send_byte(cmd);
    spi_send_byte(data);
    spi_send_end();
}

void iton_mode_usb() {
    iton_control(0x58, 0x01);
}

void iton_mode_bt() {
    iton_control(0x51, 0x62);
}

void iton_clear_pairing() {
    iton_control(0x51, 0x70);
}

void iton_pairing_mode() {
    iton_control(0x51, 0x89);
}

void iton_profile(uint8_t profile) {
    if (profile > BT_MAX_PROFILES) {
        profile = BT_MAX_PROFILES;
    } else if (profile < 0) {
        profile = 0;
    }

    iton_control(0x51, 0x81 + profile);
}

void iton_os(iton_os_type_t type) {
    iton_control(0x51, type);
}

// TODO: Implement
// void iton_change_name() {
// }
