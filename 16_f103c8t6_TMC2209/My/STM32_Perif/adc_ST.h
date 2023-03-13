/*
 * 	adcST.h
 *
 *  Created on:
 *      Author: belyaev
 */

#ifndef _ADC_ST_H
#define _ADC_ST_H
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
#define STM32_ADC_V_REF			3265U 	//Опорное напряжение
#define STM32_ADC_RESOLUTION	4096U 	//
#define STM32_ADC_SCALE			1024U	//
#define STM32_ADC_QUANT_uV		(uint32_t)((STM32_ADC_V_REF * STM32_ADC_SCALE + STM32_ADC_RESOLUTION/2) / STM32_ADC_RESOLUTION)

#define STM32_ADC_MEAS_NUM		16     	//Количество измерений для усреднения
#define STM32_ADC_MEAS_SHIFT	4      	//Количество сдвигов для деления
//************************************


//*******************************************************************************************
//*******************************************************************************************
//Аппатартный уровень.
void     STM32_ADC_Init(void);
uint16_t STM32_ADC_GetMeas_mV(uint32_t adcCh);

uint32_t STM32_ADC_GetRegDR(ADC_TypeDef *adc);
uint32_t STM32_ADC_GetRegJDRx(ADC_TypeDef *adc, uint32_t ch);
//*******************************************************************************************
//*******************************************************************************************
#endif /*_adc_ST_HH*/





