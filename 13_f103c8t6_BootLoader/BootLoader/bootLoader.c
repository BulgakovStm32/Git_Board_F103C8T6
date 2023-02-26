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
//static void _clear_RxFrame(void){
//
//	rxFrame[0] = 0;	   	 	//Сброс приемного буфера
//	rxFrame[1] = 0;
//}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _wait_STOP(void){

	while(bootLoaderI2c.ITState != I2C_IT_STATE_STOP);//Ждем завершения чтения ответа
	bootLoaderI2c.ITState = I2C_IT_STATE_READY;
}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _wait_NAC(void){

	while(bootLoaderI2c.ITState != I2C_IT_STATE_NAC);//Ждем завершения чтения ответа
	bootLoaderI2c.ITState = I2C_IT_STATE_READY;
}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _set_TxSize(uint32_t size){

	I2C_IT_SetTxSize(&bootLoaderI2c, size);
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

	uint8_t xor = buf[0];
	//---------------------
	//Checksum: XOR (byte 0, byte 1, byte 2, byte 3)
	//bootBuf[4] = bootBuf[0] ^ bootBuf[1] ^ bootBuf[2] ^ bootBuf[3];
	for(uint32_t i=1; i < size; i++)
	{
		xor ^= buf[i];
	}
	return xor;
}
//*******************************************************************************************
// Function    : _send_Byte
// Description : отправить ответ мастеру
// Parameters  : byte - код ответа
// RetVal      :
//*****************************************
static void _send_Byte(uint8_t byte){

	responseBuf[0] = byte;	//
	_set_TxSize(1);			//1 байт на передачу
	_wait_NAC();			//Ждем завершения чтения ответа
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
static uint8_t _get_ROPState(void){

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

	//Передача ACK
	_send_Byte(CMD_ACK);

	//Передача версии загрузчика
	_send_Byte(BOOT_I2C_VERSION);

	//Передача ACK
	_send_Byte(CMD_ACK);

	return 1;	//признак завершения команды.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_GetID()
// Description : выполнение команды CMD_BOOT_GetID - Получает идентификатор чипа
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_GetID(void){

	//Передача ACK
	_send_Byte(CMD_ACK);

	//Передача кадра данных - идентификатор чипа
	responseBuf[0] = 1;		//N = the number of bytes-1 (for STM32, N = 1), except for current byte and ACKs
	responseBuf[1] = 0xDC;	//product ID MSB
	responseBuf[2] = 0x12;	//product ID LSB
	_set_TxSize(3);			//3 байта на передачу
	_wait_NAC();			//Ждем завершения чтения ответа

	//Передача ACK
	_send_Byte(CMD_ACK);

	return 1;	//признак выполения команды.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_RM(void)()
// Description : выполнение команды CMD_BOOT_RM - Read Memory - Читает до 256 байт памяти,
//				 начиная с адреса, указанного приложением.
// Parameters  : None
// RetVal      :1 - команда выполнена; 0 - команда не выполнена.
//*****************************************
uint8_t _cmd_BOOT_RM(void){

	uint32_t startAddr;
	uint32_t size;
	//-----------------------
	//Проверка ROP (Readout Protect) и передача ACK/NACK
	//ROP активна
	if(_get_ROPState() == 1)
	{
		_send_Byte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//ROP отключена
	_send_Byte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) with checksum
	_wait_STOP();				//Ждем завершения приема start address

	//Контрольная сумма не совпала
	if(_get_XorChecksum(rxFrame, 5) != 0)
	{
		_send_Byte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK
	startAddr = _get_StartAddress(rxFrame);	//Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	_send_Byte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	_wait_STOP();	//Ждем завершения приема start address

	//Send data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	//bootBuf[0] = (uint8_t)(size - 1);		  //The number of bytes to be read-1
	//bootBuf[1] = (uint8_t)(size - 1) ^ 0xFF;//Checksum

	//Контрольная сумма не совпала
	if(_get_XorChecksum(rxFrame, 2) != 0xFF)
	{
		_send_Byte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK
	size = rxFrame[0] + 1;		//Сохраняем количество передаваемых данных
	_send_Byte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Send data frame: requested data to the host
	STM32_Flash_ReadBufU8((void*)startAddr, (void*)responseBuf, size);//копируем нужное кол-во байтов в буфер передачи
	_set_TxSize(size);	//кол-во байт на передачу
	_wait_NAC();		//Ждем завершения чтения ответа

	return 1; //признак выполения команды.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_WM(void)()
// Description : Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_WM(void){

	uint32_t startAddr;
	uint32_t size;
	//-----------------------

	return 1; //признак выполения команды.

//	static uint8_t  state 	  = 0;
//	static uint32_t startAddr = 0;
//	static uint32_t size 	  = 0;
//	//-----------------------
//	switch(state)
//	{
//	//------------
//		//Проверка ROP (Readout Protect) и передача ACK/NACK
//		case(0):
//			//ROP активна
//			if(_get_ReadoutProtectState() == 1)
//			{
//				_send_Byte(CMD_NACK);//Передача NACK
//				_clear_RxFrame();		 //Сброс приемного буфера
//				return 1;				 //признак завершения команды.
//			}
//			//ROP отключена
//			_send_Response(CMD_ACK); //Передача ACK
//			state = 1;		 		 //переходим на Receive data frame: start address
//		break;
//		//------------
//		//Receive data frame: start address (4 bytes) with checksum
//		case(1):
//			//Контрольная сумма не совпала
//			if(_get_XorChecksum(rxFrame, 5) != 0)
//			{
//				_send_Response(CMD_NACK);//Передача NACK
//				_clear_RxFrame();		 //Сброс приемного буфера
//				state = 0;
//				return 1;				 //признак завершения команды.
//			}
//			//Контрольная сумма OK
//			startAddr = _get_StartAddress(&rxFrame[0]);	//Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
//			_send_Response(CMD_ACK);					//Передача ACK
//			state = 2;		 							//переходим на Receive data frame: number of bytes to be read and a checksum
//		break;
//		//------------
//		//Receive data frame:
//		//- number of bytes to be written
//		//- data to be written
//		//- checksum
//		case(2):
//			//N, количество передаваемых байт (0 < N ≤ 255),
//			//что соответствует передаче N+1 байт данных (максимум 256 байт).
//			size = rxFrame[0] + 1;
//			//Контрольная сумма не совпала
//			if(_get_XorChecksum(rxFrame, size+2) != 0)//+2 т.к. +байт N и +байт Сhecksum
//			{
//				_send_Response(CMD_NACK);//Передача NACK
//				_clear_RxFrame();		 //Сброс приемного буфера
//				state = 0;
//				return 1;				 //признак завершения команды.
//			}
//			//Контрольная сумма OK
//			//Записать полученные данные в память со стартового адреса
//			STM32_Flash_Unlock();
//			STM32_Flash_WriteBuf((void*)&rxFrame[1], (void*)startAddr, size);//
//			STM32_Flash_Lock();
//
//			_send_Response(CMD_ACK);//Передача ACK
//			state = 3;		 		//переходим на Data written in Option bytes ?
//		break;
//		//------------
//		//Data written in Option bytes ?
//		case(3):
//			//Если команда Write Memory производит запись в область байтов
//			//управления (the option byte area), то все байты в этой области
//			//будут стерты перед записью новых значений, и по завершению
//			//команды загрузчик генерирует системный сброс, чтобы установки
//			//новых значений байт опций вступили в силу.
//		//break;
//		//------------
//		//Команда выполнена
//		case(4):
//			state 	  = 0;	//сброс рабочих переменных.
//			startAddr = 0;
//			size 	  = 0;
//			return 1;   	//признак завершения команды.
//		break;
//		//------------
//	}
//	return 0; //признак выполения команды.
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void _bootLoader_CmdParsing(void){

}
//************************************************************
void _bootLoader_EndOfTransmission(void){

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
	bootLoaderI2c.i2cSlaveTxCpltCallback = _bootLoader_EndOfTransmission;
	I2C_IT_Init(&bootLoaderI2c);

	//Прерывания I2C для работы протакола - приоритет = 1
	NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
	NVIC_SetPriority(I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
}
//**********************************************************
void BOOT_LOADER_Loop(void){

	static uint8_t currentCmd = 0;
	//----------------------------------------
	//Ждем команду
	_wait_STOP();//Ждем первый стоп.

	//В начале транзакции приходит код команды и инверсия кода команды.
	//После чего хост ждет ACK/NACK
	if(currentCmd == 0)
	{
		   currentCmd = ~rxFrame[0];	//0-й байт - код команды
		if(currentCmd == rxFrame[1])	//1-й байт - инверсия кода команды
		{
			currentCmd = rxFrame[0];	//запоминаем принятую команду и переходим к ее выполнению.
			LED_PC13_Toggel();			//мигнем светодиодом
		}
		//Неверная команда
		else
		{
			currentCmd = 0; 		//0 - команда битая.
			//_clear_RxFrame();		//Сброс приемного буфера
			rxFrame[0] = 0;	   	 	//Сброс приемного буфера
			rxFrame[1] = 0;
			_send_Byte(CMD_NACK);	//передаем NACK
			return;
		}
	}
	//----------------------------------------
	//Выполнение принятой ранее команды
	switch(currentCmd)
	{
		//-------------------
		//Получает версию загрузчика.
		case(CMD_BOOT_GetVersion):
			_cmd_BOOT_GetVersion();
			currentCmd = 0;	//команда отработана.

			//if(_cmd_BOOT_GetVersion()) currentCmd = 0;//0 - команда отработана.
		break;
		//-------------------
		//Получает идентификатор чипа
		case(CMD_BOOT_GetID):
			_cmd_BOOT_GetID();
			currentCmd = 0;	//команда отработана.

			//if(_cmd_BOOT_GetID()) currentCmd = 0;//0 - команда отработана.
		break;
		//-------------------
		//Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
		case(CMD_BOOT_RM):
			_cmd_BOOT_RM();
			currentCmd = 0;	//команда отработана.

			//if(_cmd_BOOT_RM()) cmd = 0;//0 - команда отработана.
		break;
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
			//LED_PC13_Toggel();
			currentCmd = 0;
			_send_Byte(CMD_NACK);	//передаем NACK
		break;
		//-------------------
	}
	//----------------------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
