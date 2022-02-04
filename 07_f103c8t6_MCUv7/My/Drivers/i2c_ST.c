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
		if(++wait_count >= I2C_WAIT_TIMEOUT) return 1;
	}
	return 0;
}
//*******************************************************************************************
//*******************************************************************************************
void I2C_Init(I2C_TypeDef *i2c, uint32_t remap){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//Включаем тактирование GPIOB
	//------------------------------
	//Инициализация портов I2C_1.
	if(i2c == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		//Ремап
		//I2C1_SCL - PB8
		//I2C1_SDA - PB9
		if(remap)
		{
			AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
			GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1 |
						  GPIO_CRH_CNF8    | GPIO_CRH_CNF9;
		}
		//I2C1_SCL - PB6
		//I2C1_SDA - PB7
		else
		{
			GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 |
						  GPIO_CRL_CNF6    | GPIO_CRL_CNF7;

		}
	}
	//------------------------------
	//Инициализация портов I2C_2.
	//I2C2_SCL - PB10
	//I2C2_SDA - PB11
	else if(i2c == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		GPIOB->CRH   |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 |
					  	GPIO_CRH_CNF10    | GPIO_CRH_CNF11;
	}
	else return;
	//------------------------------
	//Инициализация I2C.
	i2c->CR2  &= ~I2C_CR2_FREQ;   		  //
	i2c->CR2  |= (36 << I2C_CR2_FREQ_Pos);//APB1 = 36MHz

	//Скорость работы.
	i2c->CCR &= ~I2C_CCR_CCR;

	//STANDART_Mode(100kHz).
	//i2c->CCR  &= ~I2C_CCR_FS;//1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz)
	//i2c->CCR   =  120;       //(36MHz/I2C_BAUD_RATE/2)
	//i2c->TRISE =  37;        //(1mcs/(1/36MHz)+1)

	//Fast_Mode(400kHz)
	i2c->CCR  |= I2C_CCR_FS;//1 - режим FastMode(400kHz), 0 - режим STANDART(100kHz).
	i2c->CCR   = 30;        //(36MHz/I2C_BAUD_RATE/2)
	i2c->TRISE = 12;        //(0.3mcs/(1/36MHz)+1)

	//i2c->CR1 |= I2C_CR1_ENPEC;

	i2c->CR1 |= I2C_CR1_PE; //Включение модуля I2C.
}
//**********************************************************
uint32_t I2C_StartAndSendDeviceAddr(I2C_TypeDef *i2c, uint8_t deviceAddr){

	//Формирование Start condition.
	i2c->CR1 |= I2C_CR1_START;
	if(I2C_LongWait(i2c, I2C_SR1_SB)) return 1;//Ожидание формирования Start condition.
	(void)i2c->SR1;				      		   //Для сброса флага SB необходимо прочитать SR1

	//Передаем адрес.
	i2c->DR = deviceAddr;
	if(I2C_LongWait(i2c, I2C_SR1_ADDR)) return 1;//Ожидаем окончания передачи адреса
	(void)i2c->SR1;				        	     //сбрасываем бит ADDR (чтением SR1 и SR2):
	(void)i2c->SR2;				        	     //
	//(void)i2c->DR;

	return 0;
}
//**********************************************************
void I2C_SendByte(I2C_TypeDef *i2c, uint8_t byte){

	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;//Ждем освобождения буфера
	i2c->DR = byte;

}
//**********************************************************
void I2C_SendData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len){

	for(uint16_t i = 0; i < len; i++)
	{
		if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;//Ждем освобождения буфера
		i2c->DR = *(pBuf + i);
	}
	if(I2C_LongWait(i2c, I2C_SR1_BTF)) return;
	i2c->CR1 |= I2C_CR1_STOP;		 //Формируем Stop
}
//**********************************************************
//прием данных
void I2C_ReadData(I2C_TypeDef *i2c, uint8_t *pBuf, uint16_t len){

	//receiving 1 byte
	if(len == 1)
	{
		i2c->CR1 &= ~I2C_CR1_ACK;//Фомирование NACK.
		i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop.
		if(I2C_LongWait(i2c, I2C_SR1_RXNE)) return;//ожидаем окончания приема байта
		*(pBuf + 0) = i2c->DR;				       //считали принятый байт.
		i2c->CR1 |= I2C_CR1_ACK;				   //to be ready for another reception
		return;
	}
	//receiving 2 bytes
	else if(len == 2)
	{
		i2c->CR1 |=  I2C_CR1_POS;//
		i2c->CR1 &= ~I2C_CR1_ACK;//Фомирование NACK.
		if(I2C_LongWait(i2c, I2C_SR1_BTF)) return;
		i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop.
		*(pBuf + 0) = i2c->DR;	 //считали принятый байт.
		*(pBuf + 1) = i2c->DR;	 //считали принятый байт.
		i2c->CR1 &= ~I2C_CR1_POS;//
		i2c->CR1 |= I2C_CR1_ACK; //to be ready for another reception
		return;
	}
	//receiving more than 2 bytes
	else
	{
		uint16_t i;
		for(i=0; i<(len-3); i++)
		{
			if(I2C_LongWait(i2c, I2C_SR1_RXNE)) return;
			*(pBuf + i) = i2c->DR;//Read DataN
		}
		//Вычитываем оставшиеся 3 байта.
		if(I2C_LongWait(i2c, I2C_SR1_BTF)) return;
		i2c->CR1 &= ~I2C_CR1_ACK; //Фомирование NACK
		*(pBuf + i + 0) = i2c->DR;//Read DataN-2
		i2c->CR1 |= I2C_CR1_STOP; //Формируем Stop
		*(pBuf + i + 1) = i2c->DR;//Read DataN-1
		if(I2C_LongWait(i2c, I2C_SR1_RXNE)) return;
		*(pBuf + i + 2) = i2c->DR;//Read DataN
		i2c->CR1 |= I2C_CR1_ACK;
	}
}
//*******************************************************************************************
//*******************************************************************************************
void I2C_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

	//Формирование Start + AddrSlave|Write.
	if(I2C_StartAndSendDeviceAddr(i2c, deviceAddr | I2C_MODE_WRITE) != 0) return;

	//Передача адреса в который хотим записать.
	i2c->DR = regAddr;
	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;

	//передача данных на запись.
	I2C_SendData(i2c, pBuf, len);
}
//**********************************************************
void I2C_Read(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){

	//Формирование Start + AddrSlave|Write.
	if(I2C_StartAndSendDeviceAddr(i2c, deviceAddr | I2C_MODE_WRITE) != 0) return;

	//Передача адреса с которого начинаем чтение.
	i2c->DR = regAddr;
	if(I2C_LongWait(i2c, I2C_SR1_TXE)) return;
	//---------------------
	//Формирование reStart condition.
	i2c->CR1 |= I2C_CR1_STOP; //Это команда нужна для работы с DS2782. Без нее не работает

	//Формирование Start + AddrSlave|Read.
	if(I2C_StartAndSendDeviceAddr(i2c, deviceAddr | I2C_MODE_READ) != 0) return;

	//прием даннных
	I2C_ReadData(i2c, pBuf, len);
}
//*******************************************************************************************
//*******************************************************************************************
//****************************Работа I2C по прерываниям.*************************************
volatile static uint8_t I2cIT_TxBuf[32] = {0};
volatile static uint8_t I2cIT_RxBuf[32] = {0};

