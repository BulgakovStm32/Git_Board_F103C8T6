/*
 * 	sys_tick_ST.h
 *
 *  Created on:
 *      Author: belyaev
 */

#ifndef _sys_tick_ST_H
#define _sys_tick_ST_H
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************

#define F_CPU			72000000UL		//Тактовая частота 72МГЦ.
#define SYS_TICK_LOAD	(F_CPU/1000-1)	//Значение для генерации прерывания куждую 1мс

//*******************************************************************************************
//*******************************************************************************************
void SysTick_Init(void);

//*******************************************************************************************
//*******************************************************************************************
#endif /*_sys_tick_ST_H*/


