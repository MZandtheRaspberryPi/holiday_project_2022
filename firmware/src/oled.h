#ifndef OLED_H
#define OLED_H

#include "stdint.h"

#define TASK_OLED_FREQUENCY 100                             // Hz
#define TASK_OLED_INTERVAL  (1000 / TASK_OLED_FREQUENCY)   // ms

typedef enum {
    FRAME_SHIFT_LEFT = 0,
    FRAME_SHIFT_RIGHT
}FRAME_ShiftDirection_e;

typedef enum {
    FRAME_PART_FULL = 0,
    FRAME_PART_TOP,
    FRAME_PART_BOT
}FRAME_Part_e;

bool Task_OLED_Setup(void);
void Task_OLED_Periodic(void);
void Timeline_Draw_Data_Point(uint16_t dataPoint, uint16_t scale, FRAME_Part_e framePart = FRAME_PART_FULL);
void Plot_ADC_Dump(uint16_t* data, uint16_t size);

void displayFace(uint8_t left_eye_y_offset, uint8_t right_eye_y_offset, uint8_t mouth_y_offset);
void blinkEyes(const uint16_t counter, const uint16_t end_count);
void actuateMouth(const uint16_t counter, const uint16_t end_count);
void squintEyes(const uint16_t counter, const uint16_t end_count);
void animateFace();

#endif // OLED_H