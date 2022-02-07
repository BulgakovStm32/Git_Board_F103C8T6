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
#define APB1_CLK			36000000U//Частота шины APB1 в Гц
#define I2C_RISE_TIME_100kHz	1000U    //Фронт сигнала в нС для 100кГц
#define I2C_RISE_TIME_400kHz	300U     //Фронт сигнала в нС для 400кГц
#define I2C_RISE_TIME_1000kHz	120U     //Фронт сигнала в нС для 1000кГц

#define I2C_TRISE_100kHz    ???
#define I2C_TRISE_400kHz    ???
#define I2C_TRISE_1000kHz   ???

//#define I2C_CR2_VALUE   (APB1_CLK / 1000000)
//#define I2C_CCR_VALUE	((APB1_CLK / I2C_BAUD_RATE) / 2)
//#define I2C_TRISE_VALUE ((1 / (I2C_TRISE * (APB1_CLK / 1000000))) + 1)
#define I2C_WAIT_TIMEOUT	500U
//--------------------------
#define I2C_MODE_READ  			1
#define I2C_MODE_WRITE 			0
#define I2C_ADDRESS(addr, mode) ((addr<<1) | mode)
//--------------------------
#define I2C_IT_STATE_IDLE				0
#define I2C_IT_STATE_START				1
#define I2C_IT_STATE_ADDR_SEND_OK       2


#define I2C_IT_STATE_TXReg_EMPTY		3
#define I2C_IT_STATE_EV8_1				4
#define I2C_IT_STATE_EV8				5
//*******************************************************************************************
//*******************************************************************************************
void I2C_Init(I2C_TypeDef *i2c, uint32_t remap);

uint32_t I2C_StartAndSendDeviceAddr(I2C_TypeDef *i2c, uint8_t DeviceAddr);
void 	 I2C_SendByte(I2C_TypeDef *i2c, uint8_t byte);
void     I2C_SendData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len);
void     I2C_ReadData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len);

void I2C_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len);
void I2C_Read (I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len);

//Работа по прерываниям.
void 	 I2C_IT_Init(I2C_TypeDef *i2c, uint32_t remap);
void 	 I2C_IT_StartTx(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t len);
void 	 I2C_IT_StartRx(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t len);

uint32_t I2C_IT_GetState(void);
uint32_t I2C_IT_GetData(I2C_TypeDef *i2c, uint8_t *pBuf, uint32_t len);

void I2C_IT_EV_Handler(I2C_TypeDef *i2c);//Обработчик прерывания событий I2C
void I2C_IT_ER_Handler(I2C_TypeDef *i2c);//Обработчик прерывания ошибок I2C
//*******************************************************************************************
//*******************************************************************************************
//Работа чере DMA.
#define I2C1_TX_DMAChannel	DMA1_Channel6
#define I2C1_RX_DMAChannel	DMA1_Channel7

#define I2C2_TX_DMAChannel	DMA1_Channel4
#define I2C2_RX_DMAChannel	DMA1_Channel5

typedef enum{
	I2C_DMA_NOT_INIT = 0,//I2C и DMA не инициализированны.
	I2C_DMA_READY,		 //I2C и DMA готовы к передаче данных.
	I2C_DMA_BUSY		 //I2C и DMA заняты, идет передача/прием данных.
}I2C_DMA_State_t;
//*******************************************************************************************
void 			I2C_DMA_Init(I2C_TypeDef *i2c, DMA_Channel_TypeDef *dma, uint32_t remap);
I2C_DMA_State_t I2C_DMA_State(void);

uint32_t I2C_DMA_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t size);
uint32_t I2C_DMA_Read (I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t *pBuf, uint32_t size);
//*******************************************************************************************
//*******************************************************************************************
#endif /* I2C_ST_H_ */
