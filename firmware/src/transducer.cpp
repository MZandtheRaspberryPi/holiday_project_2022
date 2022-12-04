#include "transducer.h"

#include <Arduino.h>

#define PWM_PIN D0
#define ADC_PIN A0

#define PWM_FREQUENCY   39000

bool task_transducer_setup(void)
{
    analogWriteFreq(PWM_FREQUENCY);
    analogWrite(PWM_PIN, 127);

    pinMode(LED_BUILTIN, OUTPUT);

    return true;
}

void task_transducer_periodic(void)
{
    static bool led_state = false;
    uint16_t adc_value = analogRead(ADC_PIN);

    if (adc_value > 500)
        led_state = HIGH;
    else if (adc_value < 300)
        led_state = LOW;
}