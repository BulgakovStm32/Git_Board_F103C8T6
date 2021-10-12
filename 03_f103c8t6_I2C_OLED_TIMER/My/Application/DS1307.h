/* An STM32 HAL library written for the DS1307 real-time clock IC. */
/* Library by @eepj www.github.com/eepj */
#ifndef _DS1307_H_
#define _DS1307_H_
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
#define DS1307_I2C			I2C1
#define DS1307_I2C_ADDR 	0x68
//#define DS1307_I2C_TIMEOUT	1000

//DS1307 Time Registers
#define DS1307_REG_SECOND 	0x00
#define DS1307_REG_MINUTE 	0x01
#define DS1307_REG_HOUR  	0x02
#define DS1307_REG_DOW    	0x03
#define DS1307_REG_DATE   	0x04
#define DS1307_REG_MONTH  	0x05
#define DS1307_REG_YEAR   	0x06
#define DS1307_REG_CONTROL 	0x07

//DS1307 RAM
#define DS1307_REG_STATUS   0x08
#define DS1307_REG_UTC_HR	0x09
#define DS1307_REG_UTC_MIN	0x10
#define DS1307_REG_CENT    	0x11
#define DS1307_REG_RAM   	0x12

//**********************************
//Флаги статуса
#define DS1307_STATUS_FLAG_CONFIG  (1<<0) //1-была произведена настрока RTC;0-часы не настроены.

//*******************************************************************************************
typedef struct{
	uint8_t  hour;
	uint8_t  min;
	uint8_t  sec;
	uint8_t	 dayOfWeek;
	uint8_t  date;
	uint8_t  month;
	uint16_t year;
}DS1307_t;
//**********************************
typedef enum DS1307_Rate{
	DS1307_1HZ,
	DS1307_4096HZ,
	DS1307_8192HZ,
	DS1307_32768HZ
}DS1307_Rate;
//**********************************
typedef enum DS1307_SquareWaveEnable{
	DS1307_DISABLED,
	DS1307_ENABLED
} DS1307_SquareWaveEnable;
//*******************************************************************************************
//*******************************************************************************************
void DS1307_Init(I2C_TypeDef *hi2c);

void    DS1307_SetClockHalt(uint8_t halt);
uint8_t DS1307_GetClockHalt(void);

void DS1307_SetEnableSquareWave(DS1307_SquareWaveEnable mode);
void DS1307_SetInterruptRate(DS1307_Rate rate);

uint8_t  DS1307_GetDayOfWeek(void);
uint8_t  DS1307_GetDate(void);
uint8_t  DS1307_GetMonth(void);
uint16_t DS1307_GetYear(void);

uint8_t DS1307_GetHour(void);
uint8_t DS1307_GetMinute(void);
uint8_t DS1307_GetSecond(void);
int8_t  DS1307_GetTimeZoneHour(void);
uint8_t DS1307_GetTimeZoneMin(void);

void DS1307_SetDayOfWeek(uint8_t dow);
void DS1307_SetDate(uint8_t date);
void DS1307_SetMonth(uint8_t month);
void DS1307_SetYear(uint16_t year);

void DS1307_SetHour(uint8_t hour_24mode);
void DS1307_SetMinute(uint8_t minute);
void DS1307_SetSecond(uint8_t second);
void DS1307_SetTimeZone(int8_t hr, uint8_t min);

void DS1307_SetTimeAndCalendar(DS1307_t *time);
void DS1307_GetTimeAndCalendar(DS1307_t *time);

void    DS1307_SetStatusFlag(uint8_t flag);
void    DS1307_ResetStatusFlag(uint8_t flag);
uint8_t DS1307_GetStatusFlag(uint8_t flag);
//*******************************************************************************************
//*******************************************************************************************
#endif




