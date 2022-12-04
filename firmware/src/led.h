#ifndef LED_H
#define LED_H

#define TASK_LED_FREQUENCY  100                         // Hz
#define TASK_LED_INTERVAL   (1000 / TASK_LED_FREQUENCY) // ms

bool task_LED_setup(void);
void task_LED_periodic(void);

#endif // LED_H