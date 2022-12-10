#include "led.h"

#include <Arduino.h>
#include <FastLED.h>
#include "stdlib.h"

#include "pins.h"
#include "mode_sparkling.h"

#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB

typedef enum {
    LED_MODE_DEMO1,
    LED_MODE_DEMO2,
    LED_MODE_SPARKLING,
    LED_MODE_NUMBER
} LED_Modes_e;


CRGB ledsR[LEDS_PER_STRING];
CRGB ledsL[LEDS_PER_STRING];

static uint8_t ledMode;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for( int i = LEDS_PER_STRING; i >= 0 ; --i) {
        ledsR[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        ledsL[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;

    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

void LED_Mode_Demo(void)
{
    ChangePalettePeriodically();

    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors( startIndex);
}

void LED_Mode_Demo2(void)
{
    static uint32_t cnt = 0;
    uint16_t tmp_value = cnt % 1000;

    if (tmp_value < 500)
    {
        for (int i = 0; i < 5; ++i)         // for every led
        {
            int16_t pixel_intensity;
            uint8_t red_intensity;

            pixel_intensity = tmp_value - (i * 20); // shift color to negative side depending on led index

            if (pixel_intensity > 255)              // keep values in borders
                red_intensity = 255;
            else if (pixel_intensity < 0)
                red_intensity = 0;
            else
                red_intensity = pixel_intensity;

            ledsL[i].setRGB(red_intensity, 0, (255 - red_intensity));   // set colors to one LED on the left stripe
            ledsR[i].setRGB(red_intensity, 0, (255 - red_intensity));   // set colors to one LED on the right stripe
        }
    }
    else
    {
        tmp_value = 1000 - tmp_value;

        for (int i = 0; i < 5; ++i)         // for every led
        {
            int16_t pixel_intensity;
            uint8_t red_intensity;

            pixel_intensity = tmp_value - (i * 20); // shift color to negative side depending on led index

            if (pixel_intensity > 255)              // keep values in borders
                red_intensity = 255;
            else if (pixel_intensity < 0)
                red_intensity = 0;
            else
                red_intensity = pixel_intensity;

            ledsL[i].setRGB(red_intensity, 0, (255 - red_intensity));   // set colors to one LED on the left stripe
            ledsR[i].setRGB(red_intensity, 0, (255 - red_intensity));   // set colors to one LED on the right stripe
        }
    }

    ++cnt;
}

void LED_Switch_Mode(void)
{
    static uint32_t lastTime = 0;

    if ((millis() - lastTime) > 500)
    {
        ledMode = (ledMode + 1) % LED_MODE_NUMBER;
    }

    lastTime = millis();
}

bool task_LED_setup(void)
{
    // delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, PIN_WS2812_R, COLOR_ORDER>(ledsR, LEDS_PER_STRING).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, PIN_WS2812_L, COLOR_ORDER>(ledsL, LEDS_PER_STRING).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    LED_Mode_Sparkling_Set_Color(40, 0, 50);

    return true;
}

void task_LED_periodic(void)
{
    switch (ledMode)
    {
        case LED_MODE_DEMO1:
            LED_Mode_Demo();
            break;

        case LED_MODE_DEMO2:
            LED_Mode_Demo2();
            break;

        case LED_MODE_SPARKLING:
            LED_Mode_Sparkling(ledsL, ledsR);
            break;

        default:
            break;
    }

    FastLED.show();
}