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
void ADS1115_Init(I2C_TypeDef *i2c){

	I2C_Init(i2c);

//	uint8_t conf[2];
	//Request
//	I2C_StartTransmission(I2C1,I2C_Direction_Transmitter,0x90);
//	I2C_Write_Data(I2C1,0x01);
//	I2C_GenerateSTOP(I2C1,ENABLE);
	//Response
//	I2C_StartTransmission(I2C1,I2C_Direction_Receiver,0x90);
//	conf[0] = I2C_Read_ack(I2C1);
//	conf[1] = I2C_Read_nack(I2C1);
//	I2C_GenerateSTOP(I2C1,ENABLE);
}
//************************************************************
//ADS1115 have 4 channel. We can select on of them in one moment.
void ADS1115_SelectInput(uint8_t ch){

	 uint8_t conf[2];
	 conf[1] = 227;

	 //create config for selecting channel
	 if(ch == 0) conf[0] = 195;
	 if(ch == 1) conf[0] = 211;
	 if(ch == 2) conf[0] = 227;
	 if(ch == 3) conf[0] = 243;

	//send config
//	 I2C_StartTransmission(I2C1,I2C_Direction_Transmitter,0x90);
//	 I2C_Write_Data(I2C1,0x01);
//	 I2C_Write_Data(I2C1,conf[0]);
//	 I2C_Write_Data(I2C1,conf[1]);
	 //I2C_GenerateSTOP(I2C1,ENABLE);

	 I2C_Write(ADS1115_I2C, ADS1115_ADDR, 0x01, conf, 2);
}
//************************************************************
uint16_t ADS1115_GetData(void){

//	uint8_t i2c_data[2];
	//Request
//	I2C_StartTransmission(I2C1,I2C_Direction_Transmitter,0x90);
//	I2C_Write_Data(I2C1,0x00);
//	I2C_GenerateSTOP(I2C1,ENABLE);
	//Response
//	I2C_StartTransmission(I2C1,I2C_Direction_Receiver,0x90);
//	i2c_data[0] = I2C_Read_ack(I2C1);
//	i2c_data[1] = I2C_Read_nack(I2C1);
//	uint16_t adc = (i2c_data[0] <<  8) | i2c_data[1];
	//I2C_GenerateSTOP(I2C1,ENABLE);

	uint8_t i2c_data[2];
	I2C_Read(ADS1115_I2C, ADS1115_ADDR, 0, i2c_data, 2);
	uint16_t adc = (i2c_data[0] <<  8) | i2c_data[1];
	return adc;
}

//From datasheet
/*
000 : AINP = AIN0 and AINN = AIN1 (default) 100 : AINP = AIN0 and AINN = GND
001 : AINP = AIN0 and AINN = AIN3 101 : AINP = AIN1 and AINN = GND
010 : AINP = AIN1 and AINN = AIN3 110 : AINP = AIN2 and AINN = GND
011 : AINP = AIN2 and AINN = AIN3 111 : AINP = AIN3 and AINN = GND
*/
//*******************************************************************************************
//*******************************************************************************************

