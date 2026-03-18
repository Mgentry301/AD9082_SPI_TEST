// Linduino.h — Local copy for standalone Arduino IDE upload.
// Defines only the pin assignments used by this sketch.
//
// Original source: Analog Devices Linduino library
// Copyright 2018(c) Analog Devices, Inc. All rights reserved.

#ifndef LINDUINO_H
#define LINDUINO_H

#include <Arduino.h>

#define QUIKEVAL_GPIO 9          // QuikEval GPIO (connector pin 14)
#define QUIKEVAL_CS SS           // QuikEval SPI chip-select (connector pin 6)
#define QUIKEVAL_MUX_MODE_PIN 8  // I2C/SPI mux control on Linduino board

// Convenience macros
#define output_low(pin)   digitalWrite(pin, LOW)
#define output_high(pin)  digitalWrite(pin, HIGH)
#define input(pin)        digitalRead(pin)

#endif // LINDUINO_H
