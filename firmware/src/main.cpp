#include <Arduino.h>
#include <TaskScheduler.h>

#include "led.h"
#include "oled.h"
#include "transducer.h"
#include "pins.h"

Scheduler runner;
Task Task_LED_WS2812(TASK_LED_INTERVAL, TASK_FOREVER, &Task_LED_Periodic, &runner, false, &Task_LED_Setup);
Task Task_OLED(TASK_OLED_INTERVAL, TASK_FOREVER, &Task_OLED_Periodic, &runner, false, &Task_OLED_Setup);
Task Task_Transducer(TASK_TRANSDUCER_INTERVAL, TASK_FOREVER, &Task_Transducer_Periodic, &runner, false, &Task_Transducer_Setup);

void setup() {
  Serial.begin(9600);

  Task_LED_WS2812.enable();
  Task_OLED.enable();
  Task_Transducer.enable();

  runner.startNow();
}

void loop() {
  runner.execute(); // It is necessary to run the runner on each loop
}