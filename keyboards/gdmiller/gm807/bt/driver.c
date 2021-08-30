#include "quantum.h"
#include "spi.h"
static uint8_t keyboard_leds(void);
static void    send_keyboard(report_keyboard_t *report);
static void    send_mouse(report_mouse_t *report);
static void    send_system(uint16_t data);
static void    send_consumer(uint16_t data);

host_driver_t iton_bt_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer,
};

static uint8_t keyboard_leds(void) {
    return 0; // TODO: handle leds
}

static void send_mouse(report_mouse_t *report) {
    // Not Supported
}

static void send_system(uint16_t data) {
    spi_send_init(2);
    spi_send_byte(0xA4);
    spi_send_byte(data + 0x03);
    spi_send_end();
}

static void send_consumer(uint16_t data) {
    spi_send_init(3);
    spi_send_byte(0xA3);
    spi_send_byte((uint8_t)(data >> 8));
    spi_send_byte((uint8_t)(data & 0x00FF));
    spi_send_end();
}


static void send_keyboard(report_keyboard_t *report) {
#ifdef NKRO_ENABLE
    if (!keymap_config.nkro) {
#endif
        spi_send_init(9);
        spi_send_byte(0xA1);
        spi_send_bytes(&report->raw[0], 8);
        spi_send_end();
#ifdef NKRO_ENABLE
        return;
    }

    static uint8_t last_mod_sent = 0;
    if (last_mod_sent != report->nkro.mods) {
        static uint8_t zeroes[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        spi_send_init(9);
        spi_send_byte(0xA1);
        spi_send_byte(report->nkro.mods);
        spi_send_bytes(zeroes, 7);
        spi_send_end();
        last_mod_sent = report->nkro.mods;
    }

    spi_send_init(17);
    spi_send_byte(0xA2);
    spi_send_bytes(&report->nkro.bits[0], 16);
    spi_send_end();
#endif
}
