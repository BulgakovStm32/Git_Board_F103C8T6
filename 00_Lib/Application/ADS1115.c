/*
 * ADS1115.c
 *
 *  Created on:
 *      Author:
 */
//ADS1115 - 16-битный АЦП, обмен по шине I2C.
//*******************************************************************************************
//*******************************************************************************************

#include "ADS1115.h"

//*******************************************************************************************
//*******************************************************************************************
static void i2c_read(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t len){

	I2C_Read(i2c, deviceAddr, regAddr, pBuf, len);
}
//************************************************************
static void i2c_write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t len){

	I2C_Write(i2c, deviceAddr, regAddr, pBuf, len);
}
//*******************************************************************************************
//*******************************************************************************************
void ADS1115_Init(ADS1115_t *ads1115){

	I2C_Init(ads1115->i2c);

	uint16_t config = ads1115->ConfigReg;
	i2c_write(ADS1115_I2C, ADS1115_I2C_ADDR, ADS1115_CONFIG_REG_ADDR, (uint8_t*)&config, 2);
}
//************************************************************
//ADS1115 have 4 channel. We can select on of them in one moment.
void ADS1115_SelectChannel(ADS1115_t *ads1115, uint32_t ch){

	//create config for selecting channel
	ads1115->ConfigReg &= ~(0b111 << ADS1115_MUX_Pos);
	ads1115->ConfigReg |=  (ch    << ADS1115_MUX_Pos);

	i2c_write(ads1115->i2c,
			  ads1115->I2cAddr,
			  ADS1115_CONFIG_REG_ADDR,
			  (uint8_t*)&ads1115->ConfigReg, 2);
}
//************************************************************
uint16_t ADS1115_GetAdcData(ADS1115_t *ads1115){

	uint16_t adcCode = 0;

	i2c_read(ads1115->i2c,
			 ads1115->I2cAddr,
			 ADS1115_CONVERSION_REG_ADDR,
			 (uint8_t*)&adcCode, 2);

	ads1115->ConversionReg = ((adcCode<<8)&0xFF00) | (adcCode>>8);
	return ads1115->ConversionReg;
}
//************************************************************
void ADS1115_ReadAdcData(ADS1115_t *ads1115){

	volatile uint16_t adcCode = 0;

	i2c_read(ads1115->i2c,
			 ads1115->I2cAddr,
			 ADS1115_CONVERSION_REG_ADDR,
			 (uint8_t*)&adcCode, 2);

	ads1115->ConversionReg = ((adcCode<<8)&0xFF00) | (adcCode>>8);
}
//*******************************************************************************************
//*******************************************************************************************

