#include "transducer.h"

#include <Arduino.h>
#include "pins.h"
#include "led.h"
#include "oled.h"

#define PWM_FREQUENCY       39000
#define ADC_SAMPLES_BASIC   20

void basic_presence_detection(void)
{
    analogWrite(PIN_PWM, 512);

    uint32_t sum = 0;
    uint16_t avg = 0;
    for (int i = 0; i < ADC_SAMPLES_BASIC; ++i)
    {
        sum += analogRead(PIN_ADC);
    }

    analogWrite(PIN_PWM, 0);

    avg = sum / ADC_SAMPLES_BASIC;

    if (avg > 2)
        LED_Switch_Mode();

    draw_data_point(avg, 64);
}

bool task_transducer_setup(void)
{
    analogWriteFreq(PWM_FREQUENCY);
    analogWriteRange(1023);

    return true;
}

void task_transducer_periodic(void)
{
    basic_presence_detection();
}