volatile static uint8_t  I2cIT_Buf[32] = {0};
volatile static uint32_t I2cIT_BufSize = 0;

volatile static uint32_t I2cIT_State = 0;



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
	I2cIT_State = 1;
}
//**********************************************************
void I2C_IT_StartRx(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pRxBuf, uint32_t len){

	SlaveAddr    = deviceAddr;
	SlaveRegAddr = regAddr;
	ptrRxBuf     = pRxBuf;
	RxBufSize    = len;

	i2c->CR1 |= I2C_CR1_START;
	I2cIT_State = 1;
}
//**********************************************************
//сохраняет сообщение и инициирует передачу/прием данных по I2C шине,
//формируя состояние СТАРТ и разрешая прерывания.
//В качестве параметров принимает указатель на буфер, в котором хранится сообщение,
//и количество байтов, которые нужно передать/принять.
//Первый байт сообщения для функции должен содержать адрес ведомого устройства и бит R/W.
void I2C_IT_SendData(I2C_TypeDef *i2c, uint8_t *pBuf, uint32_t len){

	I2cIT_BufSize = len;
	I2cIT_Buf[0]  = pBuf[0];

	//если первый байт типа SLA+W
	if((pBuf[0] & (1<<0)) == I2C_MODE_WRITE)
	{
		//то сохряняем остальную часть сообщения
		for(uint32_t i=1; i<len; i++)
		{
			I2cIT_Buf[i] = pBuf[i];
		}
	}

	i2c->CR1 |= I2C_CR1_START;
}
//**********************************************************
//получить принятые данные. Она просто копирует принятые ранее данные в буфер msg.
//Переменная msgSize определяет сколько байт нужно скопировать.
//Функция возвращает 0, если сообщение не было принято и 1 если сообщение получено.
uint32_t I2C_IT_GetData(I2C_TypeDef *i2c, uint8_t *pBuf, uint32_t len){


	return 1;
}
//**********************************************************
uint32_t I2C_IT_GetState(void){

	return I2cIT_State;
}
//**********************************************************
//Обработчик прерывания событий I2C
void I2C_IT_EV_Handler(I2C_TypeDef *i2c){

	static uint32_t txCount = 0;
	//LedPC13Toggel();
	//------------------------------
	//------------------------------
	//Start bit sent (Master)
	if(i2c->SR1 & I2C_SR1_SB)
	{
		(void)i2c->SR1;				         //Для сброса флага SB необходимо прочитать SR1
		//i2c->DR = SlaveAddr | I2C_MODE_WRITE;//Передаем адрес slave + Запись.
		i2c->DR = I2cIT_Buf[txCount];//Передаем SlaveAddr+R/W.
		txCount++;
		return;
	}
	//------------------------------
	//------------------------------
	//Address sent (Master) or Address matched (Slave)
	if(i2c->SR1 & I2C_SR1_ADDR)
	{
		(void)i2c->SR1; //сбрасываем бит ADDR (чтением SR1 и SR2):
		(void)i2c->SR2;
		//i2c->DR   = SlaveRegAddr; //Передача адреса в который хотим записать.
		i2c->DR   = I2cIT_Buf[txCount];//Передача адреса в который хотим записать.
		i2c->CR2 |= I2C_CR2_ITBUFEN;   //Buffer interrupt enable.
		txCount++;
		return;
	}
	//------------------------------
	//------------------------------
	//Transmit buffer empty
	if(i2c->SR1 & I2C_SR1_TXE)
	{
		(void)i2c->SR1; //сбрасываем бит TXE (чтением SR1 и SR2):
		(void)i2c->SR2;

		//Был передан адрес регистра чтения.



		if(txCount < TxBufSize)
		{
			i2c->DR = *(ptrTxBuf + txCount);
			txCount++;
		}
		else
		{
			i2c->CR2 &= ~I2C_CR2_ITBUFEN;//Запрет прерывания по опостощению буфера Tx.
			i2c->CR1 |= I2C_CR1_STOP;    //Формируем Stop

			I2cIT_State = 0;
			txCount     = 0;
		}
		return;
	}
	//------------------------------
	//------------------------------
	//Receive buffer not empty
	if(i2c->SR1 & I2C_SR1_RXNE)
	{

		return;
	}
	//------------------------------
	//------------------------------
}
//**********************************************************
//Обработчик прерывания ошибок I2C
void I2C_IT_ER_Handler(I2C_TypeDef *i2c){

	LedPC13Toggel();
	//------------------------------
	//NACK - Acknowledge failure
	if(i2c->SR1 & I2C_SR1_AF)
	{
		i2c->SR1 &= ~I2C_SR1_AF; //Сброс AF.
		i2c->CR1 |= I2C_CR1_STOP;//Формируем Stop
		return;
	}
	//------------------------------
	//Bus error
	if(i2c->SR1 & I2C_SR1_BERR)
	{
		i2c->SR1 &= ~I2C_SR1_BERR; //Сброс BERR.
	}
	//------------------------------
	//Arbitration loss (Master)
	if(i2c->SR1 & I2C_SR1_ARLO)
	{
		i2c->SR1 &= ~I2C_SR1_ARLO; //Сброс ARLO.
	}
	//------------------------------
	//Overrun/Underrun
	if(i2c->SR1 & I2C_SR1_OVR)
	{
		i2c->SR1 &= ~I2C_SR1_OVR; //Сброс OVR.
	}
	//------------------------------
	//PEC error
	if(i2c->SR1 & I2C_SR1_PECERR)
	{
		i2c->SR1 &= ~I2C_SR1_PECERR; //Сброс PECERR.
	}
	//------------------------------
	//Timeout/Tlow error
	if(i2c->SR1 & I2C_SR1_TIMEOUT)
	{
		i2c->SR1 &= ~I2C_SR1_TIMEOUT; //Сброс TIMEOUT.
	}
	//------------------------------
	//SMBus Alert
	if(i2c->SR1 & I2C_SR1_SMBALERT)
	{
		i2c->SR1 &= ~I2C_SR1_SMBALERT; //Сброс SMBALERT.
	}
	//------------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//Обработчик прерывания событий I2C
void I2C1_EV_IRQHandler(void){

	I2C_IT_EV_Handler(I2C1);
}
//**********************************************************
//Обработчик прерывания ошибок I2C
void I2C1_ER_IRQHandler(void){

	I2C_IT_ER_Handler(I2C1);
}
//**********************************************************
//**********************************************************
//Обработчик прерывания событий I2C
void I2C2_EV_IRQHandler(void){

	I2C_IT_EV_Handler(I2C2);
}
//**********************************************************
//Обработчик прерывания ошибок I2C
void I2C2_ER_IRQHandler(void){

	I2C_IT_ER_Handler(I2C2);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//**************************Работа чере DMA. Не отлажено!!!**********************************
//I2C1_TX -> DMA1_Ch6
//I2C1_RX -> DMA1_Ch7

//I2C2_TX -> DMA1_Ch4
//I2C2_RX -> DMA1_Ch5

#define I2C1_TX_DMAChannel	DMA1_Channel6
#define I2C1_RX_DMAChannel	DMA1_Channel7

#define I2C2_TX_DMAChannel	DMA1_Channel4
#define I2C2_RX_DMAChannel	DMA1_Channel5

//*******************************************************************************************
void I2C_DMA_Init(I2C_TypeDef *i2c, DMA_Channel_TypeDef *dma, uint32_t remap){

	I2C_Init(i2c, remap);

	//Инициализация DMA.
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;//Enable the peripheral clock DMA1

	/* DMA1Channel config */
	dma->CCR &= ~DMA_CCR_EN;			 // Channel disable
	dma->CPAR = (uint32_t)&(i2c->DR);    // Peripheral address.
	//dma->CMAR  = (uint32_t)sendBuf;	 // Memory address.
	//dma->CNDTR = 5; 		   	   		 // Data size.
	dma->CCR |= (3 << DMA_CCR_PL_Pos)   | // PL[1:0]: Channel priority level - 11: Very high.
			   (0 << DMA_CCR_PSIZE_Pos)| // PSIZE[1:0]: Peripheral size - 00: 8-bits.
			   (0 << DMA_CCR_MSIZE_Pos)| // MSIZE[1:0]: Memory size     - 00: 8-bits.
			    DMA_CCR_MINC |			 // MINC: Memory increment mode - Memory increment mode enabled.
				DMA_CCR_DIR  |           // DIR:  Data transfer direction: 1 - Read from memory.
				//DMA_CCR_CIRC | 		 // CIRC: Circular mode
				//DMA_CCR_TEIE | 		 // TEIE: Transfer error interrupt enable
				//DMA_CCR_HTIE | 		 // HTIE: Half transfer interrupt enable
				DMA_CCR_TCIE;// | 		 // TCIE: Transfer complete interrupt enable
				//DMA_CCR_EN;			 // EN: Channel enable

	NVIC_SetPriority(DMA1_Channel6_IRQn, 0);// Set priority
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);     // Enable DMA1_Channel6_IRQn
	//DMA1_Channel6->CCR |= DMA_CCR_EN;       // Channel enable
}
//**********************************************************
void I2C_DMA_StartTx(DMA_Channel_TypeDef *dma, uint8_t *pData, uint32_t size){

	dma->CCR  &= ~DMA_CCR_EN;	   	// Channel disable
	dma->CMAR  = (uint32_t)pData;	// Memory address.
	dma->CNDTR = size; 		   		// Data size.
	//dma->CCR  |= DMA_CCR_EN;     	// Channel enable
}
//**********************************************************
void I2C_DMA_Write(I2C_TypeDef *i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t size){

	volatile DMA_Channel_TypeDef *dma;

	if(i2c == I2C1) dma = DMA1_Channel6;
	else			dma = DMA1_Channel4;

	dma->CCR  &= ~DMA_CCR_EN;	// Channel disable
	dma->CMAR  = (uint32_t)(pBuf+0);// Memory address.
	dma->CNDTR = size;			// Data size.

	__disable_irq();

	i2c->CR2 |= I2C_CR2_DMAEN;  //DMAEN(DMA requests enable)
	//Формирование Start + AddrSlave|Write.
	if(I2C_StartAndSendDeviceAddr(i2c, deviceAddr|I2C_MODE_WRITE) != 0) return;
	//Передача адреса в который хотим записать.
	i2c->DR = regAddr;

	dma->CCR  |= DMA_CCR_EN; 	//DMA Channel enable

	__enable_irq();
}
//**********************************************************


//*******************************************************************************************
//*******************************************************************************************
//Прерываение от DMA1.
//I2C1_TX -> DMA1_Ch6
//I2C1_RX -> DMA1_Ch7

//I2C2_TX -> DMA1_Ch4
//I2C2_RX -> DMA1_Ch5


//I2C1_TX -> DMA1_Ch6
void DMA1_Channel6_IRQHandler(void){

	//-------------------------
	//Обмен завершен.
	if(DMA1->ISR & DMA_ISR_TCIF6)
	{
		//LedPC13On();

		DMA1->IFCR         |=  DMA_IFCR_CTCIF6;//сбросить флаг окончания обмена.
		DMA1_Channel6->CCR &= ~DMA_CCR_EN;     //отключение канала DMA.

		while(!(I2C1->SR1 & I2C_SR1_BTF)){};//Ожидаем окончания ....
		I2C1->CR1 |= I2C_CR1_STOP;          //Формируем Stop

		//LedPC13Off();
	}
	//-------------------------
	//Передана половина буфера
	if(DMA1->ISR & DMA_ISR_HTIF6)
	{
		DMA1->IFCR |= DMA_IFCR_CHTIF6;//сбросить флаг.
	}
	//-------------------------
	//Произошла ошибка при обмене
	if(DMA1->ISR & DMA_ISR_TEIF6)
	{
		DMA1->IFCR |= DMA_IFCR_CTEIF6;//сбросить флаг.
	}
	//-------------------------
}
//**********************************************************
//I2C1_RX -> DMA1_Ch7
void DMA1_Channel7_IRQHandler(void){

	//-------------------------
	//Обмен завершен.
	if(DMA1->ISR & DMA_ISR_TCIF7)
	{
		DMA1->IFCR         |=  DMA_IFCR_CTCIF7;//сбросить флаг окончания обмена.
		DMA1_Channel7->CCR &= ~DMA_CCR_EN;     //отключение канала DMA.

	}
	//-------------------------
//  //Передана половина буфера
//  if(DMA1->ISR & DMA_ISR_HTIF7)
//    {

//    }
  //-------------------------
//  //Произошла ошибка при обмене
//  if(DMA1->ISR & DMA_ISR_TEIF7)
//    {

//    }
  //-------------------------
}
//*******************************************************************************************
//*******************************************************************************************




