// Copyright 2022 1Conan (@1Conan)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string.h>
#include <hal.h>
#include "gpio.h"
#include "config.h"
#include "iton_bt.h"
#include "atomic_util.h"

#ifndef ITON_BT_SPI_PORT
#    define ITON_BT_SPI_PORT SPID1
#endif

#ifndef ITON_BT_IRQ_LINE
#    define ITON_BT_IRQ_LINE A0
#endif

#ifndef ITON_BT_INT_LINE
#    define ITON_BT_INT_LINE A1
#endif

#ifdef KEYBOARD_SHARED_EP
#    define HID_REPORT_OFFSET 1
#else
#    define HID_REPORT_OFFSET 0
#endif

#ifndef ITON_BT_RX_BUFFER_LEN
#    define ITON_BT_RX_BUFFER_LEN 3
#endif

#ifndef ITON_BT_TX_BUFFER_LEN
#    define ITON_BT_TX_BUFFER_LEN 16
#endif

/**
 * Driver Macros
 */
#define HIGH_BITS(x)         ((uint8_t)(x >> 8))
#define LOW_BITS(x)          ((uint8_t)(x & 0x00FF))

/**
 * Function definitions
 */
static void iton_bt_rx_cb(void *arg);
void iton_bt_data_cb(SPIDriver *spip);

/**
 * Callbacks
 */
__attribute__((weak)) void iton_bt_battery_voltage_low(void) {}
__attribute__((weak)) void iton_bt_battery_exit_low_battery_mode(void) {}
__attribute__((weak)) void iton_bt_battery_low_power_shutdown(void) {}

__attribute__((weak)) void iton_bt_connection_successful(void) {}
__attribute__((weak)) void iton_bt_entered_pairing(void) {}
__attribute__((weak)) void iton_bt_disconnected(void) {}
__attribute__((weak)) void iton_bt_enters_connection_state(void) {}

/**
 * Driver variables
 */
bool iton_bt_is_connected = false;
uint8_t iton_bt_led_state = 0x00;

static uint8_t iton_bt_rx[ITON_BT_RX_BUFFER_LEN];
static uint8_t iton_bt_tx[ITON_BT_TX_BUFFER_LEN];
uint8_t iton_bt_send_kb_last_key = 0x00;

#if defined(PAL_USE_CALLBACKS) || defined(PAL_USE_WAIT)
static THD_WORKING_AREA(iton_bt_rx_cb_wa, 128);
thread_t *iton_bt_rx_thd;
#endif

const SPIConfig iton_bt_spicfg = {
    .slave = true,
    .data_cb = iton_bt_data_cb,
    // SN32 specific
    .ctrl0 = SPI_DATA_LENGTH(8),
};

/**
 * Callbacks
 */
#if defined(PAL_USE_CALLBACKS) || defined(PAL_SPID1USE_WAIT)
static void iton_bt_rx_cb(void *arg) {
    if (readPin(ITON_BT_INT_LINE)) {
        chSysLockFromISR();
        spiStartReceiveI(&ITON_BT_SPI_PORT, ITON_BT_RX_BUFFER_LEN, &iton_bt_rx[0]);
        chSysUnlockFromISR();
    } else {
        chSysLockFromISR();
        spiStopTransferI(&ITON_BT_SPI_PORT, NULL);
        chSysUnlockFromISR();

        switch (iton_bt_rx[0]) {
            case led_state:
                iton_bt_led_state = iton_bt_rx[1];
                break;
            case notification:
                chEvtSignalI(iton_bt_rx_thd, (eventmask_t)1);
                break;
        }
    }
}

