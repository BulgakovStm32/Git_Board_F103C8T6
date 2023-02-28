/*
 * i2c_ST.h
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */

#ifndef I2C_ST_H
#define I2C_ST_H
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
//Константы для настройки скорости работы I2C
#define APB1_CLK			36000000U 				//Частота шины APB1 в Гц
#define I2C_FREQ	    	(APB1_CLK / 1000000U)	//Peripheral clock frequency (MHz)

//Sm mode or SMBus:
//TPCLK1 = 27,7777 ns
//CCR    = 1000nS/ (2 * TPCLK1)
//TRISE  = (1000nS/TPCLK1)
#define I2C_SM_CCR			180 //(10000U / (2 * TPCLK1))
#define I2C_SM_TRISE		36  //(1000U  / TPCLK1)

//Fm mode:
//TPCLK1 = 27,7777 ns
//CCR    = 2500nS/ (3 * TPCLK1)
//TRISE  = (300nS/TPCLK1)
#define I2C_FM_CCR			30 //(2500U / (3 * TPCLK1))
#define I2C_FM_TRISE		12 //(300U  / TPCLK1)

//Таймаут ожидания сброса флага
#define I2C_WAIT_TIMEOUT	1000//500//5000//50000U
//--------------------------
#define I2C_MODE_READ  		1
#define I2C_MODE_WRITE 		0
#define I2C_ADDRESS(addr, mode) ((addr<<1) | mode)
//--------------------------
#define I2C_GPIO_NOREMAP	0
#define I2C_GPIO_REMAP		1

#define I2C_MODE_MASTER		0
#define I2C_MODE_SLAVE		1
//--------------------------
typedef enum{
	I2C_OK = 0,
	I2C_BUSY,		//Шина I2C занята (передача/прием данных)
	I2C_ERR_START,	//Ошибка при фоормировании Старт-последовательности
	I2C_ERR_ADDR,	//Ошибка адреса, Slave не отвечает.
	I2C_ERR_TX_BYTE,//Вышел таймаут передачи байта.
	I2C_ERR_RX_BYTE,//Вышел таймаут приема байта.
	I2C_ERR_BTF		//Вышел таймаут Byte transfer finished
}I2C_State_t;
//*******************************************************************************************
//*******************************************************************************************
//Общие функции.
I2C_State_t I2C_StartAndSendDeviceAddr(I2C_TypeDef *i2c, uint8_t deviceAddr);
I2C_State_t I2C_SendByte(I2C_TypeDef *i2c, uint8_t byte);
I2C_State_t I2C_ReadData(I2C_TypeDef *i2c, uint8_t *pBuf, uint32_t len);
void		I2C_Stop(I2C_TypeDef *i2c);
I2C_State_t I2C_SendDataWithStop(I2C_TypeDef *i2c, uint8_t *pBuf, uint32_t len);
I2C_State_t I2C_SendDataWithoutStop(I2C_TypeDef *i2c, uint8_t *pBuf, uint32_t len);

//************************************
//Функции для работы в режиме Master
void 		I2C_Master_Init(I2C_TypeDef *i2c, uint32_t remap, uint32_t speed);
uint32_t 	I2C_Master_GetNacCount(I2C_TypeDef *i2c);
uint32_t    I2C_Master_CheckSlave(I2C_TypeDef *i2c, uint8_t deviceAddr);
I2C_State_t I2C_Master_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t len);
I2C_State_t I2C_Master_Read (I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t len);

//************************************
//Функции для работы в режиме Slave
void I2C_Slave_Init(I2C_TypeDef *i2c, uint32_t remap, uint32_t slaveAddr, uint32_t speed);

//*******************************************************************************************
//*******************************************************************************************
//Работа по прерываниям.
#define I2C_IT_RX_BUF_LEN_MAX		(1024 + 16)
#define I2C_IT_TX_BUF_LEN_MAX		(1024 + 16)

#define I2C_IT_RX_BUF_SIZE_DEFAULT	32
#define I2C_IT_TX_BUF_SIZE_DEFAULT	32

//****************************************************
typedef enum{
  I2C_IT_STATE_RESET = 0,   	/*!< Peripheral is not yet Initialized         */
  I2C_IT_STATE_READY,  			/*!< Peripheral Initialized and ready for use  */
  I2C_IT_STATE_ADDR_WR,			//Принят адрес+Wr - мастер записывает данные
  I2C_IT_STATE_ADDR_RD,			//Принят адрес+Rd - мастер читает данные
  I2C_IT_STATE_BUSY_TX,   		/*!< Data Transmission process is ongoing      */
  I2C_IT_STATE_BUSY_RX,   		/*!< Data Reception process is ongoing         */
  I2C_IT_STATE_STOP,			//От Мастера принят STOP - признак завершения записи байтов.
  I2C_IT_STATE_NAC,				//от Мастера принят NACK - признак завершения чтения байтов.
}I2C_IT_State_t;
//****************************************************
//Структура контекста для работы с портом I2C по прерываниям.
//#pragma pack(push, 1)//размер выравнивания в 1 байт
typedef struct{
	I2C_TypeDef 	*i2c;			//
	uint32_t 	 	i2cMode	 : 1;	//Master или Slave
	uint32_t 		gpioRemap: 1;	//Ремап выводов для I2C1, для I2C2 ремапа нет.
	uint32_t 	 	i2cSpeed : 30;	//Скорость работы I2C

	I2C_IT_State_t 	itState;		//Состояние I2C (работа по прерываниям)
	uint32_t		timeOut;		//таймаут между запросами. Нужно для периициализации I2C в случае зависания.
//	uint32_t    	resetCount;		//Счетчик переинициализация i2c. Нужен для отладки.

	uint8_t			slaveAddr;		// В режиме Master - адрес Slave-устройства к которому идет обращение,
									// в режиме Slave  - адрес устройста на шине.

	uint8_t 		slaveRegAddr;	// В режиме Master - адрес регистра Slave-устройства куда хотим писать/читать данные.
									// в режиме Slave  - ???

	// буфер передачи.
	uint8_t  txBuf[I2C_IT_TX_BUF_LEN_MAX];
	uint32_t txBufIndex;	//индекс буфера передачи.
	uint32_t txBufSize;		//размер буфера передачи
	// буфер приема.
	uint8_t  rxBuf[I2C_IT_RX_BUF_LEN_MAX];
	uint32_t rxBufIndex;	//индекс буфера приема.
	uint32_t rxBufSize;		//размер буфера приема.

	void(*i2cSlaveRxCpltCallback)(void); 	//
	void(*i2cSlaveTxCpltCallback)(void); 	//
}I2C_IT_t;
//#pragma pack(pop)//вернули предыдущую настройку.
//*******************************************************************************************
//*******************************************************************************************
void 	 I2C_IT_Init(I2C_IT_t *i2c);
void 	 I2C_IT_SetTxSize(I2C_IT_t *i2cIt, uint32_t size);
uint8_t* I2C_IT_GetpTxBuf(I2C_IT_t *i2cIt);
uint8_t* I2C_IT_GetpRxBuf(I2C_IT_t *i2cIt);
uint32_t I2C_IT_GetLastEvent(I2C_TypeDef* I2Cx);

//Обработчики прерывания
void I2C_IT_EV_Handler(I2C_TypeDef *i2c);
void I2C_IT_ER_Handler(I2C_TypeDef *i2c);
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
#endif /* I2C_ST_H_ */






















