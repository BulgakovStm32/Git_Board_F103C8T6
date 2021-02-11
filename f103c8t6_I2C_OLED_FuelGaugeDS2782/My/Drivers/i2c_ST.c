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
static uint16_t I2C_TxCnt;
//static uint16_t RxCounter;

static uint8_t  I2C_SlaveAddr;
//static uint8_t  I2C_Buf[16];

static uint8_t  *I2C_pBuf;
static uint8_t   I2C_TxSize;
//static uint8_t   I2C_RxSize;
static uint8_t   I2C_Mode;

//static uint8_t  i2cState = I2C_STATE_IDLE;
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
			while(!(i2c->SR1 & I2C_SR1_TXE));//Ждем освобождения буфера
		}

//	while(!(i2c->SR1 & I2C_SR1_BTF));//Ждем окончания передачи
	i2c->CR1 |= I2C_CR1_STOP;		 //Формируем Stop
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
	while(!(i2c->SR1 & I2C_SR1_TXE));
//	i2c->CR1 |= I2C_CR1_STOP;	//??????

	//Формирование reStart condition.
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
//*******************************************************************************************
//*******************************************************************************************
void I2C_Int_Init(I2C_TypeDef *i2c){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
	//Тактирование I2C_1
	if(i2c == I2C1)
		{
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
			//Инициализация портов.
			GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 |
					      GPIO_CRL_CNF6    | GPIO_CRL_CNF7;
		}
	//Тактирование I2C_2
	if(i2c == I2C2)
		{
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
			//Инициализация портов.
			GPIOB->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 |
						  GPIO_CRH_CNF10    | GPIO_CRH_CNF11;
		}
//	//Инициализация I2C.
//	i2c->CR2  &= ~I2C_CR2_FREQ;   //
//	i2c->CR2  |=  I2C_CR2_VALUE;  //APB1 = 36MHz
//	i2c->CCR  &= ~I2C_CCR_CCR;    //
//	i2c->CCR   =  I2C_CCR_VALUE;  //(36MHz/I2C_BAUD_RATE/2)
////	i2c->CCR  |=  I2C_CCR_FS;     //1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz).
//	i2c->TRISE =  I2C_TRISE_VALUE;//(1mcs/(1/36MHz)+1)
//	//Активаци прерывания.
//	i2c->CR2 |= I2C_CR2_ITEVTEN;//ITEVTEN(Event interrupt enable) — прерывания по событию.
////	i2c->CR2 |= I2C_CR2_ITERREN;//ITERREN(Error interrupt enable) — прерывания при возникновении ошибок.
//
//	if(i2c == I2C1)
//		{
//			NVIC_EnableIRQ(I2C1_EV_IRQn);
//			NVIC_SetPriority(I2C1_EV_IRQn, 1);
//		}
//	if(i2c == I2C2)
//		{
//			NVIC_EnableIRQ(I2C2_EV_IRQn);
//			NVIC_SetPriority(I2C2_EV_IRQn, 1);
//		}
//	//---------------------
//	i2c->CR1 |= I2C_CR1_PE;     //Включение модуля I2C1.
//	for(uint8_t i = 0; i < 255; i++){__NOP();};
}
//**********************************************************
void I2C_Int_StartSendData(I2C_TypeDef *i2c, uint8_t slaveAddr, uint8_t *pBuf, uint16_t len){

	//---------------------
//	if(i2cState != I2C_STATE_IDLE) return;
//
//	i2cState = I2C_STATE_START;

	I2C_SlaveAddr = slaveAddr;
	I2C_pBuf      = pBuf;
	I2C_TxSize    = len;
	I2C_TxCnt	  = 0;
	I2C_Mode      = I2C_MODE_WRITE;
	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
}
//*******************************************************************************************
//*******************************************************************************************
static void I2C_Int_Handler(I2C_TypeDef *i2c, uint8_t slaveAddr){

	volatile uint32_t sr1 = i2c->SR1;//Чтениех этих регистров сбрасывает флаги событий I2C.
	//volatile uint32_t sr2 = i2c->SR2;
	//---------------------
	//Start condition generated. Clear: read SR1, write to DR
	if(sr1 & I2C_SR1_SB)
		{
		  i2c->DR = I2C_ADDRESS(slaveAddr, I2C_Mode);
		  return;
		}
	//Address sent. Clear: read SR1, read SR2
	else if(sr1 & I2C_SR1_ADDR)
	   {
	      if(I2C_Mode == I2C_MODE_WRITE)
			  {
	    	  	  i2c->DR = I2C_pBuf[0];
	    	  	  I2C_TxCnt++;

				 //I2C_SR2      = I2C_NUMB->SR2;                     // Read SR2, clear ADDR
	    	  	 // i2c->DR = (uint8_t)slaveAddr; // Send first byte
				 //if(SingleByteFlag) I2C_NUMB->CR1 |= I2C_CR1_STOP;         // Write single byte
				 //else if (ByteCount > 1)   I2C_NUMB->CR2 |= I2C_CR2_ITBUFEN;   // Enable TxE for >1 bytes
			  }
//	      if(I2C_Mode == I2C_MODE_READ )
//			  {
//				 if (ByteCount == 1)                        // Read single byte
//					 {
//						I2C_NUMB->CR2 |= I2C_CR2_ITBUFEN;         // Enable RxNE
//						I2C_NUMB->CR1 &=~I2C_CR1_ACK;            // Set ACK low (before clear ADDR, during EV6)
//					 }
//				 if (ByteCount == 2)                        // Read 2-byte
//					 {
//						I2C_NUMB->CR1 &=~I2C_CR1_ACK;            // Set ACK low
//						I2C_NUMB->CR1 |= I2C_CR1_POS;            // Set POS high
//					 }
//				 if (ByteCount > 3)
//					 {
//						I2C_NUMB->CR2 |= I2C_CR2_ITBUFEN;         // Enable RxNE
//					 }
//				 I2C_SR2 = I2C_NUMB->SR2;                  // Read SR2, clear ADDR
//			  }
//	      if(I2C_Mode == I2C_MODE_WRITEREAD)
//			  {
//				 I2C_SR2 = I2C_NUMB->SR2;                     // Read SR2, clear ADDR
//
//				 if(!(--AddrByteCnt))                        // ReStart
//					 {
//						I2C_Mode = I2C_MODE_READ;
//						I2C_NUMB->DR = (uint8_t)AddrReg;            // Send first byte
//						I2C_NUMB->DR = 0xFF;                     // Dummy
//						I2C_NUMB->CR1 |= I2C_CR1_START;               // ReStart after current byte transfer
//					 }
//				 else
//					 {
//						// не дописано! не использовать
//						I2C_NUMB->DR = (uint8_t)AddrReg;            // First byte
//						AddrReg>>=8;
//					 }
//			  }
	      return;
	   }
	//Acknowledge Failure.
//	else if(sr1 & I2C_SR1_AF)
//		{
//			I2C_TxCnt++;
//		}
	//Data Register Empty (transmitters).
	else if(sr1 & I2C_SR1_TXE)
		{
			i2c->DR = I2C_pBuf[I2C_TxCnt];
			I2C_TxCnt++;
			if(I2C_TxCnt >= I2C_TxSize)
				{
					I2C_TxCnt = 0;
					i2c->CR1 |= I2C_CR1_STOP;
				}
			return;
		}
	//Byte Transfer Finished.
	else if(sr1 & I2C_SR1_BTF)
		{

			return;
		}






















//	switch(i2cState){
//		//-----------------
//		//состояние START сформировано.
//		case(I2C_STATE_START):
//
//			if(sr1 & I2C_SR1_SB)
//				{
//					i2c->DR  = I2C_ADDRESS(slaveAddr, I2C_Mode);
//					i2cState = I2C_STATE_ADDR_SEND_OK;
//				}
//
//		break;
//		//-----------------
//		case(I2C_STATE_ADDR_SEND_OK):
//
//			if(sr1 & I2C_SR1_ADDR)
//				{
//					i2c->DR = I2C_pBuf[0];
//					TxCounter++;
//					i2cState = I2C_STATE_TXReg_EMPTY;
//				}
//
//		break;
//		//-----------------
//		case(I2C_STATE_TXReg_EMPTY):
//
//			if(sr1 & I2C_SR1_TXE)
//				{
//					if(TxCounter < I2C_TxSize)
//						{
//							i2c->DR = I2C_pBuf[TxCounter];
//							TxCounter++;
//						}
//					else
//						{
//							i2c->CR1 |= I2C_CR1_STOP;
//							TxCounter = 0;
//							i2cState  = I2C_STATE_IDLE;
//						}
//				}
//			else
//				{
//					i2c->CR1 |= I2C_CR1_STOP;
//					TxCounter = 0;
//					i2cState  = I2C_STATE_IDLE;
//				}
//		break;
//		//-----------------
//		case(I2C_STATE_ADDR_SEND_OK):
//
//			if(sr1 & I2C_SR1_ADDR)
//				{
//					i2c->DR = I2C_pBuf[0];
//					TxCounter++;
//					i2cState = I2C_STATE_TXReg_EMPTY;
//				}
//
//		break;
//		//-----------------
//	}
	//---------------------

//	//прерывание с событием EV5 -
//	if(sr1 & I2C_SR1_SB)
//		{
//			i2c->DR = I2C_ADDRESS(slaveAddr, I2C_Mode);
//		}
//	//
//	if(sr1 & I2C_SR1_TXE)
//		{
//			if(TxCounter < I2C_TxSize) i2c->DR   = I2C_pBuf[TxCounter++];
//			else		               i2c->CR1 |= I2C_CR1_STOP;
//		}
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание I2C1.
void I2C1_EV_IRQHandler(void){

//	I2C_handler(I2C1, addr);
}
//**********************************************************
//Прерывание I2C2.
void I2C2_EV_IRQHandler(void){

	I2C_Int_Handler(I2C2, I2C_SlaveAddr);
	LedPA7_Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
//void I2C_Init(void){
//
//	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
//	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;// и шины I2C1.
//	//Инициализация портов.
//	GPIOB->CRL   |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7;
//	//Инициализация I2C.
//	I2C1->CR2  &= ~I2C_CR2_FREQ;//
//	I2C1->CR2  |= 36;           //APB1 = 36MHz
//	I2C1->CCR  &= ~I2C_CCR_CCR; //
//	I2C1->CCR   = 180;          //(36MHz/100KHz/2)
//	I2C1->TRISE = 37;           //(1mcs/(1/36MHz)+1)
//	I2C1->CR1  |= I2C_CR1_PE;   //Включение модуля I2C1.
//	for(uint8_t i = 0; i < 255; i++) {__NOP();};
//}
////**********************************************************
//void I2C_SendDeviceAddr(uint8_t DeviceAddr){
//
//	while(I2C1->SR2 & I2C_SR2_BUSY){};//Ждем освобождения шины.
//	//Формирование Start condition.
//	I2C1->CR1 |= I2C_CR1_START;
//	while(!(I2C1->SR1 & I2C_SR1_SB));//Ожидание формирования Start condition.
//	(void)I2C1->SR1;				 //Для сброса флага SB необходимо прочитать SR1
//	//Передаем адрес slave.
//	I2C1->DR = DeviceAddr;
//	while(!(I2C1->SR1 & I2C_SR1_ADDR));//Ожидаем окончания передачи адреса и
//	(void)I2C1->SR1;				   //сбрасываем бит ADDR (чтением SR1 и SR2):
//	(void)I2C1->SR2;				   //
//}
////**********************************************************
//void I2C_SendData(uint8_t *Data, uint8_t DataLen){
//
//	for(uint8_t i = 0; i < DataLen; i++)
//		{
//			I2C1->DR = *(Data + i);
//			while(!(I2C1->SR1 & I2C_SR1_TXE));//Ждем освобождения буфера
//		}
//}
////**********************************************************
//void I2C_SendStop(void){
//
//	while(!(I2C1->SR1 & I2C_SR1_BTF));//Ждем окончания передачи
//	I2C1->CR1 |= I2C_CR1_STOP;		  //Формируем Stop
//}
////**********************************************************
//void I2C_WriteData(uint8_t DeviceAddr, uint16_t RegisterAddr, uint8_t *Data, uint8_t DataLen){
//
//	//Формирование Start condition.
//	I2C1->CR1 |= I2C_CR1_START;
//	while(!(I2C1->SR1 & I2C_SR1_SB));//Ожидание формирования Start condition.
//	(void)I2C1->SR1;				 //Для сброса флага SB необходимо прочитать SR1
//
//	//Передаем адрес slave.
//	I2C1->DR = DeviceAddr;
//	while(!(I2C1->SR1 & I2C_SR1_ADDR));//Ожидаем окончания передачи адреса и
//	//if(!(I2C1->SR1 & I2C_SR1_ADDR))return;
//	(void)I2C1->SR1;				     //сбрасываем бит ADDR (чтением SR1 и SR2):
//	(void)I2C1->SR2;				     //
//
//	//Передаем старший байт адреса
//	I2C1->DR = (uint8_t)(RegisterAddr >> 8);
//	while(!(I2C1->SR1 & I2C_SR1_TXE));	  //Ждем освобождения буфера
//	//Передаем младший байт адреса
//	I2C1->DR = (uint8_t)RegisterAddr;
//	while(!(I2C1->SR1 & I2C_SR1_BTF));	  //Ждем окончания передачи
//
//	//Передаем данные для ячейки памяти
//	for(uint8_t i = 0; i < DataLen; i++)
//		{
//			I2C1->DR = *(Data + i);
//			while(!(I2C1->SR1 & I2C_SR1_TXE));//Ждем освобождения буфера
//		}
//	while(!(I2C1->SR1 & I2C_SR1_BTF));//Ждем окончания передачи
//	I2C1->CR1 |= I2C_CR1_STOP;		  //Формируем Stop
//}
//*******************************************************************************************
//*******************************************************************************************

