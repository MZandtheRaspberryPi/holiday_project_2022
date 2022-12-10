#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// Transducer
#define PIN_PWM D8
#define PIN_ADC A0

// OLED
#ifdef WAVESHARE_ESP
#define CUSTOM_I2C_PINS
#define PIN_WIRE_SDA_CUSTOM (13)
#define PIN_WIRE_SCL_CUSTOM (14)
#endif

// WS2812
#define PIN_WS2812_L   D5
#define PIN_WS2812_R   D3

#endif // PINS_H