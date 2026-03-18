// AD9082 SPI Test — Direct SPI preset (AD9082_Linduino style)
#pragma once

#include "config.h"

static inline void cs_lo()  { digitalWrite(CS_PIN, LOW);  }
static inline void cs_hi()  { digitalWrite(CS_PIN, HIGH); }

static void spi_init() {
	pinMode(CS_PIN, OUTPUT);
	cs_hi();
	SPI.begin();
}

static void spi_write_reg(uint16_t reg, uint8_t val) {
	SPI.beginTransaction(SPISettings(SPI_CLK_HZ, MSBFIRST, SPI_MODE0));
	cs_lo();
	SPI.transfer((uint8_t)((reg >> 8) & 0x3F));        // write: MSB clear
	SPI.transfer((uint8_t)(reg & 0xFF));
	SPI.transfer(val);
	cs_hi();
	SPI.endTransaction();
}

static uint8_t spi_read_reg(uint16_t reg) {
	SPI.beginTransaction(SPISettings(SPI_CLK_HZ, MSBFIRST, SPI_MODE0));
	cs_lo();
	SPI.transfer((uint8_t)(((reg >> 8) & 0x3F) | SPI_READ_BIT));  // read: MSB set
	SPI.transfer((uint8_t)(reg & 0xFF));
	uint8_t val = SPI.transfer(0x00);
	cs_hi();
	SPI.endTransaction();
	return val;
}
