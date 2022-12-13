#include "oled.h"

#include <Arduino.h>
#include "pins.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH            128 // OLED display width, in pixels
#define SCREEN_HEIGHT           64 // OLED display height, in pixels
#define SCREEN_BUFFER_HEIGHT    (SCREEN_HEIGHT / 8) // 8 Pixel in a column are packed into one byte

const uint8_t LEFT_EYE_X_START = SCREEN_WIDTH / 8;
const uint8_t EYE_Y_START = SCREEN_HEIGHT / 8;
const uint8_t EYE_WIDTH = SCREEN_WIDTH / 6;
const uint8_t EYE_HEIGHT = SCREEN_HEIGHT / 2;
const uint8_t EYE_RADIUS = SCREEN_HEIGHT / 8;
const uint8_t RIGHT_EYE_X_START =  SCREEN_WIDTH - LEFT_EYE_X_START - EYE_WIDTH ;

const uint8_t MOUTH_X_1 = SCREEN_WIDTH * 2 / 8;
const uint8_t MOUTH_X_2 = SCREEN_WIDTH - MOUTH_X_1;
const uint8_t MOUTH_X_3 = ((MOUTH_X_2 - MOUTH_X_1 ) / 2) + MOUTH_X_1;
const uint8_t MOUTH_Y_1 = SCREEN_HEIGHT * 12 / 16;
const uint8_t MOUTH_Y_2 = MOUTH_Y_1;
const uint8_t MOUTH_Y_3 = SCREEN_HEIGHT * 15 / 16;
const uint8_t MOUTH_HEIGHT = MOUTH_Y_3 - MOUTH_Y_1;

const uint8_t FANG_WIDTH = SCREEN_WIDTH * 2 / 32;
const uint8_t FANG_1_HEIGHT = SCREEN_HEIGHT * 4 / 16;
const uint8_t FANG_2_HEIGHT = SCREEN_HEIGHT * 3 / 16;

const uint8_t FANG_1_X_1 = ((MOUTH_X_3 - MOUTH_X_1) / 2 ) + MOUTH_X_1 - (FANG_WIDTH / 2);
const uint8_t FANG_1_X_2 = FANG_1_X_1 + FANG_WIDTH;
const uint8_t FANG_1_X_3 = ((FANG_1_X_2 - FANG_1_X_1) / 2) + FANG_1_X_1;
const uint8_t FANG_1_Y_1 = MOUTH_Y_1;
const uint8_t FANG_1_Y_2 = MOUTH_Y_1;
const uint8_t FANG_1_Y_3 = FANG_1_Y_1 + FANG_1_HEIGHT;

const uint8_t FANG_2_X_1 = ((MOUTH_X_3 - MOUTH_X_2) / 2 ) + MOUTH_X_2 - (FANG_WIDTH / 2);
const uint8_t FANG_2_X_2 = FANG_2_X_1 + FANG_WIDTH;
const uint8_t FANG_2_X_3 = ((FANG_2_X_2 - FANG_2_X_1 ) / 2) + FANG_2_X_1;
const uint8_t FANG_2_Y_1 = MOUTH_Y_1;
const uint8_t FANG_2_Y_2 = MOUTH_Y_1;
const uint8_t FANG_2_Y_3 = FANG_2_Y_1 + FANG_2_HEIGHT;


