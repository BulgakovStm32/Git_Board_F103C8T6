/*
 * Power.h
 *
 *  Created on: 30 авг. 2022 г.
 *      Author: belyaev
 */

#ifndef _Blink_H
#define _Blink_H
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
typedef enum{
	INTERVAL_50_mS = 0,
	INTERVAL_100_mS,
	INTERVAL_250_mS,
	INTERVAL_500_mS,
	INTERVAL_1000_mS,
}BlinkIntervalEnum_t;
//*******************************************************************************************
//*******************************************************************************************
void 	 Blink_ResetCount(void);
void     Blink_Loop(void);
uint32_t Blink(BlinkIntervalEnum_t interval);

//*******************************************************************************************
//*******************************************************************************************
#endif /*_Blink_H*/


