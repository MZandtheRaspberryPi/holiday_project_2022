#include "oled.h"

#include <Arduino.h>
#include "pins.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

typedef enum {
    SHIFT_FRAME_LEFT = 0,
    SHIFT_FRAME_RIGHT
}ShiftDirection_e;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void shift_frame(ShiftDirection_e direction)
{
    uint8_t* displayBuffer = display.getBuffer();

    if (direction == SHIFT_FRAME_RIGHT)
    {
        for (int h = 0; h < (SCREEN_HEIGHT / 8); ++h)
        {
            for (int w = (SCREEN_WIDTH - 1); w >= 0; --w)
            {
                if (w == 0)
                    displayBuffer[(h * SCREEN_WIDTH) + w] = 0;
                else
                    displayBuffer[(h * SCREEN_WIDTH) + w] = displayBuffer[(h * SCREEN_WIDTH) + w - 1];
            }
        }
    }
    else
    {
        for (int h = 0; h < (SCREEN_HEIGHT / 8); ++h)
        {
            for (int w = 0; w < SCREEN_WIDTH; ++w)
            {
                if (w == (SCREEN_WIDTH - 1))
                    displayBuffer[(h * SCREEN_WIDTH) + w] = 0;
                else
                    displayBuffer[(h * SCREEN_WIDTH) + w] = displayBuffer[(h * SCREEN_WIDTH) + w + 1];
            }
        }
    }
}

void draw_data_point(uint16_t dataPoint, uint16_t scale)
{
    // uint8_t* displayBuffer = display.getBuffer();
    float scaleFactor = SCREEN_HEIGHT / scale;
    int16_t height = dataPoint * scaleFactor;

    if (height > (SCREEN_HEIGHT - 1))
        height = SCREEN_HEIGHT - 1;
    else if (height < 0)
        height = 0;

    shift_frame(SHIFT_FRAME_LEFT);

    display.drawPixel(SCREEN_WIDTH - 1, (SCREEN_HEIGHT - 1) - height, SSD1306_WHITE);


    display.display();
}

bool task_OLED_setup(void)
{
#ifdef CUSTOM_I2C_PINS
    Wire.begin(PIN_WIRE_SDA_CUSTOM, PIN_WIRE_SCL_CUSTOM);
#endif
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    // delay(2000);
    display.clearDisplay();

    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.setCursor(0, 10);
    // // Display static text
    // display.println("Bat Morning Toposens!");
    display.display();

    return true;
}

void task_OLED_periodic(void)
{
    // static uint8_t cnt = 0;
    // static bool dir = 0;

    // if (!cnt)
    // {
    //     if (dir)
    //         display.startscrollright(0x00, 0x0f);
    //     else
    //         display.startscrollleft(0x00, 0x0F);
        
    //     dir = !dir;
    // }
    // cnt = (cnt + 1) & 15;

    // shift_frame(SHIFT_FRAME_RIGHT);
    // display.display();
}