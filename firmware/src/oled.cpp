#include "oled.h"

#include <Arduino.h>
#include "pins.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH        128 // OLED display width, in pixels
#define SCREEN_HEIGHT       64 // OLED display height, in pixels

#define TIME_LINE_HEIGHT    (SCREEN_HEIGHT / 2)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void Frame_Shift(FRAME_ShiftDirection_e direction, FRAME_Part_e framePart = FRAME_PART_FULL)
{
    uint8_t height = framePart == FRAME_PART_FULL ? SCREEN_HEIGHT : SCREEN_HEIGHT / 2;
    uint16_t offset = framePart == FRAME_PART_BOT ? (SCREEN_WIDTH * SCREEN_HEIGHT / 8 / 2) : 0;
    uint8_t* displayBuffer = display.getBuffer();

    if (direction == FRAME_SHIFT_RIGHT)
    {
        for (int h = 0; h < (height / 8); ++h)
        {
            for (int w = (SCREEN_WIDTH - 1); w >= 0; --w)
            {
                uint16_t index = offset + (h * SCREEN_WIDTH) + w;

                if (w == 0)
                    displayBuffer[index] = 0;
                else
                    displayBuffer[index] = displayBuffer[index - 1];
            }
        }
    }
    else
    {
        for (int h = 0; h < (height / 8); ++h)
        {
            for (int w = 0; w < SCREEN_WIDTH; ++w)
            {
                uint16_t index = offset + (h * SCREEN_WIDTH) + w;

                if (w == (SCREEN_WIDTH - 1))
                    displayBuffer[index] = 0;
                else
                    displayBuffer[index] = displayBuffer[index + 1];
            }
        }
    }
}

void Timeline_Draw_Data_Point(uint16_t dataPoint, uint16_t scale, FRAME_Part_e framePart)
{
    uint8_t height = framePart == FRAME_PART_FULL ? SCREEN_HEIGHT : SCREEN_HEIGHT / 2;
    uint8_t offset = framePart == FRAME_PART_BOT ? (SCREEN_HEIGHT / 2) : 0;
    float scaleFactor = (float)height / scale;
    int16_t pointY = scaleFactor * dataPoint;

    if (pointY > (height - 1))
        pointY = height - 1;
    else if (pointY < 0)
        pointY = 0;

    Frame_Shift(FRAME_SHIFT_LEFT, framePart);

    display.drawPixel(SCREEN_WIDTH - 1, (offset + height - 1) - pointY, SSD1306_WHITE);
    // display.drawPixel(SCREEN_WIDTH - 1, (offset + height - 1), SSD1306_WHITE);

    display.display();
}

void Clear_Lower_Half(void)
{
    uint8_t* displayBuffer = display.getBuffer();

    memset(&displayBuffer[4 * SCREEN_WIDTH], 0, 4 * SCREEN_WIDTH);
}

void Plot_ADC_Dump(uint16_t* data, uint16_t size)
{
    if (size > (SCREEN_WIDTH * 2))
        return;

    Clear_Lower_Half();

    for (int line = 0; line < 2; ++line)
    {
        uint8_t lineLength;
        if (size < SCREEN_WIDTH)
        {
            lineLength = size;
        }
        else
        {
            lineLength = SCREEN_WIDTH;
        }
        size -= lineLength;

        for (int i = 0; i < lineLength; ++i)
        {
            uint16_t dataPoint = data[i + (line * SCREEN_WIDTH)];

            if (dataPoint > 15)
                dataPoint = 15;
            
            display.drawPixel(i, (47 + (16 * line)) - dataPoint, SSD1306_WHITE);
            display.drawPixel(i, (47 + (16 * line)), SSD1306_WHITE);
        }
    }
}

bool Task_OLED_Setup(void)
{
#ifdef CUSTOM_I2C_PINS
    Wire.begin(PIN_WIRE_SDA_CUSTOM, PIN_WIRE_SCL_CUSTOM);
#endif
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(30, 12);
    display.println("Good Morning!");
    display.display();
    // delay(2);

    // display.clearDisplay();

    return true;
}

void Task_OLED_Periodic(void)
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