/*
 * bootLoader.c
 *
 *  Created on: 10 февр. 2023 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

#include "bootLoader.h"

//*******************************************************************************************
//*******************************************************************************************
static I2C_IT_t			   	  bootLoaderI2c;
static uint8_t *rxFrame  	= bootLoaderI2c.pRxBuf;
static uint8_t *responseBuf = bootLoaderI2c.pTxBuf;

static volatile uint8_t cmd = 0;
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _clear_RxFrame()
// Description : очистка первых двух принятых байтов
//				 это нужно для корректного приема комманд после передачи NACK
// Parameters  :
// RetVal      :
//*****************************************
static void _clear_RxFrame(void){

	rxFrame[0] = 0;	   	 	//Сброс приемного буфера
	rxFrame[1] = 0;
}
//*******************************************************************************************
// Function    : B _get_XorChecksum()
// Description : расчет контрольной суммы. Полученные блоки байтов данных подвергаются операции XOR.
//			     Байт, содержащий вычисленное XOR всех предыдущих байтов, добавляется в конец каждого
//				 сообщения (байт контрольной суммы). При выполнении XOR всех полученных байтов данных и
//				 контрольной суммы результат в конце пакета должен быть 0x00.
// Parameters  : buf  - указатель на буфер
//				 size - кол-во байтов для расчета, от 1 байта
// RetVal      : XOR байтов
//*****************************************
static uint8_t _get_XorChecksum(uint8_t *buf, uint32_t size){

	uint8_t xor = 0;
	//---------------------
	//Checksum: XOR (byte 0, byte 1, byte 2, byte 3)
	//bootBuf[4] = bootBuf[0] ^ bootBuf[1] ^ bootBuf[2] ^ bootBuf[3];
	for(uint32_t i=0; i < size; i++)
	{
		xor ^= buf[i];
	}
	return xor;
}
//*******************************************************************************************
// Function    : _send_Response()
// Description : отправить ответ мастеру
// Parameters  : response - код ответа
// RetVal      :
//*****************************************
void _send_Response(uint8_t response){

	responseBuf[0] = response;
	I2C_IT_SetTxSize(&bootLoaderI2c, 1);//1 байт на передачу
}
//*******************************************************************************************
// Function    : _get_StartAddres()
// Description : собирает 4-хбайтовый адрес из принятых байтов
// Parameters  : buf - указатель на младший байт принятого адреса
// RetVal      : 4-хбайтовый адрес
//*****************************************
static uint32_t _get_StartAddress(uint8_t *buf){

	return (uint32_t)((buf[0]<<24) |
			  	  	  (buf[1]<<16) |
					  (buf[2]<< 8) |
					  (buf[3]<< 0));
}
//*******************************************************************************************
// Function    : _cmd_CMD_BOOT_GetVersion()
// Description : заглушка
// Parameters  : None
// RetVal      :
//*****************************************
static uint8_t _get_ReadoutProtectState(void){

	//заглушка
	return 0;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _cmd_CMD_BOOT_GetVersion()
// Description : выполнение команды CMD_BOOT_GetVersion - Получает версию загрузчика.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_GetVersion(void){

	static uint8_t state = 0;
		   uint8_t temp  = 0;
	//-----------------------
	switch(state)
	{
		//------------
		//Передача ACK
		case(0):
			_send_Response(CMD_ACK);
			state = 1;
		break;
		//------------
		//Передача версии загрузчика
		case(1):
			_send_Response(BOOT_I2C_VERSION);
			state = 2;
		break;
		//------------
		//Передача ACK
		case(2):
			_send_Response(CMD_ACK);
			state = 3;
		break;
		//------------
		//Команда выполнена
		case(3):
			state = 0;	//
			temp  = 1;
		break;
		//------------
	}
	return temp;
}
//*******************************************************************************************
// Function    : _cmd_BOOT_GetID()
// Description : выполнение команды CMD_BOOT_GetID - Получает идентификатор чипа
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_GetID(void){

	static uint8_t state = 0;
	//-----------------------
	switch(state)
	{
		//------------
		//Передача ACK
		case(0):
			_send_Response(CMD_ACK);
			state = 1;
		break;
		//------------
		//Передача кадра данных
		case(1):
			responseBuf[0] = 1;		//N = the number of bytes-1 (for STM32, N = 1), except for current byte and ACKs
			responseBuf[1] = 0xDC;	//product ID MSB
			responseBuf[2] = 0x12;	//product ID LSB
			I2C_IT_SetTxSize(&bootLoaderI2c, 3);//3 байта на передачу
			state = 2;
		break;
		//------------
		//Передача ACK
		case(2):
			_send_Response(CMD_ACK);
			state = 3;
		break;
		//------------
		//Команда выполнена
		case(3):
			state = 0;	//сброс рабочих переменных.
			return 1;  	//признак завершения команды.
		break;
		//------------
	}
	return 0; //признак выполения команды.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_RM(void)()
// Description : выполнение команды CMD_BOOT_RM - Read Memory - Читает до 256 байт памяти,
//				 начиная с адреса, указанного приложением.
// Parameters  : None
// RetVal      :1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_RM(void){

	static uint8_t  state 	  = 0;
	static uint32_t startAddr = 0;
	static uint32_t size	  = 0;
	//-----------------------
	switch(state)
	{
		//------------
		//Проверка ROP (Readout Protect) и передача ACK/NACK
		case(0):
			//ROP активна
			if(_get_ReadoutProtectState() == 1)
			{
				_send_Response(CMD_NACK);//Передача NACK
				_clear_RxFrame();		 //Сброс приемного буфера
				return 1;				 //признак завершения команды.
			}
			//ROP отключена
			_send_Response(CMD_ACK); //Передача ACK
			state = 1;		 		 //переходим на Receive data frame: start address
		break;
		//------------
		//Receive data frame: start address (4 bytes) with checksum
		case(1):
			//Контрольная сумма не совпала
			if(_get_XorChecksum(rxFrame, 5) != 0)
			{
				_send_Response(CMD_NACK);//Передача NACK
				_clear_RxFrame();		 //Сброс приемного буфера
				state = 0;
				return 1;				 //признак завершения команды.
			}
			//Контрольная сумма OK
			startAddr = _get_StartAddress(&rxFrame[0]);	//Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
			_send_Response(CMD_ACK);					//Передача ACK
			state = 2;		 							//переходим на Receive data frame: number of bytes to be read and a checksum
		break;
		//------------
		//Receive data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
		case(2):
			//Контрольная сумма не совпала
			if(_get_XorChecksum(rxFrame, 2) != 0)
			{
				_send_Response(CMD_NACK);//Передача NACK
				_clear_RxFrame();		 //Сброс приемного буфера
				state = 0;
				return 1;				 //признак завершения команды.
			}
			//Контрольная сумма OK
			size = rxFrame[0];		//Сохраняем количество передаваемых данных
			_send_Response(CMD_ACK);//Передача ACK
			state = 3;				//переходим на Send data frame: requested data to the host
		break;
		//------------
		//Send data frame: requested data to the host
		case(3):
			//STM32_Flash_ReadBufU32((void*)startAddr, (void*)responseBuf, size);//копируем нужное кол-во байтов в буфер передачи
			STM32_Flash_ReadBufU8((void*)startAddr, (void*)responseBuf, size);//копируем нужное кол-во байтов в буфер передачи
			I2C_IT_SetTxSize(&bootLoaderI2c, size);
			state = 4;	//переход к завершению команды - Команда выполнена
		break;
		//------------
		//Команда выполнена
		case(4):
			state 	  = 0;	//сброс рабочих переменных.
			startAddr = 0;
			size 	  = 0;
			return 1;   	//признак завершения команды.
		break;
		//------------
	}
	return 0; //признак выполения команды.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_WM(void)()
// Description : Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_WM(void){

	static uint8_t  state 	  = 0;
	static uint32_t startAddr = 0;
	static uint32_t size 	  = 0;
	//-----------------------
	switch(state)
	{
	//------------
		//Проверка ROP (Readout Protect) и передача ACK/NACK
		case(0):
			//ROP активна
			if(_get_ReadoutProtectState() == 1)
			{
				_send_Response(CMD_NACK);//Передача NACK
				_clear_RxFrame();		 //Сброс приемного буфера
				return 1;				 //признак завершения команды.
			}
			//ROP отключена
			_send_Response(CMD_ACK); //Передача ACK
			state = 1;		 		 //переходим на Receive data frame: start address
		break;
		//------------
		//Receive data frame: start address (4 bytes) with checksum
		case(1):
			//Контрольная сумма не совпала
			if(_get_XorChecksum(rxFrame, 5) != 0)
			{
				_send_Response(CMD_NACK);//Передача NACK
				_clear_RxFrame();		 //Сброс приемного буфера
				state = 0;
				return 1;				 //признак завершения команды.
			}
			//Контрольная сумма OK
			startAddr = _get_StartAddress(&rxFrame[0]);	//Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
			_send_Response(CMD_ACK);					//Передача ACK
			state = 2;		 							//переходим на Receive data frame: number of bytes to be read and a checksum
		break;
		//------------
		//Receive data frame:
		//- number of bytes to be written
		//- data to be written
		//- checksum
		case(2):
			//N, количество передаваемых байт (0 < N ≤ 255),
			//что соответствует передаче N+1 байт данных (максимум 256 байт).
			size = rxFrame[0] + 1;
			//Контрольная сумма не совпала
			if(_get_XorChecksum(rxFrame, size+2) != 0)//+2 т.к. +байт N и +байт Сhecksum
			{
				_send_Response(CMD_NACK);//Передача NACK
				_clear_RxFrame();		 //Сброс приемного буфера
				state = 0;
				return 1;				 //признак завершения команды.
			}
			//Контрольная сумма OK
			//Записать полученные данные в память со стартового адреса
			STM32_Flash_Unlock();
			STM32_Flash_WriteBuf((void*)&rxFrame[1], (void*)startAddr, size);//
			STM32_Flash_Lock();

			_send_Response(CMD_ACK);//Передача ACK
			state = 3;		 		//переходим на Data written in Option bytes ?
		break;
		//------------
		//Data written in Option bytes ?
		case(3):
			//Если команда Write Memory производит запись в область байтов
			//управления (the option byte area), то все байты в этой области
			//будут стерты перед записью новых значений, и по завершению
			//команды загрузчик генерирует системный сброс, чтобы установки
			//новых значений байт опций вступили в силу.
		//break;
		//------------
		//Команда выполнена
		case(4):
			state 	  = 0;	//сброс рабочих переменных.
			startAddr = 0;
			size 	  = 0;
			return 1;   	//признак завершения команды.
		break;
		//------------
	}
	return 0; //признак выполения команды.
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Обработчик принятых команда
static void _bootLoader_CmdParsing(void){

	//static uint8_t cmd = 0; //0 - команда небыло (или была битая) или команда отработана.
	//----------------------------------------
	//В начале транзакции приходит код команды и инверсия кода команды.
	//После чего хост ждет ACK/NACK
	if(cmd == 0)
	{
		   cmd = ~rxFrame[0];	//0-й байт - код команды
		if(cmd == rxFrame[1])	//1-й байт - инверсия кода команды
		{
			cmd = rxFrame[0];	//запоминаем принятую команду и переходим к ее выполнению.
		}
		//Неверная команда
		else
		{
			cmd = 0; 			   		//0 - команда битая.
			_clear_RxFrame();			//Сброс приемного буфера
			_send_Response(CMD_NACK);	//передаем NACK
			return;
		}
	}
	//----------------------------------------
	//Выполнение принятой ранее команды
	switch(cmd)
	{
		//-------------------
		//Получает версию загрузчика.
		case(CMD_BOOT_GetVersion):
			if(_cmd_BOOT_GetVersion()) cmd = 0;//0 - команда отработана.
		break;
		//-------------------
		//Получает идентификатор чипа
		case(CMD_BOOT_GetID):
			if(_cmd_BOOT_GetID()) cmd = 0;//0 - команда отработана.
		break;
		//-------------------
		//Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
		case(CMD_BOOT_RM):
//			if(_cmd_BOOT_RM()) cmd = 0;//0 - команда отработана.
//		break;
		//-------------------
		//Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
		case(CMD_BOOT_WM):
//			if(_cmd_BOOT_WM()) cmd = 0;//0 - команда отработана.
//		break;
		//-------------------
		case(CMD_BOOT_NS_WM):
		//-------------------
		case(CMD_BOOT_Erase):
		//-------------------
		case(CMD_BOOT_NS_Erase):
		//-------------------
		case(CMD_BOOT_Go):
		//----------------------------------------
		//Передаем NACK на незнакомую команда
		default:
			LED_PC13_Toggel();
			_send_Response(CMD_NACK);
			cmd = 0;
		break;
		//-------------------
	}
}
//************************************************************
static void _bootLoader_EndOfTransmission(void){

	//Выполнение принятой ранее команды
	switch(cmd)
	{
		//-------------------
		//Получает версию загрузчика.
		case(CMD_BOOT_GetVersion):
			_cmd_BOOT_GetVersion();
		break;
		//-------------------
		//Получает идентификатор чипа.
		case(CMD_BOOT_GetID):
			_cmd_BOOT_GetID();
		break;
		//-------------------
		//Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
		//case(CMD_BOOT_RM):
		//	_cmd_BOOT_RM();
		//break;
		//-------------------
		//Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
		//case(CMD_BOOT_WM):
		//	if(_cmd_BOOT_WM()) cmd = 0;//0 - команда отработана.
		//break;
//		//-------------------
//		case(CMD_BOOT_NS_WM):
//		//-------------------
//		case(CMD_BOOT_Erase):
//		//-------------------
//		case(CMD_BOOT_NS_Erase):
//		//-------------------
//		//Переходит к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
//		case(CMD_BOOT_Go):
//		//----------------------------------------
//		//Передаем NACK на незнакомую команда
//		default:
//		break;
//		//-------------------
	}
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void BOOT_LOADER_I2CInit(void){

	//Инициализация I2C Slave для работы по прерываниям.
	bootLoaderI2c.i2c		= BOOT_I2C;			//используемый порт I2C
	bootLoaderI2c.i2cMode	= I2C_MODE_SLAVE;	//режим SLAVE
	bootLoaderI2c.gpioRemap = I2C_GPIO_NOREMAP;	//нет ремапа ножек порта I2C
	bootLoaderI2c.i2cSpeed  = BOOT_I2C_SPEED;	//скорость работы порта I2C
	bootLoaderI2c.slaveAddr = BOOT_I2C_ADDR;	//адрес устройства на нине I2C
	bootLoaderI2c.rxBufSize = I2C_IT_RX_BUF_SIZE_DEFAULT;
	bootLoaderI2c.txBufSize = I2C_IT_TX_BUF_SIZE_DEFAULT;

	bootLoaderI2c.i2cSlaveRxCpltCallback = _bootLoader_CmdParsing;
	bootLoaderI2c.i2cSlaveTxCpltCallback = _bootLoader_CmdParsing;//_bootLoader_EndOfTransmission;
	I2C_IT_Init(&bootLoaderI2c);

	//Прерывания I2C для работы протакола - приоритет = 1
	NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
	NVIC_SetPriority(I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
}
//**********************************************************
void BOOT_LOADER_Loop(void){


}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
