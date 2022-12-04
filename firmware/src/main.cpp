#include <Arduino.h>
#include <TaskScheduler.h>

#include "led.h"
#include "oled.h"
#include "transducer.h"

// void led_blink();

Scheduler runner;
// Task Task_LED_Blink(1000, TASK_FOREVER, &led_blink, &runner, false);
Task Task_LED_WS2812(TASK_LED_INTERVAL, TASK_FOREVER, &task_LED_periodic, &runner, false, &task_LED_setup);
Task Task_OLED(TASK_OLED_INTERVAL, TASK_FOREVER, &task_OLED_periodic, &runner, false, &task_OLED_setup);
Task Task_Transducer(TASK_TRANSDUCER_INTERVAL, TASK_FOREVER, &task_transducer_periodic, &runner, false, &task_transducer_setup);

void setup() {
  Serial.begin(9600);

  // pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Task_LED_Blink.enable();
  // Task_LED_WS2812.enable();
  Task_OLED.enable();
  Task_Transducer.enable();

  runner.startNow();
}

void loop() {
  runner.execute(); // It is necessary to run the runner on each loop
}

// void led_blink() {
//   static bool statusLED = false;

//   digitalWrite(LED_BUILTIN, statusLED);
//   statusLED = !statusLED;
// }