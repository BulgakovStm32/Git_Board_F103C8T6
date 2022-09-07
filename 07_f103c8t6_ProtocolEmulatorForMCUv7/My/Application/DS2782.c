/*
 * DS2782.c
 *
 *  Created on: 11 февр. 2021 г.
 *      Author: belyaev
 */
//DS2782 - микросхема определения емкости АКБ.
//*******************************************************************************************
//*******************************************************************************************

#include "DS2782.h"

//*******************************************************************************************
//*******************************************************************************************
void DS2782_Init(I2C_TypeDef *i2c, uint32_t i2cRemap){

	I2C_Init(i2c, i2cRemap);
}
//************************************************************
uint16_t DS2782_ReadData(DS2782_Registers_t addrReg, uint8_t len){

	uint8_t rxBuf[2] = {0};
	//-------------------
	if(I2C_Read(DS2782_I2C, DS2782_ADDR, (uint8_t)addrReg, rxBuf, len) != I2C_OK)
	{
		DS2782_Init(DS2782_I2C, I2C_GPIO_NOREMAP);
		return 0;
	}
	return((rxBuf[0]<<8) | rxBuf[1]);
}
//************************************************************
void DS2782_GetI2cAddress(DS2782_t *ds){

	ds->I2C_Address = DS2782_ReadData(Parameter_Address, 1) >> 9;
}
//************************************************************
void DS2782_GetID(DS2782_t *ds){

	uint8_t rxBuf[4] = {0};
	//-------------------
	if(I2C_Read(DS2782_I2C, DS2782_ADDR, Register_Unique_ID, rxBuf, 4) != I2C_OK)
	{
		DS2782_Init(DS2782_I2C, I2C_GPIO_NOREMAP);
		ds->ID = 0;
		return;
	}
	ds->ID = (rxBuf[3] << 24) |
			 (rxBuf[2] << 16) |
			 (rxBuf[1] << 8)  |
			  rxBuf[0];
}
//************************************************************
void DS2782_GetTemperature(DS2782_t *ds){

	uint16_t Temp   = DS2782_ReadData(Register_TEMP, 2)>>5;//младшие 5 бит незначащие.
	uint32_t tTemp  = Temp * 125;
	ds->Temperature = (tTemp + 50) / 100;
}
//************************************************************
void DS2782_GetVoltage(DS2782_t *ds){

	//получение напряжения на АКБ.
	uint32_t adcTemp = DS2782_ReadData(Register_VOLT, 2)>>5;//младшие 5 бит незначащие.

	adcTemp  &= 0b0000001111111111;//Уберем знак
	adcTemp  *= 488;               //это 4,88mV * 100. Это нужно чтобы избавится от запятой => получили микровольты
	adcTemp  *= 5475; 			   //это коэф-т деления резистивного делителя, умноженный на 1000.
	ds->Voltage = ((adcTemp + 500000) / 1000000);
}
//************************************************************
//Получения тока потребления от АКБ.
void DS2782_GetCurrent(DS2782_t *ds){

	int32_t currentAdcTemp = (int16_t)DS2782_ReadData(Register_CURRENT, 2) * 1563;//
	ds->Current 		   = ((currentAdcTemp + 5000) / 10000);
}
//************************************************************
//Получения усредненного за 28сек. тока.
void DS2782_GetAverageCurrent(DS2782_t *ds){

	int32_t currentAdcTemp = (int16_t)DS2782_ReadData(Register_IAVG, 2) * 1563;//
	ds->AverageCurrent 	   = ((currentAdcTemp + 5000) / 10000);
}
//************************************************************
void DS2782_GetAccumulatedCurrent(DS2782_t *ds){

	uint32_t currentAdcTemp = (DS2782_ReadData(Register_ACRL, 2)>>4) * 625;
	ds->AccumulatedCurrent	= ((currentAdcTemp + 500) / 1000);
}
//************************************************************
void Coulomb_Calc(uint16_t current){

}
//*******************************************************************************************
//*******************************************************************************************




