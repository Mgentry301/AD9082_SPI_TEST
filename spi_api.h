// AD9082 SPI Test — ADI API HAL style preset (device struct + callbacks)
#pragma once

#include "config.h"

// Minimal subset of ADI types needed for the HAL pattern.
// Mirrors adi_ad9082_hal_t / adi_ad9082_device_t from the reference API.
typedef int32_t(*spi_xfer_fn)(void *user_data, uint8_t *in_data,
                              uint8_t *out_data, uint32_t size_bytes);
typedef int32_t(*delay_us_fn)(void *user_data, uint32_t us);

typedef struct {
	void        *user_data;
	spi_xfer_fn  spi_xfer;
	delay_us_fn  delay_us;
} api_hal_t;

typedef struct {
	api_hal_t hal_info;
} api_device_t;

// ---------- Platform callbacks (Arduino implementation) ----------

static int32_t platform_spi_xfer(void *user_data, uint8_t *in_data,
                                  uint8_t *out_data, uint32_t size_bytes) {
	(void)user_data;
	uint32_t len = size_bytes & 0xFFFF;  // lower 16 bits = byte count
	SPI.beginTransaction(SPISettings(SPI_CLK_HZ, MSBFIRST, SPI_MODE0));
	digitalWrite(CS_PIN, LOW);
	for (uint32_t i = 0; i < len; i++) {
		out_data[i] = SPI.transfer(in_data[i]);
	}
	digitalWrite(CS_PIN, HIGH);
	SPI.endTransaction();
	return 0;  // API_CMS_ERROR_OK
}

static int32_t platform_delay_us(void *user_data, uint32_t us) {
	(void)user_data;
	delayMicroseconds(us);
	return 0;
}

// ---------- Device instance ----------
static api_device_t dev;

static void spi_init() {
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);
	SPI.begin();

	dev.hal_info.user_data = NULL;
	dev.hal_info.spi_xfer  = platform_spi_xfer;
	dev.hal_info.delay_us  = platform_delay_us;
}

// Register access via the HAL spi_xfer callback — matches the
// adi_ad9082_hal_reg_get / adi_ad9082_hal_reg_set pattern in the
// reference API (ad9082_api/ad9082/src/adi_ad9082_hal.c).
static void spi_write_reg(uint16_t reg, uint8_t val) {
	uint8_t in_data[3] = {0}, out_data[3] = {0};
	in_data[0] = (reg >> 8) & 0x3F;        // write: bit 7 clear
	in_data[1] = (reg >> 0) & 0xFF;
	in_data[2] = val;
	dev.hal_info.spi_xfer(dev.hal_info.user_data, in_data, out_data, 0x3);
}

static uint8_t spi_read_reg(uint16_t reg) {
	uint8_t in_data[3] = {0}, out_data[3] = {0};
	in_data[0] = ((reg >> 8) & 0x3F) | 0x80;  // read: bit 7 set
	in_data[1] = (reg >> 0) & 0xFF;
	dev.hal_info.spi_xfer(dev.hal_info.user_data, in_data, out_data, 0x3);
	return out_data[2];
}
