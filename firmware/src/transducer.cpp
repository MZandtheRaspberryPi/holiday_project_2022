#include "transducer.h"

#include <Arduino.h>
#include "user_interface.h"
#include "pins.h"
#include "led.h"
#include "oled.h"
#include "animate_face.h"

#define PWM_FREQUENCY                   40000
#define ADC_SAMPLES_BASIC               20

#define TRANSDUCER_SWITCH_MIDDLE_TIME   3000
#define TRANSDUCER_SWITCH_LONG_TIME     7000

#define PERIOD_COUNT                    2
#define PULSE_LENGTH                    ((25 * PERIOD_COUNT) - 50)  // 40 kHz = 25 µs; 50 µs for function calls
#define ADC_CLOCK_DIV                   21                          // results in about 10µs adc sample time
#define ADC_FRAME_SAMPLES               256

#define ADC_FRAME_AVERAGE_MASK_SIZE     32
#define ADC_FRAME_AVERAGE_DATA_SIZE     (ADC_FRAME_SAMPLES - (ADC_FRAME_AVERAGE_MASK_SIZE - 1))

typedef enum {
    TRANSDUCER_SWITCH_STATE_NEAR = 0,
    TRANSDUCER_SWITCH_STATE_FAR
} TRANSDUCER_SwitchState_e;

typedef enum {
    TRANSDUCER_SWITCH_DURATION_SHORT = 0,
    TRANSDUCER_SWITCH_DURATION_MIDDLE,
    TRANSDUCER_SWITCH_DURATION_LONG
} TRANSDUCER_SwitchDuration_e;

static uint16_t adcFrame[ADC_FRAME_SAMPLES];
static bool doAdvancedADC = true;
static bool plotAdcDump = false;

// pirmary pulse < 128 samples
uint16_t adc_base[ADC_FRAME_AVERAGE_DATA_SIZE] = {177, 186, 196, 196, 208, 214, 223, 230, 240, 246, 245, 252, 252, 268, 264, 272,
                                                        262, 262, 268, 262, 274, 253, 254, 245, 243, 253, 232, 238, 221, 221, 229, 213,
                                                        221, 199, 201, 195, 186, 192, 173, 177, 162, 160, 165, 151, 154, 139, 140, 132,
                                                        125, 128, 115, 116, 106, 105, 105, 95, 97, 87, 87, 82, 78, 79, 70, 70, 64, 63,
                                                        61, 56, 57, 52, 51, 49, 47, 47, 43, 43, 40, 38, 38, 36, 36, 34, 34, 32, 32, 32,
                                                        30, 30, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 26, 25, 26, 26, 25, 25, 25,
                                                        25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
                                                        25, 24, 25, 25, 25, 25, 25, 25, 24, 25, 24, 24, 24, 25, 24, 24, 25, 24, 24, 25,
                                                        25, 25, 25, 24, 24, 24, 24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 24, 25,
                                                        25, 25, 24, 24, 25, 25, 25, 25, 25, 25, 25, 24, 24, 24, 25, 25, 25, 25, 25, 25,
                                                        25, 24, 25, 25, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24,
                                                        24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24};

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
            animationTriggerActionMode();
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
                plotAdcDump = !plotAdcDump;
                Show_ADC_Data(plotAdcDump);
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

void Record_ADC_Frame(uint16_t* adcData, uint16_t size)
{
    uint32_t startTime = micros();

    digitalWrite(PIN_PWM, HIGH);
    while((micros() - startTime < 12));
    digitalWrite(PIN_PWM, LOW);

    // analogWrite(PIN_PWM, 512);
    // startTime = micros();

    // while((micros() - startTime) < PULSE_LENGTH);
    // analogWrite(PIN_PWM, 0);

    ets_intr_lock();
    noInterrupts();

    system_adc_read_fast(adcData, size, ADC_CLOCK_DIV);

    interrupts();
    ets_intr_unlock();
}

void Calc_Average_Data(uint16_t* dataIn, uint16_t* dataOut)
{
    uint32_t sum = 0;

    for (int i = 0; i < ADC_FRAME_AVERAGE_MASK_SIZE; ++i)
    {
        sum += dataIn[i];
    }
    dataOut[0] = sum / ADC_FRAME_AVERAGE_MASK_SIZE;

    for (int i = 1; i < ADC_FRAME_AVERAGE_DATA_SIZE; ++i)
    {
        sum -= dataIn[i - 1];
        sum += dataIn[i + ADC_FRAME_AVERAGE_MASK_SIZE - 1];
        dataOut[i] = sum / ADC_FRAME_AVERAGE_MASK_SIZE;
    }
}

bool Analyze_ADC_Frame(void)
{
    uint16_t avgData[ADC_FRAME_AVERAGE_DATA_SIZE];

    Calc_Average_Data(adcFrame, avgData);

    uint32_t sum = 0;
    for (int i = 0; i < ADC_FRAME_AVERAGE_DATA_SIZE; ++i)
    {
        sum += avgData[i];
    }

    // Plot_ADC_Dump(adcFrame, 256);
    Plot_ADC_Dump(avgData, 256);

    return sum > 20000;
}

void Print_Primary_Pulse_Array(void)
{
    uint16_t tmpAdcFrame[ADC_FRAME_SAMPLES];
    uint16_t tmpAvgData[2][ADC_FRAME_AVERAGE_DATA_SIZE] = {0};

    for (int i = 0; i < 5; ++i)
    {
        delay(500);
        Record_ADC_Frame(tmpAdcFrame, ADC_FRAME_SAMPLES);
        Calc_Average_Data(tmpAdcFrame, tmpAvgData[0]);

        for (int n = 0; n < ADC_FRAME_AVERAGE_DATA_SIZE; ++n)
        {
            if (tmpAvgData[0][n] > tmpAvgData[1][n])
                tmpAvgData[1][n] = tmpAvgData[0][n];
        }
    }

    for (int n = 0; n < ADC_FRAME_AVERAGE_DATA_SIZE; ++n)
    {
        Serial.printf("%u, ", tmpAvgData[1][n] + 20);
    }
}

bool Task_Transducer_Setup(void)
{
    analogWriteFreq(PWM_FREQUENCY);
    analogWriteRange(1023);
    pinMode(PIN_PWM, OUTPUT);

    Print_Primary_Pulse_Array();

    return true;
}

void Task_Transducer_Periodic(void)
{
    bool presenceDetected = false;

    if (!doAdvancedADC)
    {
        presenceDetected = Basic_Presence_Detection();
        Transducer_Handle_Switch(presenceDetected);
    }
    else
    {
        static bool measureFrame = true;

        if (measureFrame)
            Record_ADC_Frame(adcFrame, ADC_FRAME_SAMPLES);
        else
        {
            presenceDetected = Analyze_ADC_Frame();
            Transducer_Handle_Switch(presenceDetected);
        }

        measureFrame = !measureFrame;
    }
}