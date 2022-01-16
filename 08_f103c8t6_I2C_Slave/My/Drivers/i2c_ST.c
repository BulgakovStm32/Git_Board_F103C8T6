/*
 * i2c_ST.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author:
 */
//*******************************************************************************************
//*******************************************************************************************

#include "i2c_ST.h"

//*******************************************************************************************
//*******************************************************************************************
static uint32_t I2C_LongWait(I2C_TypeDef *i2c, uint32_t flag){

	uint32_t wait_count = 0;
	//---------------------
	while(!(i2c->SR1 & flag))//Ожидаем окончания ....
	{
		if(++wait_count >= I2C_WAIT) return 1;
	}
	return 0;
}
//**********************************************************


//*******************************************************************************************
//*******************************************************************************************
void I2C_Slave_Init(I2C_TypeDef *i2c, uint32_t slaveAddr, uint32_t remap){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
	//------------------------------
	//Тактирование I2C_1
	if(i2c == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		//Ремап: I2C1_SCL - PB8, I2C1_SDA - PB9.
		if(remap)
		{
			AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
			GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1 |
						  GPIO_CRH_CNF8    | GPIO_CRH_CNF9;
		}
		else
		{
			//I2C1_SCL - PB6, I2C1_SDA - PB7.
			GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 |
						  GPIO_CRL_CNF6    | GPIO_CRL_CNF7;

		}
	}
	//------------------------------
	//Тактирование I2C_2
	else if(i2c == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		//I2C2_SCL - PB10, I2C2_SDA - PB11.
		GPIOB->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 |
					  GPIO_CRH_CNF10    | GPIO_CRH_CNF11;
	}
	else return;
	//------------------------------
	//Инициализация I2C в режиме Slave.
	i2c->CR1  |=  I2C_CR1_PE;   //Включение модуля I2C1.

	i2c->CR1 &= ~I2C_CR1_SMBUS; //модуль работает в режиме I2C
	i2c->SR2 &= ~I2C_SR2_MSL;   //режим Slave.

	i2c->OAR1 = slaveAddr << 1; //адрес устройства на шине.
	i2c->CR1 |= I2C_CR1_ACK;	//разрешаем отправлять ACK/NACK после приема байта адреса.

	//i2c->CR1 |= I2C_CR1_NOSTRETCH;

	//Скорость работы.
	i2c->CR2  &= ~I2C_CR2_FREQ;   		  //
	i2c->CR2  |= (36 << I2C_CR2_FREQ_Pos);//APB1 = 36MHz

	i2c->CCR  &= ~I2C_CCR_CCR;
	i2c->CCR  |=  I2C_CCR_FS; //1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz).
	//i2c->CCR   =  120;//100кГц
	i2c->CCR   =  30; //400кГц  45;//I2C_CCR_VALUE;  //(36MHz/I2C_BAUD_RATE/2)
	i2c->TRISE =  12;//37;//I2C_TRISE_VALUE;//(1mcs/(1/36MHz)+1)

	//i2c->CR1  |=  I2C_CR1_PE; //Включение модуля I2C1.
	//for(uint8_t i = 0; i < 255; i++){__NOP();};
}
//**********************************************************



