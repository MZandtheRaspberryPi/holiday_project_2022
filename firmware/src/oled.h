#ifndef OLED_H
#define OLED_H

#define TASK_OLED_FREQUENCY  10                             // Hz
#define TASK_OLED_INTERVAL   (1000 / TASK_OLED_FREQUENCY)   // ms

bool task_OLED_setup(void);
void task_OLED_periodic(void);

#endif // LED_H