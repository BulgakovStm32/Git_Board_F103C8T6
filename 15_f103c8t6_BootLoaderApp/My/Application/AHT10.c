/*
 *  AHT10.c
 *
 *  Created on: 25 янв. 2021 г.
 *      Author: belyaev
 */
//AHT10 - датчик температуры и влажности.
//*******************************************************************************************
//*******************************************************************************************

#include "AHT10.h"

//*******************************************************************************************
//*******************************************************************************************
static uint8_t 		  rxBuf[6] = {0};
static AHT10_Status_t status   = AHTXX_STATUS_RESET;

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
static void _readMeasurement(void){

	//-------------------
	//Прочитаем данные (6 байтов).
	if(I2C_Master_ReadData(AHT10_I2C, AHT10_ADDR, rxBuf, 6) != I2C_OK)
	{
		status = AHTXX_STATUS_ERR_ACK;
		return;
	}
	//-------------------
	//Проверим флаг BUSY
	if(rxBuf[0] & AHTXX_STATUS_CTRL_BUSY)
	{
		//Модуль занят измерением...
		status = AHTXX_STATUS_BUSY;
		return;
	}
	//-------------------
	//Модуль закончил измерения и данные вылидны.
	//Передадим команду на запуск измерения.
	uint8_t txBuf[3];
	txBuf[0] = AHTXX_START_MEASUREMENT_REG;			//send measurement command, strat measurement
	txBuf[1] = AHTXX_START_MEASUREMENT_CTRL;		//send measurement control
	txBuf[2] = AHTXX_START_MEASUREMENT_CTRL_NOP;	//send measurement NOP control
	if(I2C_Master_SendData(AHT10_I2C, AHT10_ADDR, txBuf, 3) != I2C_OK)
	{
		status = AHTXX_STATUS_ERR_ACK;
		return;
	}
	status = AHTXX_STATUS_OK;
	//-------------------
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void AHT10_Init(void){

	status = AHTXX_STATUS_INIT;
}
//************************************************************
void AHT10_SoftReset(void){

	uint8_t cmd = 0xBA;//soft reset command
	//-------------------
	I2C_StartAndSendDeviceAddr(AHT10_I2C, AHT10_ADDR);
	//I2C_SendData(I2C2, &cmd, 1);
	I2C_SendDataWithStop(AHT10_I2C, &cmd, 1);

//	aht10_write(0xBA, uint8_t *data, uint8_t size);//0xBA - soft reset command
	msDelay(50);
}
//************************************************************
void AHT10_ReadData(void){

	//Чтение данных
	_readMeasurement();

//	static uint32_t flag = 0;
//	//-------------------
//	if(!flag)
//	{
//		//Передача команды на запус измерения
//		/* send measurement command */
//		rxBuf[0] = AHTXX_START_MEASUREMENT_REG;			//send measurement command, strat measurement
//		rxBuf[1] = AHTXX_START_MEASUREMENT_CTRL;		//send measurement control
//		rxBuf[2] = AHTXX_START_MEASUREMENT_CTRL_NOP;	//send measurement NOP control
//
//		if(I2C_Master_SendData(AHT10_I2C, AHT10_ADDR, rxBuf, 3) != I2C_OK)
//		{
//			status = AHTXX_STATUS_ERR_ACK;
//			return;
//		}
//		flag = 1;
//		return;
//	}
//	//-------------------
//	//TODO ... /* check busy bit */
//
//	//DELAY_milliS(AHTXX_MEASUREMENT_DELAY);
//
//	//-------------------
//	/* read data from sensor */
//	flag = 0;
//	if(I2C_Master_ReadData(AHT10_I2C, AHT10_ADDR, rxBuf, 6) != I2C_OK)
//	{
//		status = AHTXX_STATUS_ERR_ACK;
//		return;
//	}
//	status = AHTXX_STATUS_OK;
}
//**********************************************************
AHT10_Status_t AHT10_GetStatus(void){

	return status;
}
//**********************************************************
int32_t AHT10_GetTemperature(void){

	int32_t temp;
	//-------------------
	//Расчет температуры.
	//Выражение для преобразования температуры, °C: T = AHT10_ADC_Raw * 200 / 2^20 - 50
	temp  = ((rxBuf[3] & 0x0F) << 16) | (rxBuf[4] << 8) | rxBuf[5];//собираем значение температуры
	temp *= 19; // 19 - это 200 / 2^20 * 100000
	temp  = temp - (50 * 100000);
	return ((temp + 1000/2) / 1000);
}
//**********************************************************
uint32_t  AHT10_GetHumidity(void){

	uint32_t hum;
	const uint32_t div = 1<<20;//это 2^20
	//-------------------
	//Расчет влажности
	//Выражение для преобразования относительной влажности, %: H = AHT10_ADC_Raw * 100 / 2^20
	hum   = (uint32_t)((rxBuf[1] << 16) | (rxBuf[2] << 8) | (rxBuf[3] & 0xF0));//собираем значение влажности
	hum >>= 4;
	return ((hum * 100 + div/2) / div);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************









