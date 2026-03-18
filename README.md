# AD9082 SPI Test

Standalone Arduino sketch for verifying SPI communication with the
[AD9082](https://www.analog.com/AD9082) MxFE (mixed-signal front-end) using a
Linduino (Arduino Mega).

Use this sketch **before** running a full device initialization to confirm that
the SPI bus is wired correctly and the AD9082 is responding.  No converter
clock is required — the registers tested here are accessible as soon as the
digital supplies are up.

## What it does

1. **Chip ID read** — reads `CHIP_ID_L` (0x0003), `CHIP_ID_H` (0x0004),
   `CHIP_GRADE` (0x0006), and `SPI_CONF_A` (0x0000).  Reports an error if
   the bus returns all-0xFF (no device) or all-0x00 (stuck bus).
2. **Scratch register write/readback** — writes five patterns (`0x5A`, `0xA5`,
   `0x0F`, `0xF0`, `0x00`) to the scratch register (0x001C) and reads each
   back, reporting pass/fail per pattern.
3. **Periodic monitor** — after the one-time tests, reads the same five
   registers every 3 seconds and prints a snapshot to the serial console.

## Hardware

| Item | Detail |
|------|--------|
| Controller | Linduino (Arduino Mega) |
| CS pin | `QUIKEVAL_CS` (pin 10 / SS) |
| SPI clock | 1 MHz, Mode 0, MSB-first |
| MUX mode pin | `QUIKEVAL_MUX_MODE_PIN` (pin 8, driven LOW for SPI) |
| Serial | 115200 baud |

## SPI presets

Two compile-time SPI backends are provided.  Both produce **identical traffic
on the wire** — the difference is in code structure.

| Preset | `#define` | Description |
|--------|-----------|-------------|
| **Linduino** | `PRESET_LINDUINO` | Direct `SPI.transfer()` calls. Simple and minimal, same approach as the AD9082_Linduino project. |
| **API** | `PRESET_API` | ADI HAL callback style with a device struct and `spi_xfer()` function pointer, mirroring the `adi_ad9082_hal` layer from the ADI reference API. |

To switch presets, edit the `#define` in `config.h`:

```c
#define PRESET_LINDUINO   // Direct SPI calls (simple, AD9082_Linduino style)
//#define PRESET_API      // ADI API HAL style (device struct + callbacks)
```

## File layout

```
AD9082_SPI_TEST/
├── AD9082_SPI_TEST.ino   Main sketch — includes, setup(), loop()
├── config.h              Preset selection, hardware config, register
│                         addresses, hex print helpers
├── spi_linduino.h        PRESET_LINDUINO implementation
├── spi_api.h             PRESET_API implementation (ADI HAL style)
├── tests.h               Test routines (test_read_id, test_scratch,
│                         monitor_registers)
├── Linduino.h            Local copy of Linduino pin definitions
│                         (no external library required)
└── README.md             This file
```

## Usage

1. Open `AD9082_SPI_TEST.ino` in the Arduino IDE.
2. Select **Arduino Mega 2560** as the board.
3. Verify the desired preset is enabled in `config.h`.
4. Upload the sketch.
5. Open the Serial Monitor at **115200 baud**.

### Expected output (healthy bus)

```
=== AD9082 SPI Test ===
Preset: LINDUINO (direct SPI)
CS pin : 10
SPI Hz : 1000000

--- Read ID registers ---
  Chip ID     : 0x9082
  Chip grade  : 0x10
  SPI conf A  : 0x18
  ID read OK.

--- Scratch register (0x001C) write/read test ---
  Write 0x5A -> Read 0x5A  OK
  Write 0xA5 -> Read 0xA5  OK
  Write 0x0F -> Read 0x0F  OK
  Write 0xF0 -> Read 0xF0  OK
  Write 0x00 -> Read 0x00  OK
  All scratch patterns passed.

Setup complete. Monitoring every 3 seconds.
```

### Failure modes

| Symptom | Likely cause |
|---------|--------------|
| All reads = `0xFF` | Device not powered, CS not connected, or MISO floating |
| All reads = `0x00` | MOSI/SCLK not reaching the device, or CS stuck low |
| Scratch readback mismatch | SPI mode or clock polarity mismatch |

## Dependencies

None beyond the Arduino core.  `Linduino.h` is included locally — no external
Linduino library installation is needed.

## Power-up notes

- The AD9082 digital supplies (VDD1x = 1.0 V, VDD1P8 = 1.8 V, VDD2 = 2.0 V)
  must be up before SPI will respond.
- A reference clock is **not** required for the registers tested here.
- Recommended power-up order: supplies ON → wait 1 s → Linduino reset/upload.
