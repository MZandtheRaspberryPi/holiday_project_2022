#include "transducer.h"

#include <Arduino.h>
#include "pins.h"

#define PWM_FREQUENCY   39000

bool task_transducer_setup(void)
{
    analogWriteFreq(PWM_FREQUENCY);
    analogWrite(PIN_PWM, 127);

    pinMode(LED_BUILTIN, OUTPUT);

    return true;
}

void task_transducer_periodic(void)
{
    static bool led_state = false;
    uint16_t adc_value = analogRead(PIN_ADC);

    if (adc_value > 500)
        led_state = HIGH;
    else if (adc_value < 300)
        led_state = LOW;
}