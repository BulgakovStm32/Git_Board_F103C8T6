/*
 * i2c_ST.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */
//*******************************************************************************************
//*******************************************************************************************

#include "i2c_ST.h"

//*******************************************************************************************
//*******************************************************************************************
//static uint8_t   I2C_SlaveAddr;
//static uint8_t  *I2C_pBuf;
//static uint16_t  I2C_TxCnt;
//static uint8_t   I2C_TxSize;
//static uint8_t   I2C_Mode;
//*******************************************************************************************
//*******************************************************************************************
//static uint32_t I2C_LongWaitTransmitters(I2C_TypeDef *i2c){
//
//	uint32_t wait_count = 0;
//	//---------------------
//	//Ждем освобождения передающего буфера I2C
//	while(!(i2c->SR1 & I2C_SR1_TXE))
//	{
//		if(++wait_count >= I2C_WAIT)
//		{
////			i2c->SR1 |= I2C_CR1_SWRST;
////			I2C_Init(i2c);
//			return 1;
//		}
//	}
//	return 0;
//}
//**********************************************************
//static uint32_t I2C_LongWaitReceivers(I2C_TypeDef *i2c){
//
//	uint32_t wait_count = 0;
//	//---------------------
//	while(!(i2c->SR1 & I2C_SR1_RXNE))//{};//ожидаем окончания приема байта
//	{
//		if(++wait_count >= I2C_WAIT)
//		{
////			i2c->SR1 |= I2C_CR1_SWRST;
////			I2C_Init(i2c);
//			return 1;
//		}
//	}
//	return 0;
//}
//**********************************************************
//static uint32_t I2C_LongWaitStartCondition(I2C_TypeDef *i2c){
//
//	uint32_t wait_count = 0;
//	//---------------------
//	while(!(i2c->SR1 & I2C_SR1_SB))//Ожидание формирования Start condition.
//	{
//		if(++wait_count >= I2C_WAIT)
//		{
////			i2c->SR1 |= I2C_CR1_SWRST;
////			I2C_Init(i2c);
//			return 1;
//		}
//	}
//	return 0;
//}
//**********************************************************
//static uint32_t I2C_LongWaitAddressSend(I2C_TypeDef *i2c){
//
//	uint32_t wait_count = 0;
//	//---------------------
//	while(!(i2c->SR1 & I2C_SR1_ADDR))//Ожидаем окончания передачи адреса
//	{
//		if(++wait_count >= I2C_WAIT)
//		{
////			i2c->SR1 |= I2C_CR1_SWRST;
////			I2C_Init(i2c);
//			return 1;
//		}
//	}
//	return 0;
//}
//**********************************************************
static uint32_t I2C_LongWait(I2C_TypeDef *i2c, uint32_t flag){

	uint32_t wait_count = 0;
	//---------------------
	while(!(i2c->SR1 & flag))//Ожидаем окончания ....
	{
		if(++wait_count >= I2C_WAIT) return 1;
	}
	return 0;
}
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
	if(i2c == I2C2)
		{
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
			//Инициализация портов.
			//I2C2_SCL - PB10
			//I2C2_SDA - PB11
			GPIOB->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 |
						  GPIO_CRH_CNF10    | GPIO_CRH_CNF11;
		}
	//------------------------------
	//Инициализация I2C.
	i2c->CR2  &= ~I2C_CR2_FREQ;   //
	i2c->CR2  |=  36;//I2C_CR2_VALUE;  //APB1 = 36MHz
	i2c->CCR  &= ~I2C_CCR_CCR;    //

	//i2c->CCR   =  120;//100кГц
	i2c->CCR   =  30; //400кГц  45;//I2C_CCR_VALUE;  //(36MHz/I2C_BAUD_RATE/2)

	i2c->CCR  |=  I2C_CCR_FS;     //1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz).
	i2c->TRISE =  12;//37;//I2C_TRISE_VALUE;//(1mcs/(1/36MHz)+1)
	i2c->CR1  |=  I2C_CR1_PE;     //Включение модуля I2C1.
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
	(void)i2c->SR1;				      //Для сброса флага SB необходимо прочитать SR1
	//Передаем адрес slave + Запись.
	i2c->DR = deviceAddr | I2C_MODE_WRITE;
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;				        //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				        //
	//Передача адреса в который хотим записать.
	i2c->DR = regAddr;
	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;
	//передача данных на запись.
	for(uint16_t i = 0; i < len; i++)
		{
			i2c->DR = *(pBuf + i);
			//Ждем освобождения буфера
			if(I2C_LongWait(i2c, I2C_SR1_TXE)) goto STOP;
		}
	STOP:
	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//**********************************************************
