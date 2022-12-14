#include "mode_sparkling.h"

#include "led.h"

#define UNEVEN_LED_CNT          (LEDS_PER_STRING % 2)
#define SPARKLING_FADE_SPEED    2

typedef struct ledSparkling_t
{
    bool isActive;
    uint16_t cnt;
    uint8_t intensity;
    Color_t targetColor;
    Color_t color;
}LedDataSparkling_t;


static LedDataSparkling_t ledDataL[LEDS_PER_STRING] = {0};
static LedDataSparkling_t ledDataR[LEDS_PER_STRING] = {0};

void LED_Mode_Sparkling_Set_Color(uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < LEDS_PER_STRING; ++i)
    {
        ledDataL[i].targetColor.red = red;
        ledDataL[i].targetColor.green = green;
        ledDataL[i].targetColor.blue = blue;
    }
    for (uint8_t i = 0; i < LEDS_PER_STRING; ++i)
    {
        ledDataR[i].targetColor.red = red;
        ledDataR[i].targetColor.green = green;
        ledDataR[i].targetColor.blue = blue;
    }
}

void LED_Handle_Sparkling_Single_Fade(ledSparkling_t *ledData)
{
    if (!ledData->isActive)
        return;

    if (ledData->cnt <= 255)
    {
        ledData->intensity = ledData->cnt;
    }
    else
    {
        ledData->intensity = 511 - ledData->cnt;
    }

    ledData->cnt += SPARKLING_FADE_SPEED;

    if (ledData->cnt >= 512)
    {
        ledData->cnt = 0;
        ledData->intensity = 0;
        ledData->isActive = false;
    }

    ledData->color.red = ((float)ledData->targetColor.red / 255) * ledData->intensity;
    ledData->color.green = ((float)ledData->targetColor.green / 255) * ledData->intensity;
    ledData->color.blue = ((float)ledData->targetColor.blue / 255) * ledData->intensity;
}

void LED_Handle_Sparkling_Stripe(ledSparkling_t *ledData, CRGB *leds)
{
    for (uint8_t i = 0; i < LEDS_PER_STRING; ++i)
    {
        uint8_t index = i;

        // add some randomness to the led order
        if (i % 2)
            index = (LEDS_PER_STRING - UNEVEN_LED_CNT) - i;

        if (index == 0) // first LED
        {
            if (!ledData[index + 1].isActive && (rand() % 1000 == 0)) // second LED is off
                ledData[index].isActive = true;
        }
        else if (index == (LEDS_PER_STRING - 1)) // last LED
        {
            if (!ledData[index - 1].isActive && (rand() % 1000 == 0)) // second last LED is off
                ledData[index].isActive = true;
        }
        else if (!ledData[index - 1].isActive && !ledData[index + 1].isActive) // LED before and after are off
        {
            if (rand() % 1000 == 0)
                ledData[index].isActive = true;
        }

        LED_Handle_Sparkling_Single_Fade(&ledData[index]);
        leds[index].setRGB(ledData[index].color.red, ledData[index].color.green, ledData[index].color.blue);
    }
}

void LED_Mode_Sparkling(CRGB *ledsL, CRGB *ledsR)
{
    LED_Handle_Sparkling_Stripe(ledDataL, ledsL);
    LED_Handle_Sparkling_Stripe(ledDataR, ledsR);
}