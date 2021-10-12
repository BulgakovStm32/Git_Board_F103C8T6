/*
 * i2c_ST.h
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */

#ifndef I2C_ST_H_
#define I2C_ST_H_
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"

//*******************************************************************************************
#define I2C_BAUD_RATE	400000  //Частота в Гц
#define I2C_TRISE       300		//значение в нС
#define APB1_CLK		36000000//Частота шины APB1 в Гц

//#define I2C_CR2_VALUE   (APB1_CLK / 1000000)
//#define I2C_CCR_VALUE	((APB1_CLK / I2C_BAUD_RATE) / 2)
//#define I2C_TRISE_VALUE ((1 / (I2C_TRISE * (APB1_CLK / 1000000))) + 1)
#define I2C_WAIT		0xFF
//--------------------------
#define I2C_MODE_READ  			1
#define I2C_MODE_WRITE 			0
#define I2C_ADDRESS(addr, mode) ((addr<<1) | mode)
//--------------------------
#define I2C_STATE_IDLE				0
#define I2C_STATE_START				1
#define I2C_STATE_ADDR_SEND_OK      2
#define I2C_STATE_TXReg_EMPTY		3
#define I2C_STATE_EV8_1				4
#define I2C_STATE_EV8				5
//*******************************************************************************************
//*******************************************************************************************
void 	I2C_Init(I2C_TypeDef *i2c);
uint8_t I2C_StartAndSendDeviceAddr(I2C_TypeDef *i2c, uint8_t DeviceAddr);
void    I2C_SendData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len);
void    I2C_ReadData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len);
void    I2C_Stop(I2C_TypeDef *i2c);

void I2C_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len);
void I2C_Read (I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len);

//работа по прерываниям.
void I2C_Int_Init(I2C_TypeDef *i2c);
void I2C_Int_StartSendData(I2C_TypeDef *i2c, uint8_t slaveAddr, uint8_t *pBuf, uint16_t len);
//*******************************************************************************************
//*******************************************************************************************
#endif /* I2C_ST_H_ */
