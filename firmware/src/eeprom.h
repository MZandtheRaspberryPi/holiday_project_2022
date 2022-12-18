#ifndef EEPROM_H
#define EEPROM_H

#include "stdint.h"

#define EEPROM_PP_TEMPLATE_MAX_SIZE 128u // uint16_t values

void EEPROM_Init(void);
bool EEPROM_Get_Is_Primary_Pulse_Init(void);
void EEPROM_Set_Is_Primary_Pulse_Init(void);
bool EEPROM_Get_Primary_Pulse_Template(uint16_t* pulseTemplate, uint16_t size);
bool EEPROM_Set_Primary_Pulse_Template(uint16_t* pulseTemplate, uint16_t size);

#endif // EEPROM_H