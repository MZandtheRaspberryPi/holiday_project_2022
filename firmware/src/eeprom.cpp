#include "eeprom.h"

#include "Arduino.h"
#include "EEPROM.h"

#define EEPROM_SIZE 512

#define EEPROM_PP_INIT_ADDR     0
#define EEPROM_PP_INIT_SIZE     1

// Primary pulse template
#define EEPROM_PP_TEMPLATE_ADDR 1
#define EEPROM_PP_TEMPLATE_SIZE (EEPROM_PP_TEMPLATE_MAX_SIZE * sizeof(uint16_t))

#define EEPROM_IS_PP_INIT       0x80

void EEPROM_Init(void)
{
    EEPROM.begin(EEPROM_SIZE);
}

bool EEPROM_Get_Is_Primary_Pulse_Init(void)
{
    return EEPROM.read(EEPROM_PP_INIT_ADDR) == EEPROM_IS_PP_INIT;
}

void EEPROM_Set_Is_Primary_Pulse_Init(void)
{
    EEPROM.write(EEPROM_PP_INIT_ADDR, EEPROM_IS_PP_INIT);
}

bool EEPROM_Get_Primary_Pulse_Template(uint16_t* pulseTemplate, uint16_t size)
{
    if (size > EEPROM_PP_TEMPLATE_MAX_SIZE)
        return false;

    for (uint16_t i = 0; i < size; ++i)
    {
        uint16_t tmp = 0;
        tmp = EEPROM.read(EEPROM_PP_TEMPLATE_ADDR + (i * 2)) << 8;
        tmp |= EEPROM.read(EEPROM_PP_TEMPLATE_ADDR + (i * 2) + 1);

        pulseTemplate[i] = tmp;
    }

    return true;
}

bool EEPROM_Set_Primary_Pulse_Template(uint16_t* pulseTemplate, uint16_t size)
{
    if (size > EEPROM_PP_TEMPLATE_MAX_SIZE)
        return false;

    for (uint16_t i = 0; i < size; ++i)
    {
        EEPROM.write(EEPROM_PP_TEMPLATE_ADDR + (i * 2), pulseTemplate[i] >> 8);
        EEPROM.write(EEPROM_PP_TEMPLATE_ADDR + (i * 2) + 1, pulseTemplate[i]);
    }

    return EEPROM.commit();
}