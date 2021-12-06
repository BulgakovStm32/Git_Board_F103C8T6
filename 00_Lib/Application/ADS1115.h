/*
 * ADS1115.h
 *
 *  Created on:
 *      Author:
 */
#ifndef ADS1115_H_
#define ADS1115_H_
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
#define ADS1115_I2C_ADDR 	(0b1001000 << 1)
#define ADS1115_I2C			I2C2

//Регистры ADS1115.
#define ADS1115_CONVERSION_REG_ADDR	 0 //Регистр хранения результата преобразования
#define ADS1115_CONFIG_REG_ADDR		 1 //Конфигурационный регистр
#define ADS1115_LoTHRESHOLD_REG_ADDR 2 //Регистр уставки, минимальное значение	
#define ADS1115_HiTHRESHOLD_REG_ADDR 3 //Регистр уставки, максимальное значение	00000011

//Описание регистра конфигурации ADS1115_CONFIG_REG.
//MUX - Настройка мультиплексора.
#define ADS1115_MUX_Pos			12
#define ADS1115_MUX_AIN0_AIN1	0 //000 : AINP = AIN0 and AINN = AIN1 (default)
#define ADS1115_MUX_AIN0_AIN3	1 //001 : AINP = AIN0 and AINN = AIN3
#define ADS1115_MUX_AIN1_AIN3	2 //010 : AINP = AIN1 and AINN = AIN3
#define ADS1115_MUX_AIN2_AIN3	3 //011 : AINP = AIN2 and AINN = AIN3
#define ADS1115_MUX_AIN0_GND	4 //100 : AINP = AIN0 and AINN = GND
#define ADS1115_MUX_AIN1_GND	5 //101 : AINP = AIN1 and AINN = GND
#define ADS1115_MUX_AIN2_GND	6 //110 : AINP = AIN2 and AINN = GND
#define ADS1115_MUX_AIN3_GND	7 //111 : AINP = AIN3 and AINN = GND

//PGA - Коэффициент усиления усилителя
#define ADS1115_PGA_Pos			9
#define ADS1115_PGA_2_3			0 //FS=±6,144 В
#define ADS1115_PGA_1			1 //FS=±4,096 В
#define ADS1115_PGA_2			2 //FS=±2,048 В  (умолч.)
#define ADS1115_PGA_4			3 //FS=±1,024 В
#define ADS1115_PGA_8			4 //FS=±0,512 В
#define ADS1115_PGA_16			5 //FS=±0,256 В
//#define ADS1115_PGA_16		6 //FS=±0,256 В
//#define ADS1115_PGA_16		7 //FS=±0,256 В

//MODE - Режим работы.
#define ADS1115_MODE_Pos		8
#define ADS1115_MODE_CONTINUOUS	0 //Непрерывное преобразование
#define ADS1115_MODE_SINGLE		1 //Одиночное преобразование, режим пониженного потребления (умолч)

//DR - Частота дискретизации.
#define ADS1115_DR_Pos			5
#define ADS1115_DR_8Hz			0
#define ADS1115_DR_16Hz			1
#define ADS1115_DR_32Hz			2
#define ADS1115_DR_64Hz			3
#define ADS1115_DR_128Hz		4 //128 ГЦ (умолч)
#define ADS1115_DR_250Hz		5
#define ADS1115_DR_475Hz		6
#define ADS1115_DR_860Hz		7
//**********************************
typedef struct{
	I2C_TypeDef *i2c;
	uint8_t		I2cAddr;
	uint16_t	ConfigReg;
	uint16_t	ConversionReg;
}ADS1115_t;
//*******************************************************************************************
//*******************************************************************************************
void 	 ADS1115_Init(ADS1115_t *ads1115);
void     ADS1115_SelectChannel(ADS1115_t *ads1115, uint32_t ch);
uint16_t ADS1115_GetAdcData(ADS1115_t *ads1115);
void     ADS1115_ReadAdcData(ADS1115_t *ads1115);
//*******************************************************************************************
//*******************************************************************************************
#endif /* ADS1115_H_ */

