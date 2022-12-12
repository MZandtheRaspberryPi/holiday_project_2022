#include "led.h"

#include <Arduino.h>
#include <FastLED.h>
#include "stdlib.h"

#include "pins.h"
#include "mode_sparkling.h"

#define LED_TYPE                WS2812B
#define COLOR_ORDER             GRB

#define LED_FADE_ON_DIFF        15
#define LED_FADE_MAX_INTENSITY  100
#define LED_FADE_ON_MAX_COUNT   (LED_FADE_MAX_INTENSITY + (LEDS_PER_STRING * LED_FADE_ON_DIFF))
#define NUM_OF_WING_FLAPS       3

typedef enum {
    LED_FADE_OFF,
    LED_FADE_ON
} LED_FadeDirection_e;

typedef enum {
    LED_MODE_DEMO1,
    LED_MODE_DEMO2,
    LED_MODE_SPARKLING,
    LED_MODE_NUMBER
} LED_Modes_e;

typedef enum {
    LED_ACTION_MODE_WING_FLAP,
    LED_ACTION_MODE_NUMBER
} LED_ActionMode_e;

typedef struct {
    bool isStarted;
    bool direction;
    uint8_t speed;
    int16_t count;
} LED_FadeLightCurtain_t;

CRGB ledsR[LEDS_PER_STRING];
CRGB ledsL[LEDS_PER_STRING];

static uint8_t ledMode = LED_MODE_SPARKLING;
static uint8_t ledActionMode = LED_ACTION_MODE_WING_FLAP;
static bool ledActionModeActive = false;

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

bool LED_Fade_Light_Curtain(uint8_t* pixelIntensity, LED_FadeLightCurtain_t* fadeData)
{
    bool isFinished = false;

    if (!fadeData->isStarted)
    {
        fadeData->isStarted = true;
        if (fadeData->direction)
            fadeData->count = 0;
        else
            fadeData->count = LED_FADE_ON_MAX_COUNT;
    }

    for (int i = 0; i < LEDS_PER_STRING; ++i)         // for every led
    {
        int16_t pixelIntensityTemp;

        pixelIntensityTemp = fadeData->count - (i * LED_FADE_ON_DIFF); // shift color to negative side depending on led index

        if (pixelIntensityTemp > LED_FADE_MAX_INTENSITY)                     // keep values in borders
            pixelIntensity[i] = LED_FADE_MAX_INTENSITY;
        else if (pixelIntensityTemp < 0)
            pixelIntensity[i] = 0;
        else
            pixelIntensity[i] = pixelIntensityTemp;
    }

    if (fadeData->direction)
    {
        fadeData->count += fadeData->speed;
        if (fadeData->count > LED_FADE_ON_MAX_COUNT)
        {
            isFinished = true;
        }
    }
    else
    {
        fadeData->count -= fadeData->speed;
        if (fadeData->count < 0)
        {
            isFinished = true;
        }
    }

    if (isFinished)
    {
        fadeData->isStarted = false;
        for (int i = 0; i < LEDS_PER_STRING; ++i)   // make sure, the intensity is at the expected finish value
        {
            if (fadeData->direction)
                pixelIntensity[i] = LED_FADE_MAX_INTENSITY;
            else
                pixelIntensity[i] = 0;
        }
    }

    return isFinished;
}

void LED_Mode_Demo2(void)
{
    static LED_FadeLightCurtain_t fadeData = {.direction = LED_FADE_ON, .speed = 1};
    uint8_t pixelIntensity[LEDS_PER_STRING];
    bool isFinished = LED_Fade_Light_Curtain(pixelIntensity, &fadeData);

    for (uint8_t i = 0; i < LEDS_PER_STRING; ++i)
    {
        ledsL[i].setRGB(pixelIntensity[i], 0, (LED_FADE_MAX_INTENSITY - pixelIntensity[i]));   // set colors to one LED on the left stripe
        ledsR[i].setRGB(pixelIntensity[i], 0, (LED_FADE_MAX_INTENSITY - pixelIntensity[i]));   // set colors to one LED on the right stripe
    }

    if (isFinished)
        fadeData.direction = !fadeData.direction;
}

void LED_Mode_Wing_Flap(void)
{
    static LED_FadeLightCurtain_t fadeData = {.direction = LED_FADE_ON, .speed = 8};
    static uint8_t flapCount = 0;
    uint8_t pixelIntensity[LEDS_PER_STRING];
    bool isFinished = LED_Fade_Light_Curtain(pixelIntensity, &fadeData);

    for (uint8_t i = 0; i < LEDS_PER_STRING; ++i)
    {
        ledsL[i].setRGB(pixelIntensity[i], 0, 0);   // set colors to one LED on the left stripe
        ledsR[i].setRGB(pixelIntensity[i], 0, 0);   // set colors to one LED on the right stripe
    }

    if (isFinished)
    {
        if (fadeData.direction == LED_FADE_OFF)
            ++flapCount;

        fadeData.direction = !fadeData.direction;
    }

    if (flapCount >= NUM_OF_WING_FLAPS)
    {
        flapCount = 0;
        ledActionModeActive = false;
    }
}

void LED_Switch_Mode(void)
{
    ledMode = (ledMode + 1) % LED_MODE_NUMBER;
}

void LED_Trigger_Action_Mode(void)
{
    ledActionModeActive = true;
}

bool Task_LED_Setup(void)
{
    // delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, PIN_WS2812_R, COLOR_ORDER>(ledsR, LEDS_PER_STRING).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, PIN_WS2812_L, COLOR_ORDER>(ledsL, LEDS_PER_STRING).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    LED_Mode_Sparkling_Set_Color(0, 50, 50);

    return true;
}

void Task_LED_Periodic(void)
{
    if (ledActionModeActive)
    {
        switch (ledActionMode)
        {
            case LED_ACTION_MODE_WING_FLAP:
                LED_Mode_Wing_Flap();
                break;
            
            default:
                break;
        }
    }
    else
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
    }

    FastLED.show();
}