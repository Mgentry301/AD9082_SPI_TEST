// AD9082 SPI Test — Test and monitor routines
// Preset-agnostic: both presets expose spi_read_reg() / spi_write_reg().
#pragma once

#include "config.h"

// Test patterns for scratch register
static const uint8_t SCRATCH_PATTERNS[] = { 0x5A, 0xA5, 0x0F, 0xF0, 0x00 };
#define NUM_PATTERNS (sizeof(SCRATCH_PATTERNS) / sizeof(SCRATCH_PATTERNS[0]))

// Read chip ID and grade registers
static bool test_read_id() {
	Serial.println(F("\n--- Read ID registers ---"));

	uint8_t id_l  = spi_read_reg(REG_CHIP_ID_L);
	uint8_t id_h  = spi_read_reg(REG_CHIP_ID_H);
	uint8_t grade = spi_read_reg(REG_CHIP_GRADE);
	uint8_t spi_a = spi_read_reg(REG_SPI_CONF_A);

	uint16_t chip_id = ((uint16_t)id_h << 8) | id_l;
	Serial.print(F("  Chip ID     : 0x")); hex16(chip_id); Serial.println();
	Serial.print(F("  Chip grade  : 0x")); hex8(grade);    Serial.println();
	Serial.print(F("  SPI conf A  : 0x")); hex8(spi_a);    Serial.println();

	if (chip_id == 0xFFFF) {
		Serial.println(F("  ** Bus reads all 0xFF — no device responding **"));
		return false;
	}
	if (chip_id == 0x0000 && grade == 0x00) {
		Serial.println(F("  ** Bus reads all 0x00 — check CS, MOSI, SCLK **"));
		return false;
	}
	Serial.println(F("  ID read OK."));
	return true;
}

// Write/readback scratch register with multiple patterns
static bool test_scratch() {
	Serial.println(F("\n--- Scratch register (0x001C) write/read test ---"));
	uint8_t failures = 0;

	for (uint8_t i = 0; i < NUM_PATTERNS; i++) {
		uint8_t pat = SCRATCH_PATTERNS[i];
		spi_write_reg(REG_SCRATCH, pat);
		uint8_t rb = spi_read_reg(REG_SCRATCH);

		Serial.print(F("  Write 0x")); hex8(pat);
		Serial.print(F(" -> Read 0x")); hex8(rb);
		if (rb == pat) {
			Serial.println(F("  OK"));
		} else {
			Serial.println(F("  FAIL"));
			failures++;
		}
	}

	if (failures == 0) {
		Serial.println(F("  All scratch patterns passed."));
	} else {
		Serial.print(F("  ")); Serial.print(failures);
		Serial.println(F(" pattern(s) FAILED."));
	}
	return (failures == 0);
}

// One-shot monitor: read 5 registers and report
static void monitor_registers() {
	static const uint16_t mon_regs[] = {
		REG_SPI_CONF_A, REG_CHIP_ID_L, REG_CHIP_ID_H, REG_SCRATCH, REG_CHIP_GRADE
	};
	static const char* const mon_names[] = {
		"SPI_CONF_A", "CHIP_ID_L ", "CHIP_ID_H ", "SCRATCH   ", "CHIP_GRADE"
	};
	#define NUM_MON (sizeof(mon_regs) / sizeof(mon_regs[0]))

	Serial.println(F("--- Register snapshot ---"));
	for (uint8_t i = 0; i < NUM_MON; i++) {
		uint8_t val = spi_read_reg(mon_regs[i]);
		Serial.print(F("  0x"));
		hex16(mon_regs[i]);
		Serial.print(F(" ("));
		Serial.print(mon_names[i]);
		Serial.print(F(") = 0x"));
		hex8(val);
		Serial.println();
	}
}
