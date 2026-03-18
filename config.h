// AD9082 SPI Test — Shared configuration
#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "Linduino.h"

// =======================================================================
// Preset selection  (uncomment ONE)
// =======================================================================
#define PRESET_LINDUINO   // Direct SPI calls (simple, AD9082_Linduino style)
//#define PRESET_API      // ADI API HAL style (device struct + callbacks)

// =======================================================================
// Hardware config
// =======================================================================
#define CS_PIN          QUIKEVAL_CS   // Arduino pin 10 / SS
#define SPI_CLK_HZ      1000000UL     // 1 MHz — safe for bring-up
#define SPI_READ_BIT     0x80          // MSB of first address byte = read

// =======================================================================
// Test registers (all accessible without converter clock)
// =======================================================================
#define REG_SPI_CONF_A   0x0000  // SPI interface config (R/W)
#define REG_CHIP_ID_L    0x0003  // Chip ID low  (read-only)
#define REG_CHIP_ID_H    0x0004  // Chip ID high (read-only)
#define REG_SCRATCH       0x001C  // Scratch pad  (R/W)
#define REG_CHIP_GRADE    0x0006  // Grade + revision (read-only)

// =======================================================================
// Hex printing helpers
// =======================================================================
static void hex8(uint8_t v) {
	if (v < 0x10) Serial.print('0');
	Serial.print(v, HEX);
}
static void hex16(uint16_t v) { hex8(v >> 8); hex8(v & 0xFF); }
