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
#include <SN32F240B.h>
#include <stdbool.h>
#include "spi.h"
#include "hal.h"
#include "quantum.h"

void spi_init() {
    SN_SYS1->AHBCLKEN_b.SPI0CLKEN   = true;
    SN_SPI0->CTRL0_b.DL             = SPI_DL_8;
    SN_SPI0->CTRL0_b.MS             = SPI_MS_SLAVE_MODE;
    SN_SPI0->CTRL0_b.LOOPBACK       = SPI_LOOPBACK_DIS;
    SN_SPI0->CTRL0_b.SDODIS         = SPI_SDODIS_EN;
    SN_SPI0->CTRL0_b.RXFIFOTH       = SPI_RXFIFOTH_7;
    SN_SPI0->CTRL0_b.TXFIFOTH       = SPI_TXFIFOTH_7;

    SN_SPI0->CLKDIV_b.DIV           = (SPI_DIV / 2) - 1; // 2
    SN_SPI0->CTRL1_b.CPHA           = SPI_CPHA_FALLING_EDGE;
    SN_SPI0->CTRL1_b.CPOL           = SPI_CPOL_SCK_IDLE_LOW;
    SN_SPI0->CTRL1_b.MLSB           = SPI_MLSB_MSB;
    SN_SPI0->CTRL0_b.SELDIS         = SPI_SELDIS_EN;

    SN_SPI0->CTRL0_b.FRESET         = SPI_FRESET_RESET_FIFO; // SPI0_FIFO_RESET

	NVIC_DisableIRQ(SPI0_IRQn);
    writePinLow(SPI_IRQ_PIN);

    SN_SPI0->CTRL0_b.SPIEN = SPI_SPIEN_EN;
}

void spi_send_init(uint8_t _len) {
    writePinHigh(SPI_IRQ_PIN);
}

void spi_send_byte(uint8_t data) {
    while (SN_SPI0->STAT_b.TX_FULL);
    SN_SPI0->DATA = data;
}

void spi_send_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        spi_send_byte(data[i]);
    }
}

void spi_send_end() {
    while (!SN_SPI0->STAT_b.TX_EMPTY);
    writePinLow(SPI_IRQ_PIN);
}

