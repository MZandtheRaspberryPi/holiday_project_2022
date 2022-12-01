#include <Arduino.h>
#include <TaskScheduler.h>

#include "led.h"

void led_blink();

Scheduler runner;
Task Task_LED_Blink(1000, TASK_FOREVER, &led_blink);
Task Task_LED_WS2812(500, TASK_FOREVER, &task_LED_periodic);

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  runner.addTask(Task_LED_Blink);
  runner.addTask(Task_LED_WS2812);

  task_LED_setup();

  Task_LED_Blink.enable();
  Task_LED_WS2812.enable();
}

void loop() {
  runner.execute(); // It is necessary to run the runner on each loop
}

void led_blink() {
  static bool statusLED = false;

  digitalWrite(LED_BUILTIN, statusLED);
  statusLED = !statusLED;
}