//*******************************************************************************************
//*******************************************************************************************
void I2C_Init(I2C_TypeDef *i2c, uint32_t remap){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
	//------------------------------
	//Тактирование I2C_1
	if(i2c == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		//Ремап SCL/PB8, SDA/PB9)
		if(remap)
		{
			AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
			GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1 |
						  GPIO_CRH_CNF8    | GPIO_CRH_CNF9;
		}
		else
		{
			//I2C1_SCL - PB6, I2C1_SDA - PB7
			GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 |
						  GPIO_CRL_CNF6    | GPIO_CRL_CNF7;

		}
	}
	//------------------------------
	//Тактирование I2C_2
	else if(i2c == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		//Инициализация портов.
		//I2C2_SCL - PB10
		//I2C2_SDA - PB11
		GPIOB->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 |
					  GPIO_CRH_CNF10    | GPIO_CRH_CNF11;
	}
	else return;
	//------------------------------
	//Инициализация I2C.
	i2c->CR2  &= ~I2C_CR2_FREQ;   		  //
	i2c->CR2  |= (36 << I2C_CR2_FREQ_Pos);//APB1 = 36MHz

	//Скорость работы.
	i2c->CCR  &= ~I2C_CCR_CCR;    //
	//i2c->CCR   =  120;//100кГц
	i2c->CCR   =  30; //400кГц  45;//I2C_CCR_VALUE;  //(36MHz/I2C_BAUD_RATE/2)

	i2c->CCR  |=  I2C_CCR_FS; //1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz).
	i2c->TRISE =  12;//37;//I2C_TRISE_VALUE;//(1mcs/(1/36MHz)+1)
	i2c->CR1  |=  I2C_CR1_PE; //Включение модуля I2C1.
	for(uint8_t i = 0; i < 255; i++){__NOP();};
}
//**********************************************************
uint8_t I2C_StartAndSendDeviceAddr(I2C_TypeDef *i2c, uint8_t deviceAddr){

	uint32_t wait = 0;
	//---------------------
	//Ждем освобождения шины. - не отлажено!!!
	while(i2c->SR2 & I2C_SR2_BUSY)
		{
			if(++wait >= I2C_WAIT)
			{
				i2c->CR1 |=	I2C_CR1_SWRST;
				return 1;
			}
		}
	wait = 0;
	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	while(!(i2c->SR1 & I2C_SR1_SB))//Ожидание формирования Start condition.
		{
			if(++wait >= I2C_WAIT)
				{
					i2c->CR1 |=	I2C_CR1_SWRST;
					return 1;
				}
		}
	wait = 0;
	(void)i2c->SR1;				    //Для сброса флага SB необходимо прочитать SR1
	//Передаем адрес slave.
	i2c->DR = deviceAddr;
	while(!(i2c->SR1 & I2C_SR1_ADDR))//Ожидаем окончания передачи адреса и
		{
			if(++wait >= I2C_WAIT)
				{
					i2c->CR1 |=	I2C_CR1_SWRST;
					return 1;
				}
		}
	wait = 0;
	(void)i2c->SR1;				      //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				      //

	return 0;
}
//**********************************************************
void I2C_SendData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len){

	for(uint16_t i = 0; i < len; i++)
		{
			i2c->DR = *(pBuf + i);
			while(!(i2c->SR1 & I2C_SR1_TXE));//Ждем освобождения буфера
		}
}
//**********************************************************
void I2C_ReadData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len){

	if(len == 1)
		{
			i2c->CR1 &= ~I2C_CR1_ACK;           //Фомирование NACK после приема последнего байта.
			while(!(i2c->SR1 & I2C_SR1_RXNE)){};//ожидаем окончания приема байта
			*(pBuf + 0) = i2c->DR;				//считали принятый байт.
			i2c->CR1 |= I2C_CR1_STOP;           //Формируем Stop
		}
	else
		{
			for(uint16_t i = 0; i < (len-1); i++)
				{
					i2c->CR1 |= I2C_CR1_ACK; 			//Фомирование ACK после приема байта
					while(!(i2c->SR1 & I2C_SR1_RXNE)){};//ожидаем окончания приема байта
					*(pBuf + i) = i2c->DR;			    //считали принятый
				}

			i2c->CR1 &= ~I2C_CR1_ACK;           //Фомирование NACK после приема последнего байта.
			while(!(i2c->SR1 & I2C_SR1_RXNE)){};//ожидаем окончания приема байта
			*(pBuf + len - 1) = i2c->DR;		//считали принятый байт.
			i2c->CR1 |= I2C_CR1_STOP;           //Формируем Stop
		}
}
//**********************************************************
void I2C_Stop(I2C_TypeDef *i2c){

	while(!(i2c->SR1 & I2C_SR1_BTF));//Ждем окончания передачи
	i2c->CR1 |= I2C_CR1_STOP;		 //Формируем Stop
}
//*******************************************************************************************
//*******************************************************************************************
void I2C_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

