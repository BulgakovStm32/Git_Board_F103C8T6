/*
 * Delay.h
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */

#ifndef DELAY_H_
#define DELAY_H_
//************************************************************************************************************
//************************************************************************************************************

#include "main.h"

//************************************************************************************************************
void msDelay_Loop(void);
void msDelay(volatile uint32_t del);

void 	 MICRO_DELAY_Init(void);
uint32_t MICRO_DELAY_GetCount(void);
void 	 MICRO_DELAY(uint32_t microSec);
//************************************************************************************************************
//************************************************************************************************************
#endif /* DELAY_H_ */







