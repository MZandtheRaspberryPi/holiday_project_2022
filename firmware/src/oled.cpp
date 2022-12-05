#include "oled.h"

#include <Arduino.h>
#include "pins.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("Bat Morning Toposens!");
    display.display();

    return true;
}

void task_OLED_periodic(void)
{
    static uint8_t cnt = 0;
    static bool dir = 0;

    if (!cnt)
    {
        if (dir)
            display.startscrollright(0x00, 0x0f);
        else
            display.startscrollleft(0x00, 0x0F);
        
        dir = !dir;
    }
    cnt = (cnt + 1) & 15;
}