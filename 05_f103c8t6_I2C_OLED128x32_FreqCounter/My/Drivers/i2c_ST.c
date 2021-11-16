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
static uint32_t I2C_LongWaitTransmitters(I2C_TypeDef *i2c){

	uint32_t wait_count = 0;
	//---------------------
	//Ждем освобождения передающего буфера I2C
	while(!(i2c->SR1 & I2C_SR1_TXE))
	{
		if(++wait_count >= I2C_WAIT) return 1;
	}
	return 0;
}
//**********************************************************
static uint32_t I2C_LongWaitReceivers(I2C_TypeDef *i2c){

	uint32_t wait_count = 0;
	//---------------------
	while(!(i2c->SR1 & I2C_SR1_RXNE))//{};//ожидаем окончания приема байта
	{
		if(++wait_count >= I2C_WAIT) return 1;
	}
	return 0;
}
//*******************************************************************************************
//*******************************************************************************************
void I2C_Init(I2C_TypeDef *i2c){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
	//------------------------------
	//Тактирование I2C_1
	if(i2c == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		//Инициализация портов.
		//I2C1_SCL - PB6
		//I2C1_SDA - PB7
		GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 |
					  GPIO_CRL_CNF6    | GPIO_CRL_CNF7;
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
void I2C_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

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
	for(uint16_t i = 0; i < len; i++)
		{
			i2c->DR = *(pBuf + i);
			//Ждем освобождения буфера
			if(I2C_LongWaitTransmitters(i2c)) goto STOP;
		}
	STOP:
	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//**********************************************************
void I2C_Read(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	while(!(i2c->SR1 & I2C_SR1_SB)){};//Ожидание формирования Start condition.
	(void)i2c->SR1;				      //Для сброса флага SB необходимо прочитать SR1
	//Передаем адрес slave + Запись.
	i2c->DR = deviceAddr | I2C_MODE_WRITE;
	while(!(i2c->SR1 & I2C_SR1_ADDR)){};//Ожидаем окончания передачи адреса и
	(void)i2c->SR1;				        //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				        //
	//Передача адреса с которого начинаем чтение.
	i2c->DR = regAddr;
	//while(!(i2c->SR1 & I2C_SR1_TXE));
	if(I2C_LongWaitTransmitters(i2c)) return;
	//---------------------
	//Формирование reStart condition.
	i2c->CR1 |= I2C_CR1_STOP;		//Это команда нужня для работы с DS2782. Без нее не работает
	i2c->CR1 |= I2C_CR1_START;

	while(!(i2c->SR1 & I2C_SR1_SB)){};//Ожидание формирования Start condition.
	(void)i2c->SR1;				      //Для сброса флага SB необходимо прочитать SR1
	//Передаем адрес slave + Чтение.
	i2c->DR = deviceAddr | I2C_MODE_READ;
	while(!(i2c->SR1 & I2C_SR1_ADDR)){};//Ожидаем окончания передачи адреса и
	(void)i2c->SR1;				        //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				        //
	//прием даннных
	if(len == 1)
		{
			i2c->CR1 &= ~I2C_CR1_ACK;           //Фомирование NACK после приема последнего байта.
			//while(!(i2c->SR1 & I2C_SR1_RXNE)){};//ожидаем окончания приема байта
			if(I2C_LongWaitReceivers(i2c)) goto STOP;//ожидаем окончания приема байта
			*(pBuf + 0) = i2c->DR;				//считали принятый байт.
			//i2c->CR1 |= I2C_CR1_STOP;           //Формируем Stop
		}
	else
		{
			for(uint16_t i = 0; i < (len-1); i++)
				{
					i2c->CR1 |= I2C_CR1_ACK; 			//Фомирование ACK после приема байта
					//while(!(i2c->SR1 & I2C_SR1_RXNE)){};//ожидаем окончания приема байта
					if(I2C_LongWaitReceivers(i2c)) goto STOP;//ожидаем окончания приема байта
					*(pBuf + i) = i2c->DR;			    //считали принятый
				}
			i2c->CR1 &= ~I2C_CR1_ACK;           //Фомирование NACK после приема последнего байта.
			//while(!(i2c->SR1 & I2C_SR1_RXNE)){};//ожидаем окончания приема байта
			if(I2C_LongWaitReceivers(i2c)) goto STOP;//ожидаем окончания приема байта
			*(pBuf + len - 1) = i2c->DR;		//считали принятый байт.
			//i2c->CR1 |= I2C_CR1_STOP;           //Формируем Stop
		}
	//---------------------
	STOP:
	i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
}
//*******************************************************************************************
//*******************************************************************************************





