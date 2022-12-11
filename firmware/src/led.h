#ifndef LED_H
#define LED_H

#include "stdint.h"

#define TASK_LED_FREQUENCY  100                         // Hz
#define TASK_LED_INTERVAL   (1000 / TASK_LED_FREQUENCY) // ms

#define LEDS_PER_STRING     5
#define BRIGHTNESS          100

typedef struct Color_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}Color_t;

void LED_Switch_Mode(void);
void LED_Trigger_Action_Mode(void);
bool task_LED_setup(void);
void task_LED_periodic(void);

#endif // LED_H