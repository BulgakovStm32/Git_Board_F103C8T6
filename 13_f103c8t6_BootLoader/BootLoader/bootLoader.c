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
static I2C_IT_t   	  		  I2c;
static uint8_t *rxFrame  	= I2c.rxBuf;
static uint8_t *responseBuf = I2c.txBuf;
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _goToApp(uint32_t appAddr){

	void(*app)(void);
	uint32_t addrResetHandler;
	//------------------
	__disable_irq();										//Глобальное отключение прерываний.
	__set_MSP(*(volatile uint32_t*)appAddr);               	//Устанавливаем указатель стека SP приложения
	addrResetHandler = *(volatile uint32_t*)(appAddr + 4); 	//Адрес функции Reset_Handler приложения
	app = (void(*)(void))addrResetHandler; 			  		//Указатель на функцию Reset_Handler приложения
	app();								   					//Переход на функцию Reset_Handler приложения
}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _waitEndRx(void){

	uint32_t ms = DELAY_microSecCount();
	//------------------
	while(I2c.itState != I2C_IT_STATE_STOP)//Ждем завершения чтения ответа
	{
		//Мигаем...
		if((DELAY_microSecCount() - ms) >= 500000)
		{
			ms = DELAY_microSecCount();
			LED_PC13_Toggel();
		}
	}
	I2c.itState = I2C_IT_STATE_READY;
}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _waitEndTx(void){

	while(I2c.itState != I2C_IT_STATE_NAC);//Ждем завершения чтения ответа
	I2c.itState = I2C_IT_STATE_READY;
}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _setTxSize(uint32_t size){

	I2C_IT_SetTxSize(&I2c, size);
}
//*******************************************************************************************
// Function    : _send_Byte
// Description : отправить ответ мастеру
// Parameters  : byte - код ответа
// RetVal      :
//*****************************************
static void _sendByte(uint8_t byte){

	responseBuf[0] = byte;	//
	_setTxSize(1);			//1 байт на передачу
	_waitEndTx();			//Ждем завершения передачи байта
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
static uint8_t _getXorChecksum(uint8_t *buf, uint32_t size){

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
// Function    : _cmd_BOOT_Get() -- Не отлажено!!!!
// Description : Передача версии загрузчика и поддерживаемые команды.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_Get(void){

	//Передача ACK
	_sendByte(CMD_ACK);

	//Сборка кадра данных
	responseBuf[0]	= 19;			//N = Number of bytes to follow - 1, except current and ACKs
	responseBuf[1]	= 0x01;			//bootloader version
	responseBuf[2] 	= CMD_BOOT_Get;	//list of supported commands
	responseBuf[3] 	= CMD_BOOT_GetVersion;
	responseBuf[4] 	= CMD_BOOT_GetID;
	responseBuf[5] 	= CMD_BOOT_RM;
	responseBuf[6] 	= CMD_BOOT_Go;
	responseBuf[7] 	= CMD_BOOT_WM;
	responseBuf[8] 	= CMD_BOOT_NS_WM;
	responseBuf[9] 	= CMD_BOOT_Erase;
	responseBuf[10] = CMD_BOOT_NS_Erase;
	responseBuf[11] = CMD_BOOT_WP;
	responseBuf[12] = CMD_BOOT_NS_WP;
	responseBuf[13] = CMD_BOOT_WUP;
	responseBuf[14] = CMD_BOOT_NS_WUP;
	responseBuf[15] = CMD_BOOT_RP;
	responseBuf[16] = CMD_BOOT_NS_RP;
	responseBuf[17] = CMD_BOOT_RUP;
	responseBuf[18] = CMD_BOOT_NS_RUP;
	responseBuf[19] = CMD_BOOT_NS_GetMemCs;

	_setTxSize(20);	//кол-во байт на передачу
	_waitEndTx();	//Ждем завершения передачи кадра данных

	//Передача ACK
	_sendByte(CMD_ACK);

	return 1; //команда выполнена.
}
//*******************************************************************************************
// Function    : _cmd_CMD_BOOT_GetVersion()
// Description : выполнение команды CMD_BOOT_GetVersion - Получает версию загрузчика.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_GetVersion(void){

	//Передача ACK
	_sendByte(CMD_ACK);

	//Передача версии загрузчика
	_sendByte(BOOT_I2C_VERSION);

	//Передача ACK
	_sendByte(CMD_ACK);

	return 1; //команда выполнена.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_GetID()
// Description : выполнение команды CMD_BOOT_GetID - Получает идентификатор чипа
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_GetID(void){

	//Передача ACK
	_sendByte(CMD_ACK);

	//Передача кадра данных - идентификатор чипа
	//STM32F103CBT6 - Medium-density - PID = 0x410
	responseBuf[0] = 1;		//N = the number of bytes-1 (for STM32, N = 1), except for current byte and ACKs
	responseBuf[1] = 0x04;	//product ID MSB
	responseBuf[2] = 0x10;	//product ID LSB
	_setTxSize(3);			//3 байта на передачу
	_waitEndTx();			//Ждем завершения передачи ответа

	//Передача ACK
	_sendByte(CMD_ACK);

	return 1; //команда выполнена.
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
	if(_get_ROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//ROP отключена
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) with checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Контрольная сумма не совпала
	if(_getXorChecksum(rxFrame, 5) != 0)
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK
	startAddr = _get_StartAddress(rxFrame);	//Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Send data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	//bootBuf[0] = (uint8_t)(size - 1);		  //The number of bytes to be read-1
	//bootBuf[1] = (uint8_t)(size - 1) ^ 0xFF;//Checksum

	//Контрольная сумма не совпала
	if(_getXorChecksum(rxFrame, 2) != 0xFF)
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK
	size = rxFrame[0] + 1;	//Сохраняем количество передаваемых данных
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Send data frame: requested data to the host
	STM32_Flash_ReadBufU8((void*)startAddr, (void*)responseBuf, size);//копируем нужное кол-во байтов в буфер передачи
	_setTxSize(size);	//кол-во байт на передачу
	_waitEndTx();		//Ждем завершения передачи ответа
	//-----------------------
	return 1; //команда выполнена.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_Go()
// Description : Переход к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
// Parameters  : None
// RetVal      :1 - команда выполнена; 0 - команда не выполнена.
//*****************************************
uint8_t _cmd_BOOT_Go(void){

	//Хост отправляет байты на STM32 следующим образом:

	//- Byte 1: 0x21
	//- Byte 2: 0xDE
	//Wait for ACK

	//- Byte 3 to byte 6: start address
	//- Byte 3: MSB
	//- Byte 6: LSB
	//- Byte 7: checksum: XOR (byte 3, byte 4, byte 5, byte 6)
	//Wait for ACK

	uint32_t appAddr;
	//-----------------------
	//Проверка ROP (Readout Protect) и передача ACK/NACK
	if(_get_ROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//ROP отключена
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) and checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Контрольная сумма не совпала
	if(_getXorChecksum(rxFrame, 5) != 0)
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK. Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	appAddr = _get_StartAddress(rxFrame);

	//Application start address
	if(appAddr == APP_PROGRAM_START_ADDR)
	{
		//по стартовому адресу приложения должно лежать значение вершины стека
		//приложения, т.е. значение больше 0x2000 0000. Если это не так, значит
		//приложение отсутсвует
		uint32_t var = STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR);
		if((var ^ 0x2000000) > 0)
		{
			_sendByte(CMD_ACK);//Передача ACK
			_goToApp(appAddr);	//Переход на функцию Reset_Handler приложения
		}
	}
	//-----------------------
	_sendByte(CMD_NACK);	//Передача NACK
	return 0; 				//команда не выполнена.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_WM(void)()
// Description : Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_BOOT_WM(void){

	//Хост отправляет байты на STM32 следующим образом:

	//- Byte 1: 0x31
	//- Byte 2: 0xCE
	//Wait for ACK

	//- Byte 3 to byte 6: Start address (Byte 3: MSB, Byte 6: LSB)
	//- Byte 7: Checksum: XOR (byte 3, byte 4, byte 5, byte 6)
	//Wait for ACK

	//- Byte 8: Number of bytes to be received (0 < N <= 255)
	//- N+1 data bytes: (max 256 bytes)
	//- Checksum byte: XOR (N, N+1 data bytes)
	//Wait for ACK

	uint32_t startAddr;
	uint32_t nBytesMinusOne;
	//-----------------------
	//Проверка ROP (Readout Protect) и передача ACK/NACK
	if(_get_ROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//ROP отключена
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) with checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Контрольная сумма не совпала
	if(_getXorChecksum(rxFrame, 5) != 0)
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//Контрольная сумма OK. Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	startAddr = _get_StartAddress(rxFrame);
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame:
	//- number of bytes to be written
	//- data to be written
	//- checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	nBytesMinusOne = rxFrame[0]; //N - количество записываемых байт минус 1 (количество принимаемых байтов = N+1),

	//Контрольная сумма не совпала
	//N количество записываемых байт минус 1;
	//N+1 байтов данных
	//байт Сhecksum
	if(_getXorChecksum(rxFrame, (nBytesMinusOne+1+1+1)) != 0)//+3 т.к.+(N+1)байтов_данных +байт_N и +байт Сhecksum
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//Контрольная сумма OK. Проверка принятого адреса на валидность.
	//Адреса могут быть:
	// - Main memory   			- 0x0800 0000 - 0x0801 FFFF (128 KB)
	// - System memory 			- 0x1FFF F000 - 0x1FFF F7FF (2 KB)
	// - Option Bytes  			- 0x1FFF F800 - 0x1FFF F80F (16 bytes)
	// - Flash memory registers - 0x4002 2000 - 0x4002 2023 (36 bytes)
	// - Peripheral				- 0x4000 0000 - 0x4FFF FFFF

	//Main memory
	if(startAddr >= APP_PROGRAM_START_ADDR)
	{
		//Записываем полученные данные во флэш-память со стартового адреса
		STM32_Flash_Unlock();
		STM32_Flash_WriteBuf((void*)&rxFrame[1], (void*)startAddr, nBytesMinusOne+1);
		STM32_Flash_Lock();
	}
	//Option Bytes
	else if(startAddr >= 0x1FFFF800 && startAddr < 0x1FFFF80F)
	{
		//ToDo .... Data written in Option bytes ?
		//Если команда Write Memory производит запись в область байтов
		//управления (the option byte area), то все байты в этой области
		//будут стерты перед записью новых значений, и по завершению
		//команды загрузчик генерирует системный сброс, чтобы установки
		//новых значений байт опций вступили в силу.

		//_sendByte(CMD_ACK);//Передача ACK
		//NVIC_SystemReset();//Cистемный сброс!!!
	}
	//Неверный адрес
	else
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//Передача ACK
	_sendByte(CMD_ACK);
	//-----------------------
	return 1; //команда выполнена.
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
	I2c.i2c		  = BOOT_I2C;			//используемый порт I2C
	I2c.i2cMode	  = I2C_MODE_SLAVE;		//режим SLAVE
	I2c.gpioRemap = I2C_GPIO_NOREMAP;	//нет ремапа ножек порта I2C
	I2c.i2cSpeed  = BOOT_I2C_SPEED;		//скорость работы порта I2C
	I2c.slaveAddr = BOOT_I2C_ADDR;		//адрес устройства на нине I2C
	I2c.rxBufSize = I2C_IT_RX_BUF_SIZE_DEFAULT;
	I2c.txBufSize = I2C_IT_TX_BUF_SIZE_DEFAULT;
	I2c.i2cSlaveRxCpltCallback = _bootLoader_CmdParsing;
	I2c.i2cSlaveTxCpltCallback = _bootLoader_EndOfTransmission;
	I2C_IT_Init(&I2c);

	//Прерывания I2C для работы протакола - приоритет = 1
	//NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
	//NVIC_SetPriority(I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
}
//**********************************************************
void BOOT_LOADER_Loop(void){

	static uint8_t currentCmd = 0;
	//----------------------------------------
	//Ждем завершения приема кадра команды (Command frame) от хоста.
	_waitEndRx();

	//Кадр команды (Command frame) от хоста состояит из кода команды и инверсии кода команды.
	//После чего хост ждет ACK или NACK.
	if(currentCmd == 0)
	{
		//Проверка команды.
		   currentCmd = ~rxFrame[0];	//0-й байт - код команды
		if(currentCmd == rxFrame[1])	//1-й байт - инверсия кода команды
		{
			currentCmd = ~currentCmd; 	//запоминаем принятую команду и переходим к ее выполнению.
			LED_PC13_Toggel();			//мигнем светодиодом
		}
		//Неверная команда
		else
		{
			_sendByte(CMD_NACK);	//передаем NACK
			currentCmd = 0; 		//0 - команда битая.
			rxFrame[0] = 0;	   	 	//Сброс приемного буфера
			rxFrame[1] = 0;
			return;
		}
	}
	//----------------------------------------
	//Выполнение принятой команды
	switch(currentCmd)
	{
		//-------------------
	 	//Передача версии загрузчика и поддерживаемые команды.
		case(CMD_BOOT_Get):
			_cmd_BOOT_Get();
			currentCmd = 0;	//команда отработана.
		break;
		//-------------------
		//Передача версии загрузчика.
		case(CMD_BOOT_GetVersion):
			_cmd_BOOT_GetVersion();
			currentCmd = 0;	//команда отработана.

			//if(_cmd_BOOT_GetVersion()) currentCmd = 0;//0 - команда отработана.
		break;
		//-------------------
		//Передача идентификатора чипа
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
		//Переход к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
		case(CMD_BOOT_Go):
			_cmd_BOOT_Go();
			currentCmd = 0;	//команда отработана.
		break;
		//-------------------
		//Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
		case(CMD_BOOT_WM):
			_cmd_BOOT_WM();
			currentCmd = 0;	//команда отработана.

			//if(_cmd_BOOT_WM()) cmd = 0;//0 - команда отработана.
		break;
		//-------------------
		case(CMD_BOOT_NS_WM):
		//-------------------
		case(CMD_BOOT_Erase):
		//-------------------
		case(CMD_BOOT_NS_Erase):
		//----------------------------------------
		//Передаем NACK на незнакомую команда
		default:
			//LED_PC13_Toggel();
			currentCmd = 0;
			_sendByte(CMD_NACK);	//передаем NACK
		break;
		//-------------------
	}
	//----------------------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