static THD_FUNCTION(iton_bt_rx_thread, arg) {
    iton_bt_rx_thd = chThdGetSelfX();
    while(true) {
        chEvtWaitAny((eventmask_t)1);

        switch (iton_bt_rx[1]) {
            case notif_battery:
                switch (iton_bt_rx[2]) {
                    case batt_voltage_low:
                        iton_bt_battery_voltage_low();
                        break;
                    case batt_exit_low_battery_mode:
                        iton_bt_battery_exit_low_battery_mode();
                        break;
                    case batt_low_power_shutdown:
                        iton_bt_battery_low_power_shutdown();
                        break;
                    case query_working_mode:
                        break;
                    case query_bt_name:
                        break;
                }
                break;
            case notif_bluetooth:
                switch (iton_bt_rx[2]) {
                    case bt_connection_success:
                        iton_bt_is_connected = true;
                        iton_bt_connection_successful();
                        break;
                    case bt_entered_pairing:
                        iton_bt_entered_pairing();
                        break;
                    case bt_disconected:
                        iton_bt_is_connected = false;
                        iton_bt_disconnected();
                        break;
                    case bt_enters_connection: // connection ready?
                        break;

                }
                break;
        }
    }
}
#endif


void iton_bt_data_cb(SPIDriver *spip) {
    writePinLow(ITON_BT_IRQ_LINE);
}

/**
 * Driver Functions
 */
void iton_bt_init(void) {
    setPinOutput(ITON_BT_IRQ_LINE);
    setPinInput(ITON_BT_INT_LINE);

#if defined(PAL_USE_CALLBACKS) || defined(PAL_USE_WAIT)
    chThdCreateStatic(iton_bt_rx_cb_wa, sizeof(iton_bt_rx_cb_wa), NORMALPRIO, iton_bt_rx_thread, NULL);
    palSetLineCallback(ITON_BT_INT_LINE, iton_bt_rx_cb, NULL);
    palEnableLineEvent(ITON_BT_INT_LINE, PAL_EVENT_MODE_BOTH_EDGES);
#endif

    spiStart(&ITON_BT_SPI_PORT, &iton_bt_spicfg);
}

void iton_bt_send(uint8_t cmd, uint8_t *data, uint8_t len) {
    while (readPin(ITON_BT_IRQ_LINE));

    writePinHigh(ITON_BT_IRQ_LINE);
    iton_bt_tx[0] = cmd;
    memcpy(&iton_bt_tx[1], data, len);
    spiStartSend(&ITON_BT_SPI_PORT, len + 1, &iton_bt_tx[0]);
}

void iton_bt_send2(uint8_t cmd, uint8_t b1, uint8_t b2) {
    while (readPin(ITON_BT_IRQ_LINE));
    writePinHigh(ITON_BT_IRQ_LINE);

    iton_bt_tx[0] = cmd;
    iton_bt_tx[1] = b1;
    iton_bt_tx[2] = b2;

    spiStartSend(&ITON_BT_SPI_PORT, 3, &iton_bt_tx[0]);
}

void iton_bt_send_fn(bool pressed) {
    uint8_t data = pressed ? 0xA3 : 0x00;

    iton_bt_send(report_fn, &data, 1);
}

void iton_bt_send_system(uint16_t data) {
    iton_bt_send(report_system, (uint8_t *)&data, 1);
}

void iton_bt_send_consumer(uint16_t data) {
    iton_bt_send2(report_consumer, HIGH_BITS(data), LOW_BITS(data));
}

void iton_bt_send_keyboard(report_keyboard_t *report) {
    // iton module only reads 5 of the keys in the hid report
    // so we send the last key as an nkro report.
    if (report->keys[5] != iton_bt_send_kb_last_key) {
        uint8_t nkro_report[15] = {0};
        nkro_report[report->keys[5] >> 3] |= (1 << (report->keys[5] & 7));
        iton_bt_send_kb_last_key = report->keys[5];

        return iton_bt_send(report_nkro, &nkro_report[0], 15);
    }

    iton_bt_send(report_hid, &report->raw[HID_REPORT_OFFSET], 8);
}

// pin 0-3 = 26-29
void iton_bt_set_pin(uint8_t pin, bool value) {
    iton_bt_control(control_pins, (1 << pin) | (value << (pin + 4)));
}

#if ITON_BT_TX_BUFFER_LEN >= 35
void iton_bt_set_bt_name(char *name) {
    // Less than 32 octets
    uint8_t length = strlen(name);
    if (length > 31) return;

    uint8_t data[34];

    uint16_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum += name[i];
    }

    data[0] = HIGH_BITS(checksum);
    data[1] = LOW_BITS(checksum);
    memcpy(&data[2], name, length);

    iton_bt_send(set_name, &data[0], length + 2);
}
#endif