//	uint32_t wait_count = 0;
	//---------------------
	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	if(I2C_LongWait(i2c, I2C_SR1_SB)) return;//Ожидание формирования Start condition.
	(void)i2c->SR1;				      		 //Для сброса флага SB необходимо прочитать SR1

	//Передаем адрес slave + Запись.
	i2c->DR = deviceAddr | I2C_MODE_WRITE;
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;				        	   //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				        	   //

	//Передача адреса в который хотим записать.
	i2c->DR = regAddr;
	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;
	//передача данных на запись.
	for(uint16_t i = 0; i < len; i++)
		{
			i2c->DR = *(pBuf + i);
			if(I2C_LongWait(i2c, I2C_SR1_TXE)) goto STOP;//Ждем освобождения буфера
		}
	STOP:
	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//**********************************************************
void I2C_Read(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	if(I2C_LongWait(i2c, I2C_SR1_SB)) return;//Ожидание формирования Start condition.
	(void)i2c->SR1;							 //Для сброса флага SB необходимо прочитать SR1

	//Передаем адрес slave + Запись.
	i2c->DR = deviceAddr | I2C_MODE_WRITE;
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;							   //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;							   //

	//Передача адреса с которого начинаем чтение.
	i2c->DR = regAddr;
	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;
	//---------------------
	//Формирование reStart condition.
	i2c->CR1 |= I2C_CR1_STOP; //Это команда нужня для работы с DS2782. Без нее не работает
	i2c->CR1 |= I2C_CR1_START;
	if(I2C_LongWait(i2c, I2C_SR1_SB)) return;//Ожидание формирования Start condition.
	(void)i2c->SR1;							 //Для сброса флага SB необходимо прочитать SR1

	//Передаем адрес slave + Чтение.
	i2c->DR = deviceAddr | I2C_MODE_READ;
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;							   //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;							   //
	//прием даннных
	if(len == 1)
		{
			i2c->CR1 &= ~I2C_CR1_ACK;                	  //Фомирование NACK после приема последнего байта.
			if(I2C_LongWait(i2c, I2C_SR1_RXNE)) goto STOP;//ожидаем окончания приема байта
			*(pBuf + 0) = i2c->DR;				          //считали принятый байт.
		}
	else
		{
			for(uint16_t i = 0; i < (len-1); i++)
				{
					i2c->CR1 |= I2C_CR1_ACK; 			          //Фомирование ACK после приема байта
					if(I2C_LongWait(i2c, I2C_SR1_RXNE)) goto STOP;//ожидаем окончания приема байта
					*(pBuf + i) = i2c->DR;			         	  //считали принятый
				}
			i2c->CR1 &= ~I2C_CR1_ACK;                	  //Фомирование NACK после приема последнего байта.
			if(I2C_LongWait(i2c, I2C_SR1_RXNE)) goto STOP;//ожидаем окончания приема байта
			*(pBuf + len - 1) = i2c->DR;		     	  //считали принятый байт.
		}
	//---------------------
	STOP:
	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//*******************************************************************************************
//*******************************************************************************************
//****************************Работа I2C по прерываниям.*************************************
static uint32_t I2cMode	   = 0;
static uint32_t I2cItState = 0;

static uint8_t SlaveAddr    = 0;
static uint8_t SlaveRegAddr = 0;

static uint8_t *ptrTxBuf  	= 0;
static uint32_t TxBufSize 	= 0;

static uint8_t *ptrRxBuf  	= 0;
static uint32_t RxBufSize 	= 0;

//**********************************************************
void I2C_IT_Init(I2C_TypeDef *i2c, uint32_t remap){

	I2C_Init(i2c, remap);

	//Инит-я прерывания.
	i2c->CR2 |= I2C_CR2_ITEVTEN | //Разрешение прерывания по событию.
				I2C_CR2_ITERREN;  //Разрешение прерывания по ошибкам.

	NVIC_SetPriority(I2C1_EV_IRQn, 15);//Приоритет прерывания.
	NVIC_SetPriority(I2C1_ER_IRQn, 15);//Приоритет прерывания.

	NVIC_EnableIRQ(I2C1_EV_IRQn);      //Разрешаем прерывание.
	NVIC_EnableIRQ(I2C1_ER_IRQn);      //Разрешаем прерывание.
}
//**********************************************************
void I2C_IT_StartTx(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pTxBuf, uint32_t len){

	SlaveAddr    = deviceAddr;
	SlaveRegAddr = regAddr;
	ptrTxBuf     = pTxBuf;
	TxBufSize    = len;

	i2c->CR1  |= I2C_CR1_START;
	I2cItState = 1;
}
//**********************************************************
void I2C_IT_StartRx(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pRxBuf, uint32_t len){

	SlaveAddr    = deviceAddr;
	SlaveRegAddr = regAddr;
	ptrRxBuf     = pRxBuf;
	RxBufSize    = len;

	i2c->CR1  |= I2C_CR1_START;
	I2cItState = 1;
}
//**********************************************************
uint8_t I2C_IT_GetState(void){

	return I2cItState;
}
//*******************************************************************************************
//*******************************************************************************************
void I2C_IT_Slave_Init(I2C_TypeDef *i2c, uint32_t slaveAddr, uint32_t remap){

	I2C_Slave_Init(i2c, slaveAddr, remap);

	I2cMode = I2C_MODE_SLAVE;

	//Инит-я прерывания.
	i2c->CR2 |= I2C_CR2_ITEVTEN | //Разрешение прерывания по событию.
				I2C_CR2_ITERREN;  //Разрешение прерывания по ошибкам.

	NVIC_SetPriority(I2C1_EV_IRQn, 15);//Приоритет прерывания.
	NVIC_SetPriority(I2C1_ER_IRQn, 15);//Приоритет прерывания.

	NVIC_EnableIRQ(I2C1_EV_IRQn);      //Разрешаем прерывание.
	NVIC_EnableIRQ(I2C1_ER_IRQn);      //Разрешаем прерывание.
}
//**********************************************************
void I2C_IT_Slave_StartRx(uint8_t *pRxBuf, uint32_t len){

	ptrRxBuf  = pRxBuf;
	RxBufSize = len;
}
//**********************************************************
void I2C_IT_Slave_StartTx(uint8_t *pTxBuf, uint32_t len){

	ptrTxBuf  = pTxBuf;
	TxBufSize = len;
}
//*******************************************************************************************
//*******************************************************************************************
//Обработчик прерывания событий I2C
void I2C1_EV_IRQHandler(void){

	static uint32_t txCount = 0;
	static uint32_t rxCount = 0;
	//LedPC13Toggel();
	//------------------------------
	//Сформирована START последоательность.
	if(I2C1->SR1 & I2C_SR1_SB)
	{
		(void)I2C1->SR1;				      //Для сброса флага SB необходимо прочитать SR1
		I2C1->DR = SlaveAddr | I2C_MODE_WRITE;//Передаем адрес slave + Запись.
		return;
	}
	//------------------------------
	//Окончание приема/передачи адреса.
	if(I2C1->SR1 & I2C_SR1_ADDR)
	{
		(void)I2C1->SR1; //сбрасываем бит ADDR (чтением SR1 и SR2):
		(void)I2C1->SR2;
		//Slave
		if(I2cMode == I2C_MODE_SLAVE)
		{
			//От Мастера пришла команда на запись.
			if((I2C1->SR2 & I2C_SR2_TRA) == 0)
			{
				*(ptrRxBuf + 0) = I2C1->DR;//Сохранили первый байт
				rxCount++;
			}
			//От Мастера пришла команда на чтение.
			else
			{
				I2C1->DR = *(ptrTxBuf + 0);
				txCount  = 1;
			}
		}
		//Master
		else
		{
			I2C1->DR   = SlaveRegAddr;   //Передача адреса в который хотим записать.
			I2C1->CR2 |= I2C_CR2_ITBUFEN;//Разрешение прерывания по опустощению буфера Tx.
		}
		return;
	}
	//------------------------------
	//Передающий буфер свободен.
	if(I2C1->SR1 & I2C_SR1_TXE)
	{
		(void)I2C1->SR1; //сбрасываем бит TXE (чтением SR1 и SR2):
		(void)I2C1->SR2;

		//Slave
		if(I2cMode == I2C_MODE_SLAVE)
		{
			I2C1->DR = *(ptrTxBuf + txCount);
			txCount++;
			if(txCount >= TxBufSize)txCount = 0;
		}
		//Master
		else
		{
			if(txCount < TxBufSize)
			{
				I2C1->DR = *(ptrTxBuf + txCount);
				txCount++;
			}
			else
			{
				I2C1->CR2 &= ~I2C_CR2_ITBUFEN;//Запрет прерывания по опостощению буфера Tx.
				I2C1->CR1 |= I2C_CR1_STOP;    //Формируем Stop
				I2cItState = 0;
				txCount    = 0;
			}
		}
		return;
	}
	//------------------------------
	//Принят байт.
	if(I2C1->SR1 & I2C_SR1_RXNE)
	{
		if(rxCount < RxBufSize)
		{
			*(ptrRxBuf + rxCount) = I2C1->DR;
			rxCount++;
		}
		else
		{
			rxCount = 0;
		}

		return;
	}
	//------------------------------
	//STOP
	if(I2C1->SR1 & I2C_SR1_STOPF)
	{
		(void)I2C1->SR1; 		   //сбрасываем бит STOPF
		I2C1->CR1 &= ~I2C_CR1_STOP;//
	}
}
//**********************************************************
//Обработчик прерывания ошибок I2C
void I2C1_ER_IRQHandler(void){

	LedPC13Toggel();
	//------------------------------
	//NACK - Acknowledge failure
	if(I2C1->SR1 & I2C_SR1_AF)
	{
		I2C1->SR1 &= ~I2C_SR1_AF; //Сброс AF.
		//I2C1->CR1 |= I2C_CR1_STOP;//Формируем Stop
		//return;
	}
	//------------------------------
	//Bus error
	if(I2C1->SR1 & I2C_SR1_BERR)
	{
		I2C1->SR1 &= ~I2C_SR1_BERR; //Сброс BERR.
	}
	//------------------------------
	//Arbitration loss (Master)
	if(I2C1->SR1 & I2C_SR1_ARLO)
	{
		I2C1->SR1 &= ~I2C_SR1_ARLO; //Сброс ARLO.
	}
	//------------------------------
	//Overrun/Underrun
	if(I2C1->SR1 & I2C_SR1_OVR)
	{
		I2C1->SR1 &= ~I2C_SR1_OVR; //Сброс OVR.
	}
	//------------------------------
	//PEC error
	if(I2C1->SR1 & I2C_SR1_PECERR)
	{
		I2C1->SR1 &= ~I2C_SR1_PECERR; //Сброс PECERR.
	}
	//------------------------------
	//Timeout/Tlow error
	if(I2C1->SR1 & I2C_SR1_TIMEOUT)
	{
		I2C1->SR1 &= ~I2C_SR1_TIMEOUT; //Сброс TIMEOUT.
	}
	//------------------------------
	//SMBus Alert
	if(I2C1->SR1 & I2C_SR1_SMBALERT)
	{
		I2C1->SR1 &= ~I2C_SR1_SMBALERT; //Сброс SMBALERT.
	}
	//------------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void I2C1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
	//------------------------------
	//Тактирование I2C_1
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	//Инициализация портов.
	//I2C1_SCL - PB6
	//I2C1_SDA - PB7
	GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 |
				  GPIO_CRL_CNF6    | GPIO_CRL_CNF7;
	//------------------------------
	//Инициализация I2C.
	I2C1->CR2  &= ~I2C_CR2_FREQ;   //
	I2C1->CR2  |=  36;//I2C_CR2_VALUE;  //APB1 = 36MHz
	I2C1->CCR  &= ~I2C_CCR_CCR;    //

	//i2c->CCR   =  120;//100кГц
	I2C1->CCR   =  30; //400кГц  45;//I2C_CCR_VALUE;  //(36MHz/I2C_BAUD_RATE/2)

	I2C1->CCR  |=  I2C_CCR_FS;     //1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz).
	I2C1->TRISE =  12;//37;//I2C_TRISE_VALUE;//(1mcs/(1/36MHz)+1)

	I2C1->CR2 |= I2C_CR2_DMAEN; //DMAEN(DMA requests enable) — единица в этом бите разрешает делать запрос к DMA
								//при установке флагов TxE или RxNE.

	I2C1->CR1 |= I2C_CR1_PE;    //Включение модуля I2C1.

	for(uint8_t i = 0; i < 255; i++){__NOP();};
}
//**********************************************************
void I2C1_DMAInit(void){

	/* Enable the peripheral clock DMA1 */
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	/* DMA1 Channel6 I2C1_TX config */
	DMA1_Channel6->CPAR  = (uint32_t)&(I2C1->DR);  // Peripheral address.
	//DMA1_Channel6->CMAR  = (uint32_t)pData;      // Memory address.
	//DMA1_Channel6->CNDTR = size; 		   		   // Data size.

	DMA1_Channel6->CCR = (3 << DMA_CCR_PL_Pos)   | // PL[1:0]: Channel priority level - 11: Very high.
						 (0 << DMA_CCR_PSIZE_Pos)| // PSIZE[1:0]: Peripheral size - 00: 8-bits.
						 (0 << DMA_CCR_MSIZE_Pos)| // MSIZE[1:0]: Memory size     - 00: 8-bits.
						 DMA_CCR_MINC |			   // MINC: Memory increment mode - Memory increment mode enabled.
						 DMA_CCR_DIR  |            // DIR: Data transfer direction - 1: Read from memory.
						 //DMA_CCR_CIRC | 		   // CIRC: Circular mode
						 //DMA_CCR_TEIE | 		   // TEIE: Transfer error interrupt enable
						 //DMA_CCR_HTIE | 		   // HTIE: Half transfer interrupt enable
						 DMA_CCR_TCIE;// | 		   // TCIE: Transfer complete interrupt enable
						 //DMA_CCR_EN;			   // EN: Channel enable

	//NVIC_SetPriority(DMA1_Channel6_IRQn, 0);// Set priority for DMA1_Channel2_3_IRQn */
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);       // Enable DMA1_Channel2_3_IRQn */
}
//**********************************************************
void I2C1_DMAStartTx(uint8_t *pData, uint32_t size){

	DMA1_Channel6->CCR  &= ~DMA_CCR_EN;	   // Channel disable
	DMA1_Channel6->CMAR  = (uint32_t)pData;// Memory address.
	DMA1_Channel6->CNDTR = size; 		   // Data size.
	DMA1_Channel6->CCR  |= DMA_CCR_EN;     // Channel enable
}
//**********************************************************
void I2C1_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t size){

	if(DMA1_Channel6->CCR & DMA_CCR_EN)return;

	//I2C1->CR2 &= ~I2C_CR2_LAST;//запрет DMA генерировать сигнал окончания передачи EOT(End of Transfer).

	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	while(!(i2c->SR1 & I2C_SR1_SB)){};//Ожидание формирования Start condition.
	(void)i2c->SR1;				      //Для сброса флага SB необходимо прочитать SR1
	//Передаем адрес slave + Запись.
	i2c->DR = deviceAddr | I2C_MODE_WRITE;
	while(!(i2c->SR1 & I2C_SR1_ADDR)){};//Ожидаем окончания передачи адреса и
	(void)i2c->SR1;				        //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				        //
	//Передача адреса в который хотим записать.
	i2c->DR = regAddr;
	while(!(i2c->SR1 & I2C_SR1_TXE)){};
	//передача данных на запись.
	I2C1_DMAStartTx(pBuf, size);

//	for(uint16_t i = 0; i < len; i++)
//		{
//			i2c->DR = *(pBuf + i);
//			//Ждем освобождения буфера
//			if(I2C_LongWaitTransmitters(i2c)) goto STOP;
//		}
//	STOP:
//	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//**********************************************************
void DMA1_Channel6_IRQHandler(void){

	//-------------------------
	//Обмен завершен.
	if(DMA1->ISR & DMA_ISR_TCIF6)
	{
		DMA1->IFCR |= DMA_IFCR_CTCIF6;    //сбросить флаг окончания обмена.
		DMA1_Channel6->CCR &= ~DMA_CCR_EN;//запретить работу канала.

		//I2C1->CR2 |= I2C_CR2_LAST;//LAST(DMA last transfer) — разрешаем DMA генерировать сигнал окончания передачи EOT(End of Transfer).
	}
	//-------------------------
	LedPC13Toggel();
}
//*******************************************************************************************
//*******************************************************************************************




