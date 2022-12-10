#ifndef MODE_SPARKLING_H
#define MODE_SPARKLING_H

#include "Arduino.h"
#include "FastLED.h"

void LED_Mode_Sparkling_Set_Color(uint8_t red, uint8_t green, uint8_t blue);
void LED_Mode_Sparkling(CRGB *ledsL, CRGB *ledsR);

#endif // MODE_SPARKLING_H