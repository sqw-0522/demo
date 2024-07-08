#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"

void adc_init(void);                                                /* ADC≥ı ºªØ */
void HAL_ADC_MSPInit(ADC_HandleTypeDef *hadc);
uint32_t adc_get_result(void);

#endif 

