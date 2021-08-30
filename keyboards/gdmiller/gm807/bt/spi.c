#include <SN32F240B.h>
#include <stdbool.h>
#include "driver.h"
#include "host.h"
#include "gpio.h"
#include "spi.h"
#include "hal.h"

#include "print.h"

// Longest we send is 0xA2 NKRO which is 17 bytes
uint8_t SPI_TX_BUFFER[17] = {};
uint8_t SPI_TX_PTR = 0;
uint8_t SPI_TX_SENT = 0;
uint8_t SPI_TX_LEN = 0;

uint8_t SPI_RX_BUFFER[17] = {};
uint8_t SPI_RX_PTR = 0;
uint8_t SPI_RX_LEN = 0;

void spi_init() {
    SN_SYS1->AHBCLKEN_b.SPI0CLKEN   = true;

    SN_SPI0->CTRL0_b.DL             = SPI_DL_8;
    SN_SPI0->CTRL0_b.MS             = SPI_MS_SLAVE_MODE;
    SN_SPI0->CTRL0_b.LOOPBACK       = SPI_LOOPBACK_DIS;
    SN_SPI0->CTRL0_b.SDODIS         = SPI_SDODIS_EN;
    SN_SPI0->CTRL0_b.RXFIFOTH       = SPI_RXFIFOTH_0;
    SN_SPI0->CTRL0_b.TXFIFOTH       = SPI_TXFIFOTH_0;

    SN_SPI0->CLKDIV_b.DIV           = (SPI_DIV / 2) - 1; // 2

    SN_SPI0->CTRL1_b.CPHA           = SPI_CPHA_FALLING_EDGE;
    SN_SPI0->CTRL1_b.CPOL           = SPI_CPOL_SCK_IDLE_LOW;
    SN_SPI0->CTRL1_b.MLSB           = SPI_MLSB_MSB;

    SN_SPI0->CTRL0_b.SELDIS         = SPI_SELDIS_EN;
    SN_SPI0->IC_b.RXOVFIC           = true;
    SN_SPI0->IC_b.RXTOIC            = true;
    SN_SPI0->IC_b.RXFIFOTHIC        = true;
    SN_SPI0->IC_b.TXFIFOTHIC        = true;

    SN_SPI0->IC_b.RXOVFIC           = true;
    SN_SPI0->IC_b.RXTOIC            = true;
    SN_SPI0->IC_b.RXFIFOTHIC        = true;
    SN_SPI0->IC_b.TXFIFOTHIC        = true;

    // SN_SPI0->IE_b.TXFIFOTHIE        = SPI_TXFIFOTHIE_EN;
    SN_SPI0->IE_b.RXFIFOTHIE        = SPI_RXFIFOTHIE_EN;

    SPI0_FIFO_RESET();

    #define SPI0_IRQ 6
	NVIC_ClearPendingIRQ(SPI0_IRQ);
	NVIC_EnableIRQ(SPI0_IRQ);

    // NVIC_SetPriority(SPI0_IRQ, 0);

    SN_SPI0->CTRL0_b.SPIEN = SPI_SPIEN_EN;
}

void spi_send(uint8_t *bytes, uint8_t size) {
    SPI_TX_LEN = size;
    for (uint8_t i = 0; i < size; i++) {
        SPI_TX_BUFFER[i] = bytes[i];
    }
    writePinHigh(SPI_IRQ_PIN);
    while (SN_SPI0->STAT_b.BUSY);
    SN_SPI0->CTRL0_b.FRESET = 0b11;
    while (SN_SPI0->CTRL0_b.FRESET == 0b11);
    SN_SPI0->DATA = (uint32_t)SPI_TX_BUFFER[0];
    SPI_TX_SENT = 1;
}

void spi_send_init(uint8_t length) {
    writePinHigh(SPI_IRQ_PIN);
    SPI_TX_LEN = length;
    SPI_TX_PTR = 0;
}

void spi_send_byte(uint8_t byte) {
    SPI_TX_BUFFER[SPI_TX_PTR++] = byte;
}

void spi_send_bytes(uint8_t *bytes, uint8_t size) {
    for (uint8_t i = 0; i < size; i++) {
        spi_send_byte(bytes[i]);
    }
}

void spi_send_end() {
    while (SN_SPI0->STAT_b.BUSY);
    SN_SPI0->CTRL0_b.FRESET = 0b11;
    while (SN_SPI0->CTRL0_b.FRESET == 0b11);
    SN_SPI0->DATA = (uint32_t)SPI_TX_BUFFER[SPI_TX_SENT];
    SPI_TX_SENT = 1;
}

CH_IRQ_HANDLER(SN32_SPI0_HANDLER) {
    uint8_t count;
    CH_IRQ_PROLOGUE();
    if (!readPin(A1)) {
        if (SPI_TX_LEN != 0) {
            if (SPI_TX_SENT < SPI_TX_LEN) {
                count = SPI_TX_SENT;
                SPI_TX_SENT++;
                SN_SPI0->DATA = (uint32_t) SPI_TX_BUFFER[count];
            } else {
                writePinLow(SPI_IRQ_PIN);
                SPI_TX_SENT = 0;
                SPI_TX_LEN = 0;
            }
            SN_SPI0->IC_b.TXFIFOTHIC = true;
        }
    } else if (!SN_SPI0->STAT_b.RX_EMPTY) {
        uint8_t data = SN_SPI0->DATA;
        SPI_RX_BUFFER[SPI_RX_PTR++] = data;
        if (SPI_RX_PTR > sizeof(SPI_RX_BUFFER)) {
            SPI_RX_PTR = 0;
        }
        SN_SPI0->IC_b.RXFIFOTHIC = true;
        SN_SPI0->DATA = 0x0;
    }
    CH_IRQ_EPILOGUE();
}
