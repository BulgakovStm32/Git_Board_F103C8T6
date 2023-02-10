/*
 * main.h
 *
 *  Created on: Dec 18, 2020
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************
#ifndef _MAIN_H_
#define _MAIN_H_
//-------------------------
//Hardware.
#include "stm32f103xb.h"
#include "math.h"
//-------------------------
//Drivers.
#include "sys_ST.h"
#include "gpio_ST.h"
#include "sys_tick_ST.h"
#include "spi_ST.h"
#include "i2c_ST.h"
#include "timers_ST.h"
#include "dma_ST.h"
#include "adc_ST.h"
#include "uart_ST.h"
#include "flash_ST.h"
//-------------------------
//Планировщик
//#include "Scheduler.h"
//#include "rtos.h"
//-------------------------
//Application
#include "Delay.h"
#include "i2cBootLoaderProtocol.h"


//#include "LcdGraphic.h"
//#include "Blink.h"
//#include "DS18B20.h"
//#include "Filters.h"
//#include "AHT10.h"
//#include "DS2782.h"
//#include "EncoderMy.h"
//
//#include "ssd1306.h"
//#include "MyString.h"
//#include "PID.h"
//#include "Time.h"
//#include "Crc.h"
//
//#include "StringParser.h"
//#include "HC-12.h"
//#include "si5351.h"
//#include "Config.h"
//*******************************************************************************************
//*******************************************************************************************
#define FLAG_SET	1
#define FLAG_CLEAR	0

#define TRUE 		1
#define FALSE 		0
//*******************************************************************************************
//*******************************************************************************************
//extern void Reset_Handler(void);
//int  main(void);
void SysTick_IT_Handler(void);

//*******************************************************************************************
//*******************************************************************************************
#endif /* CORE_INC_MAIN_H_ */











