// AD9082 SPI Test — Minimal register read/write/monitor
//
// Two SPI preset modes selectable at compile time:
//
//   PRESET_LINDUINO  — Direct SPI using Arduino SPI.h (same approach as
//                      AD9082_Linduino project). Simple, minimal overhead.
//
//   PRESET_API       — ADI API-style HAL with device struct, function-pointer
//                      callbacks, and spi_xfer() matching the adi_ad9082_hal
//                      calling convention from the reference source.
//
// Both presets produce identical SPI traffic on the wire; the difference is
// in code structure and how the bus access is abstracted.
//
// File layout:
//   config.h        — Preset selection, hardware config, register addresses,
//                     hex helpers
//   spi_linduino.h  — PRESET_LINDUINO SPI implementation
//   spi_api.h       — PRESET_API SPI implementation (ADI HAL style)
//   tests.h         — Test routines (test_read_id, test_scratch,
//                     monitor_registers)

#include <Arduino.h>
#include <SPI.h>
#include "Linduino.h"   // Local copy — no external library needed

#include "config.h"

#ifdef PRESET_LINDUINO
#include "spi_linduino.h"
#endif
#ifdef PRESET_API
#include "spi_api.h"
#endif

#include "tests.h"

// =======================================================================
// Arduino entry points
// =======================================================================

void setup() {
	Serial.begin(115200);
	while (!Serial) { ; }

	Serial.println();
	Serial.println(F("=== AD9082 SPI Test ==="));
#ifdef PRESET_LINDUINO
	Serial.println(F("Preset: LINDUINO (direct SPI)"));
#endif
#ifdef PRESET_API
	Serial.println(F("Preset: API (ADI HAL callback style)"));
#endif
	Serial.print(F("CS pin : ")); Serial.println(CS_PIN);
	Serial.print(F("SPI Hz : ")); Serial.println(SPI_CLK_HZ);

	// Enable SPI mode on QuikEval connector
	pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);
	digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);

	spi_init();

	// --- Run tests ---
	bool id_ok = test_read_id();
	if (!id_ok) {
		Serial.println(F("\nSPI bus not responding. Halting."));
		while (true) { delay(1000); }
	}

	test_scratch();

	Serial.println(F("\nSetup complete. Monitoring every 3 seconds.\n"));
}

void loop() {
	static uint32_t last_ms = 0;
	if (millis() - last_ms >= 3000) {
		last_ms = millis();
		monitor_registers();
	}
}
