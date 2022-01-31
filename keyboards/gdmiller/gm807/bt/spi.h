#pragma once
#include <stdint.h>

void spi_init(void);
void spi_send_init(uint8_t len);
void spi_send_byte(uint8_t data);
void spi_send_bytes(uint8_t *data, uint8_t len);
void spi_send_end(void);

#define	SPI_DL_8                7
#define	SPI_MS_SLAVE_MODE       1
#define	SPI_LOOPBACK_DIS        0
#define	SPI_SDODIS_EN           0
#define	SPI_RXFIFOTH_0          0
#define	SPI_RXFIFOTH_7          7
#define	SPI_TXFIFOTH_0          0
#define	SPI_TXFIFOTH_7          7
#define	SPI_DIV                 6
#define	SPI_CPHA_FALLING_EDGE   0
#define	SPI_CPOL_SCK_IDLE_LOW   0
#define	SPI_MLSB_MSB            0
#define	SPI_SELDIS_EN           0
#define	SPI_RXFIFOTHIE_EN       1
#define SPI_TXFIFOTHIE_DIS      0
#define SPI_TXFIFOTHIE_EN       1
#define	SPI_SPIEN_EN            1
#define	SPI_FRESET_RESET_FIFO	3

#ifndef SPI_IRQ_PIN
#define SPI_IRQ_PIN A0
#endif
#ifndef SPI_INT_PIN
#define SPI_INT_PIN A1
#endif