const uint16_t ANIMATION_MIN_COUNT_DURATION = 20;
const uint16_t ANIMATION_MAX_COUNT_DURATION = 100;
const uint16_t ANIMATION_TIMELINE_MAX = 300;
const uint16_t ANIMATION_NUM_ANIMATIONS = 3;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void Frame_Shift(FRAME_ShiftDirection_e direction, FRAME_Part_e framePart = FRAME_PART_FULL)
{
    uint8_t height = framePart == FRAME_PART_FULL ? SCREEN_BUFFER_HEIGHT : SCREEN_BUFFER_HEIGHT / 2;
    uint16_t offset = framePart == FRAME_PART_BOT ? (SCREEN_WIDTH * SCREEN_BUFFER_HEIGHT / 2) : 0;
    uint8_t* displayBuffer = display.getBuffer();

    if (direction == FRAME_SHIFT_RIGHT)
    {
        for (int h = 0; h < height; ++h)
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
        for (int h = 0; h < height; ++h)
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

void Frame_Clear(FRAME_Part_e framePart = FRAME_PART_FULL)
{
    uint16_t size = framePart == FRAME_PART_FULL ? SCREEN_WIDTH * SCREEN_BUFFER_HEIGHT : SCREEN_WIDTH * SCREEN_BUFFER_HEIGHT / 2;
    uint16_t offset = framePart == FRAME_PART_BOT ? SCREEN_WIDTH * SCREEN_BUFFER_HEIGHT / 2 : 0;
    uint8_t* displayBuffer = display.getBuffer();

    memset(&displayBuffer[0 + offset], 0, size);
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
    // display.drawPixel(SCREEN_WIDTH - 1, (offset + height - 1), SSD1306_WHITE); // draw zero line

    display.display();
}

void Plot_ADC_Dump(uint16_t* data, uint16_t size)
{
    uint16_t plotHeight = SCREEN_HEIGHT / 2;
    uint16_t maxDataVal = plotHeight - 1;

    if (size > (SCREEN_WIDTH * 2))
        return;

    Frame_Clear();

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

            if (dataPoint > maxDataVal)
                dataPoint = maxDataVal;
            
            display.drawPixel(i, (maxDataVal + (plotHeight * line)) - dataPoint, SSD1306_WHITE);
            display.drawPixel(i, (maxDataVal + (plotHeight * line)), SSD1306_WHITE);
        }
    }

    display.display();
}

void displayFace(uint8_t left_eye_y_offset, uint8_t right_eye_y_offset, uint8_t mouth_y_offset)
{
  display.clearDisplay();
  display.fillTriangle(MOUTH_X_1, MOUTH_Y_1, MOUTH_X_2, MOUTH_Y_2, MOUTH_X_3,
                    MOUTH_Y_3 - mouth_y_offset, SSD1306_WHITE);

  display.fillTriangle(FANG_1_X_1, FANG_1_Y_1, FANG_1_X_2, FANG_1_Y_2, FANG_1_X_3,
                  FANG_1_Y_3, SSD1306_WHITE);

  display.fillTriangle(FANG_2_X_1, FANG_2_Y_1, FANG_2_X_2, FANG_2_Y_2, FANG_2_X_3,
                  FANG_2_Y_3, SSD1306_WHITE);

  display.fillRoundRect(LEFT_EYE_X_START, EYE_Y_START + left_eye_y_offset,
                        EYE_WIDTH, EYE_HEIGHT - left_eye_y_offset,
      EYE_RADIUS, SSD1306_WHITE);

  display.fillRoundRect(RIGHT_EYE_X_START, EYE_Y_START + right_eye_y_offset,
                      EYE_WIDTH, EYE_HEIGHT - right_eye_y_offset,
    EYE_RADIUS, SSD1306_WHITE);
  display.display();
}