void I2C_Read(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	//if(I2C_LongWaitStartCondition(i2c)) return;//Ожидание формирования Start condition.

	(void)i2c->SR1;//Для сброса флага SB необходимо прочитать SR1

	//Передаем адрес slave + Запись.
	i2c->DR = deviceAddr | I2C_MODE_WRITE;
	//if(I2C_LongWaitAddressSend(i2c)) return;//Ожидаем окончания передачи адреса
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;//сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;//

	//Передача адреса с которого начинаем чтение.
	i2c->DR = regAddr;
	//if(I2C_LongWaitTransmitters(i2c)) return;
	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;
	//---------------------
	//Формирование reStart condition.
	i2c->CR1 |= I2C_CR1_STOP; //Это команда нужня для работы с DS2782. Без нее не работает
	i2c->CR1 |= I2C_CR1_START;
	//if(I2C_LongWaitStartCondition(i2c)) return;//Ожидание формирования Start condition.
	if(I2C_LongWait(i2c, I2C_SR1_SB)) return;//Ожидание формирования Start condition.
	(void)i2c->SR1;//Для сброса флага SB необходимо прочитать SR1

	//Передаем адрес slave + Чтение.
	i2c->DR = deviceAddr | I2C_MODE_READ;
	//if(I2C_LongWaitAddressSend(i2c)) return;//Ожидаем окончания передачи адреса
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;//сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;//
	//прием даннных
	if(len == 1)
		{
			i2c->CR1 &= ~I2C_CR1_ACK;                //Фомирование NACK после приема последнего байта.
			//if(I2C_LongWaitReceivers(i2c)) goto STOP;//ожидаем окончания приема байта
			if(I2C_LongWait(i2c, I2C_SR1_RXNE)) goto STOP;//ожидаем окончания приема байта
			*(pBuf + 0) = i2c->DR;				     //считали принятый байт.
		}
	else
		{
			for(uint16_t i = 0; i < (len-1); i++)
				{
					i2c->CR1 |= I2C_CR1_ACK; 			     //Фомирование ACK после приема байта
					//if(I2C_LongWaitReceivers(i2c)) goto STOP;//ожидаем окончания приема байта
					if(I2C_LongWait(i2c, I2C_SR1_RXNE)) goto STOP;//ожидаем окончания приема байта
					*(pBuf + i) = i2c->DR;			         //считали принятый
				}
			i2c->CR1 &= ~I2C_CR1_ACK;                //Фомирование NACK после приема последнего байта.
			//if(I2C_LongWaitReceivers(i2c)) goto STOP;//ожидаем окончания приема байта
			if(I2C_LongWait(i2c, I2C_SR1_RXNE)) goto STOP;//ожидаем окончания приема байта
			*(pBuf + len - 1) = i2c->DR;		     //считали принятый байт.
		}
	//---------------------
	STOP:
	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//*******************************************************************************************
//*******************************************************************************************
uint32_t I2C_Write2(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

//	//---------------------
//	//Формирование Start condition.
//	i2c->CR1 |= I2C_CR1_START;
//	while(!(i2c->SR1 & I2C_SR1_SB)){};//Ожидание формирования Start condition.
//	(void)i2c->SR1;				      //Для сброса флага SB необходимо прочитать SR1
//
//	//Передаем адрес slave + Запись.
//	i2c->DR = deviceAddr | I2C_MODE_WRITE;
//	while(!(i2c->SR1 & I2C_SR1_ADDR)){};//Ожидаем окончания передачи адреса и
//	(void)i2c->SR1;				        //сбрасываем бит ADDR (чтением SR1 и SR2):
//	(void)i2c->SR2;				        //
//
//	//Передача адреса в который хотим записать.
//	i2c->DR = regAddr;
//	while(!(i2c->SR1 & I2C_SR1_TXE)){};
//
//	//передача данных на запись.
//	for(uint16_t i = 0; i < len; i++)
//		{
//			i2c->DR = *(pBuf + i);
//			//Ждем освобождения буфера
//			if(I2C_LongWaitTransmitters(i2c)) goto STOP;
//		}
//	STOP:
//	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
//
//	return 0;
	//---------------------

	static uint32_t i2c_state = 0;
	static uint32_t index = 0;

	switch(i2c_state)
	{
		//---------------------
		//Формирование Start condition.
		case(0):
			i2c->CR1 |= I2C_CR1_START;
			i2c_state = 1;
		break;
		//---------------------
		//Ожидание формирования Start condition.
		case(1):
			if(!(i2c->SR1 & I2C_SR1_SB)) break;//Ожидание формирования Start condition.
			(void)i2c->SR1;				        //Для сброса флага SB необходимо прочитать SR1
			i2c_state = 2;
		break;
		//---------------------
		//Передаем адрес slave + Запись.
		case(2):
			i2c->DR = deviceAddr | I2C_MODE_WRITE;
			i2c_state = 3;
		break;
		//---------------------
		//Ожидаем окончания передачи адреса
		case(3):
			if(!(i2c->SR1 & I2C_SR1_ADDR))break;//Ожидаем окончания передачи адреса и
			(void)i2c->SR1;				         //сбрасываем бит ADDR (чтением SR1 и SR2):
			(void)i2c->SR2;				         //
			i2c_state = 4;
		break;
		//---------------------
		//Передача адреса в который хотим записать.
		case(4):
			i2c->DR = regAddr;
			i2c_state = 5;
		break;
		//---------------------
		//Ожидаем окончания передачи адреса в который хотим записать.
		case(5):
			if(!(i2c->SR1 & I2C_SR1_TXE))break;
			i2c_state = 6;
		break;
		//---------------------
		//передача данных на запись.
		case(6):
			i2c->DR = *(pBuf + index);//передаем байт
			index++;
			i2c_state = 7;
		break;
		//---------------------
		//Проверка окончания передачи байта
		case(7):
			if(!(i2c->SR1 & I2C_SR1_TXE))break;

			if(index <= len) i2c_state = 6;//Продолжение передачи буфера.
			else             i2c_state = 8;//Окончили передачу буфера.

		break;
		//---------------------
		//Формируем Stop
		case(8):
			i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
			index     = 0;
			i2c_state = 0;
		break;
		//---------------------
	}
	return i2c_state;
}
//*******************************************************************************************
//*******************************************************************************************
//Передача данных в i2c через dma.


//**********************************************************
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




