#include "transducer.h"

#include <Arduino.h>
#include "pins.h"
#include "led.h"
#include "oled.h"

#define PWM_FREQUENCY               39000
#define ADC_SAMPLES_BASIC           20

#define TRANSDUCER_SWITCH_LONG_TIME 3000

typedef enum {
    TRANSDUCER_SWITCH_STATE_NEAR = 0,
    TRANSDUCER_SWITCH_STATE_FAR
} TRANSDUCER_SwitchState_e;

typedef enum {
    TRANSDUCER_SWITCH_DURATION_SHORT = 0,
    TRANSDUCER_SWITCH_DURATION_LONG
} TRANSDUCER_SwitchDuration_e;

void Transducer_Handle_Switch(bool switchInput)
{
    static uint32_t startTime = 0;
    static uint32_t lastTime = 0;
    static TRANSDUCER_SwitchState_e switchState = TRANSDUCER_SWITCH_STATE_FAR;
    static TRANSDUCER_SwitchDuration_e switchDuration = TRANSDUCER_SWITCH_DURATION_SHORT;

    if (switchState == TRANSDUCER_SWITCH_STATE_FAR)
    {
        if (switchInput)
        {
            switchState = TRANSDUCER_SWITCH_STATE_NEAR;
            switchDuration = TRANSDUCER_SWITCH_DURATION_SHORT;
            startTime = millis();
            lastTime = startTime;
            LED_Trigger_Action_Mode();
        }
    }
    else
    {
        if (switchInput)
        {
            if (switchDuration == TRANSDUCER_SWITCH_DURATION_SHORT && (millis() - startTime) > TRANSDUCER_SWITCH_LONG_TIME)
            {
                switchDuration = TRANSDUCER_SWITCH_DURATION_LONG;
                LED_Switch_Mode();
            }

            lastTime = millis();
        }
        else
        {
            if ((millis() - lastTime) > 500)
            {
                switchState = TRANSDUCER_SWITCH_STATE_FAR;
            }
        }
    }
}

bool basic_presence_detection(void)
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

    draw_data_point(avg, 64);

    return avg > 2;
}

bool task_transducer_setup(void)
{
    analogWriteFreq(PWM_FREQUENCY);
    analogWriteRange(1023);

    return true;
}

void task_transducer_periodic(void)
{
    bool presenceDetected = basic_presence_detection();
    Transducer_Handle_Switch(presenceDetected);
}