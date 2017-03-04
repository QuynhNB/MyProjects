#ifndef CURRENT_MEAS_H
#define CURRENT_MEAS_H


#include "main.h"


void ADC_Initialize(void);
uint32_t AIRCON_1_GetCurrent(void);
uint32_t AIRCON_2_GetCurrent(void);

#endif
