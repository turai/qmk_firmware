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
#ifdef BT_SN32F260
#include <SN32F260.h>
#else
#include <SN32F240B.h>
#endif

#include <stdbool.h>
#include "spi.h"
#include "hal.h"
#include "quantum.h"

#define SPI SN_SPI0
#define SYS SN_SYS1

void spi_init() {
    SYS->AHBCLKEN_b.SPI0CLKEN   = true;
    SPI->CTRL0_b.DL             = SPI_DL_8;
    SPI->CTRL0_b.MS             = SPI_MS_SLAVE_MODE;
    SPI->CTRL0_b.LOOPBACK       = SPI_LOOPBACK_DIS;
    SPI->CTRL0_b.SDODIS         = SPI_SDODIS_EN;
    SPI->CTRL0_b.RXFIFOTH       = SPI_RXFIFOTH_7;
    SPI->CTRL0_b.TXFIFOTH       = SPI_TXFIFOTH_7;

    SPI->CLKDIV_b.DIV           = (SPI_DIV / 2) - 1; // 2
    SPI->CTRL1_b.CPHA           = SPI_CPHA_FALLING_EDGE;
    SPI->CTRL1_b.CPOL           = SPI_CPOL_SCK_IDLE_LOW;
    SPI->CTRL1_b.MLSB           = SPI_MLSB_MSB;
    SPI->CTRL0_b.SELDIS         = SPI_SELDIS_EN;

    SPI->CTRL0_b.FRESET         = SPI_FRESET_RESET_FIFO; // SPI0_FIFO_RESET

	NVIC_DisableIRQ(SPI0_IRQn);
    writePinLow(SPI_IRQ_PIN);

#ifdef BT_SN32F260
    SPI->CTRL0_b.SSPEN = SPI_SPIEN_EN;
#else
    SPI->CTRL0_b.SPIEN = SPI_SPIEN_EN;
#endif
}

void spi_send_init(uint8_t _len) {
    writePinHigh(SPI_IRQ_PIN);
}

void spi_send_byte(uint8_t data) {
    while (SPI->STAT_b.TX_FULL);
    SPI->DATA = data;
}

void spi_send_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        spi_send_byte(data[i]);
    }
}

void spi_send_end() {
    while (!SPI->STAT_b.TX_EMPTY);
    writePinLow(SPI_IRQ_PIN);
}
