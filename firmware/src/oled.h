#ifndef OLED_H
#define OLED_H

#include "stdint.h"

#define TASK_OLED_FREQUENCY 1                             // Hz
#define TASK_OLED_INTERVAL  (1000 / TASK_OLED_FREQUENCY)   // ms

bool task_OLED_setup(void);
void task_OLED_periodic(void);
void draw_data_point(uint16_t dataPoint, uint16_t scale);

#endif // OLED_H