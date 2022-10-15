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
void DS2782_Init(I2C_TypeDef *i2c){

	I2C_Master_Init(i2c, I2C_GPIO_NOREMAP); //Init(i2c, 0);
}
//************************************************************
uint16_t DS2782_ReadADC(DS2782_Registers_t addrReg, uint8_t len){

	uint8_t rxBuf[2] = {0};
	//-------------------
	//Этот кусок кода работает. Обмен с DS2782 есть.
//	I2C_StartAndSendDeviceAddr(DS2782_I2C, DS2782_ADDR | I2C_MODE_WRITE);
//	I2C_SendData(DS2782_I2C, &regAddr, 1);
//	I2C_Stop(DS2782_I2C);
	//-------------------
	I2C_Master_Read(DS2782_I2C, DS2782_ADDR, (uint8_t)addrReg, rxBuf, len);
	return((rxBuf[1] << 8) | rxBuf[0]);
}
//************************************************************
void DS2782_GetI2cAddress(DS2782_t *ds){

	ds->I2C_Address = DS2782_ReadADC(Parameter_Address, 1) >> 1;
}
//************************************************************
void DS2782_GetID(DS2782_t *ds){

	ds->ID = DS2782_ReadADC(Register_Unique_ID, 1) >> 1;
}
//************************************************************
void DS2782_GetTemperature(DS2782_t *ds){

	uint16_t Temp   = DS2782_ReadADC(Register_TEMP, 2);
	         Temp   = ( ((Temp << 8) & 0xFF00) | ((Temp >> 8) & 0x00FF) );
	         Temp >>= 5;

	uint32_t tTemp =  Temp * 125;
			 //tTemp = (tTemp + 50) / 100;
			 ds->Temperature = (tTemp + 50) / 100;
}
//************************************************************
void DS2782_GetVoltage(DS2782_t *ds){

	//получение напряжения на АКБ.
	uint16_t voltTemp = DS2782_ReadADC(Register_VOLT, 2);
	voltTemp = ( ((voltTemp << 8) & 0xFF00) | ((voltTemp >> 8) & 0x00FF) );

	uint32_t adcTemp = voltTemp >> 5;   //

	adcTemp  &= 0b0000001111111111;//Уберем знак
	adcTemp  *= 488;               //это 4,88mV * 100. Это нужно чтобы избавится от запятой => получили микровольты
	//adcTemp = ((adcTemp + 50) / 100);
	adcTemp  *= 5500; //5475;              //это коэф-т деления резистивного делителя, умноженного на 1000.
	//adcTemp = ((adcTemp + 500000) / 1000000);
	ds->Voltage = ((adcTemp + 500000) / 1000000);
}
//************************************************************
//Получения тока потребления от АКБ.
void DS2782_GetCurrent(DS2782_t *ds){

	uint16_t temp = DS2782_ReadADC(Register_CURRENT, 2);
	int16_t  currentTemp = ( ((temp << 8) & 0xFF00) | ((temp >> 8) & 0x00FF) );
	int32_t  currentAdcTemp = currentTemp * 1563;
	ds->Current = ((currentAdcTemp + 5000) / 10000);
}
//************************************************************
void Coulomb_Calc(uint16_t current){

}
//*******************************************************************************************
//*******************************************************************************************




