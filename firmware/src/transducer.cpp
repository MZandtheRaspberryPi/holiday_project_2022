#include "transducer.h"

#include <Arduino.h>
#include "user_interface.h"
#include "pins.h"
#include "led.h"
#include "oled.h"

#define PWM_FREQUENCY                   40000
#define ADC_SAMPLES_BASIC               20

#define TRANSDUCER_SWITCH_MIDDLE_TIME   3000
#define TRANSDUCER_SWITCH_LONG_TIME     7000

typedef enum {
    TRANSDUCER_SWITCH_STATE_NEAR = 0,
    TRANSDUCER_SWITCH_STATE_FAR
} TRANSDUCER_SwitchState_e;

typedef enum {
    TRANSDUCER_SWITCH_DURATION_SHORT = 0,
    TRANSDUCER_SWITCH_DURATION_MIDDLE,
    TRANSDUCER_SWITCH_DURATION_LONG
} TRANSDUCER_SwitchDuration_e;

static bool doAdvancedADC = false;

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
            if (switchDuration == TRANSDUCER_SWITCH_DURATION_SHORT && (millis() - startTime) > TRANSDUCER_SWITCH_MIDDLE_TIME)
            {
                switchDuration = TRANSDUCER_SWITCH_DURATION_MIDDLE;
                LED_Switch_Mode();
            }
            else if (switchDuration == TRANSDUCER_SWITCH_DURATION_MIDDLE && (millis() - startTime) > TRANSDUCER_SWITCH_LONG_TIME)
            {
                switchDuration = TRANSDUCER_SWITCH_DURATION_LONG;
                doAdvancedADC = !doAdvancedADC;
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

bool Basic_Presence_Detection(void)
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

    // Timeline_Draw_Data_Point(avg, 64, FRAME_PART_BOT);

    return avg > 2;
}

#define PULSE_LENGTH        (25 * 10)   // 40 kHz = 25 µs
#define ADC_CLOCK_DIV       21          // results in about 10µs adc sample time
#define ADC_FRAME_SAMPLES   256

static uint16_t adcFrame[ADC_FRAME_SAMPLES];
void Record_ADC_Frame(void)
{
    uint32_t startTime;

    analogWrite(PIN_PWM, 512);
    startTime = micros();

    while((micros() - startTime) < PULSE_LENGTH);
    analogWrite(PIN_PWM, 0);

    ets_intr_lock();
    noInterrupts();

    system_adc_read_fast(adcFrame, ADC_FRAME_SAMPLES, ADC_CLOCK_DIV);

    interrupts();
    ets_intr_unlock();
}

void Analyze_ADC_Frame(void)
{
    Plot_ADC_Dump(adcFrame, ADC_FRAME_SAMPLES);
}

bool Task_Transducer_Setup(void)
{
    analogWriteFreq(PWM_FREQUENCY);
    analogWriteRange(1023);

    return true;
}

void Task_Transducer_Periodic(void)
{
    if (!doAdvancedADC)
    {
        bool presenceDetected = Basic_Presence_Detection();
        Transducer_Handle_Switch(presenceDetected);
    }
    else
    {
        static bool measureFrame = true;

        if (measureFrame)
            Record_ADC_Frame();
        else
            Analyze_ADC_Frame();

        measureFrame = !measureFrame;
    }
}