void blinkEyes(const uint16_t counter, const uint16_t end_count)
{
  static bool first_call_in_cycle = true;
  static uint8_t num_blinks;
  static uint16_t time_for_animation;
  static float y_offset_increment;
  static bool eyes_open = true;
  static float y_offset = 0;

  if (first_call_in_cycle)
  {
    num_blinks = random(1, 4);
    time_for_animation = end_count - counter;
    y_offset_increment = static_cast<float>(EYE_HEIGHT) * num_blinks * 2 / time_for_animation;
    if (y_offset_increment == 0)
    {
      y_offset_increment = 1;
    }
    first_call_in_cycle = false;
  }

  if (counter >= end_count - 1)
  {
    eyes_open = true;
    first_call_in_cycle = true;
    return;
  }

  if (eyes_open)
  {
    y_offset += y_offset_increment;
  }
  else
  {
    y_offset -= y_offset_increment;
  }

  if (y_offset >= EYE_HEIGHT)
  {
    y_offset = EYE_HEIGHT;
    eyes_open = false;
  }
  else if (y_offset <= 0)
  {
    y_offset = 0;
    eyes_open = true;
  }

   displayFace(y_offset, y_offset, 0);
}
void actuateMouth(const uint16_t counter, const uint16_t end_count)
{
  static bool first_call_in_cycle = true;
  static uint8_t num_mouth_cycles;
  static uint16_t time_for_animation;
  static float y_offset_increment;
  static bool mouth_open = true;
  static float y_offset = 0;

  if (first_call_in_cycle)
  {
    num_mouth_cycles = random(3, 4);
    time_for_animation = end_count - counter;
    y_offset_increment = static_cast<float>(MOUTH_HEIGHT) * num_mouth_cycles * 2 / time_for_animation;
    if (y_offset_increment == 0)
    {
      y_offset_increment = 1;
    }
    first_call_in_cycle = false;
  }

  if (counter >= end_count - 1)
  {
    mouth_open = true;
    first_call_in_cycle = true;
    return;
  }

  if (mouth_open)
  {
    y_offset += y_offset_increment;
  }
  else
  {
    y_offset -= y_offset_increment;
  }

  if (y_offset >= MOUTH_HEIGHT)
  {
    y_offset = MOUTH_HEIGHT;
    mouth_open = false;
  }
  else if (y_offset <= 0)
  {
    y_offset = 0;
    mouth_open = true;
  }

   displayFace(0, 0, y_offset);
}

void squintEyes(const uint16_t counter, const uint16_t end_count)
{
  static bool first_call_in_cycle = true;
  static uint8_t num_eye_squints;
  static uint16_t time_for_animation;
  static uint16_t flip_increment;
  static bool right_eye_squint = true;
  static float y_offset = 0;

  if (first_call_in_cycle)
  {
    num_eye_squints = random(1, 4);
    time_for_animation = end_count - counter;
    flip_increment = time_for_animation / num_eye_squints;
    y_offset = random(0, static_cast<float>(EYE_HEIGHT)/2);
    first_call_in_cycle = false;
  }

  if (counter >= end_count - 1)
  {
    right_eye_squint = true;
    first_call_in_cycle = true;
    return;
  }

  int left_eye_offset = 0;
  int right_eye_offset = 0;
  if (right_eye_squint)
  {
    right_eye_offset = y_offset;
  }
  else
  {
    left_eye_offset = y_offset;
  }

  if (counter % flip_increment == 0)
  {
    right_eye_squint = !right_eye_squint;
  }

   displayFace(left_eye_offset, right_eye_offset, 0);
}

void animateFace()
{

  // blink eyes
  // close and open mouth
  // alternate left / right eye shorter
  static uint16_t counter = 0;

  static uint8_t action_number = random(0, 10);
  static uint16_t animation_count_duration = random(ANIMATION_MIN_COUNT_DURATION, ANIMATION_MAX_COUNT_DURATION);
  static uint16_t count_end = counter + animation_count_duration;

  if (action_number >= 0 && action_number < 4)
  {
    displayFace(0, 0, 0);
  }
  else if (action_number >= 4 && action_number < 6)
  {
    blinkEyes(counter, count_end);
  }
  else if (action_number >= 6 && action_number < 8)
  {
    actuateMouth(counter, count_end);
  }
  else
  {
    squintEyes(counter, count_end);
  }

  counter++;
  if (counter > ANIMATION_TIMELINE_MAX)
  {
    counter = 0;
  }

  if (counter > count_end || counter == 0)
  {
    if (action_number < 4)
    {
      action_number = random(0, 10);
    }
    else
    {
      action_number = 0;
    }
    animation_count_duration = random(ANIMATION_MIN_COUNT_DURATION, ANIMATION_MAX_COUNT_DURATION);
    count_end = counter + animation_count_duration;
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
    displayFace(0, 0, 0);
    return true;
}

void Task_OLED_Periodic(void)
{
    // if we are in adc dump mode, show graph
    // else show face
    animateFace();
}