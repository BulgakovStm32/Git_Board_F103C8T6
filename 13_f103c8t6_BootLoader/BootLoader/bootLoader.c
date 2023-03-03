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
static uint8_t 	bootBuf[1024+16] = {0};	//Рабочий буфер загрузчика
static I2C_IT_t I2c;					//Рабочий I2C

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _waitEndRx(void){

//	if(I2c.state != I2C_IT_STATE_STOP) return 1;//Ждем завершения чтения ответа
//	I2c.state = I2C_IT_STATE_READY;
//	return 0;

	uint32_t micros = DELAY_microSecCount();
	//------------------
	while(I2c.state != I2C_IT_STATE_STOP)//Ждем завершения чтения ответа
	{
		//Мигаем...
		if((DELAY_microSecCount() - micros) >= 500000)
		{
			micros = DELAY_microSecCount();
			LED_PC13_Toggel();
		}
	}
	I2c.state = I2C_IT_STATE_READY;
}
//*******************************************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
static void _sendBuf(uint32_t size){

	I2C_IT_SetDataSize(&I2c, size);			//кол-во байт на передачу
	while(I2c.state != I2C_IT_STATE_NAC);	//Ждем завершения передачи кадра данных
	I2c.state = I2C_IT_STATE_READY;			//Сброс состояния I2C
}
//*******************************************************************************************
// Function    : _send_Byte
// Description : отправить ответ мастеру
// Parameters  : byte - код ответа
// RetVal      :
//*****************************************
static void _sendByte(uint8_t byte){

	bootBuf[0] = byte;
	I2C_IT_SetDataSize(&I2c, 1);			//1 байт на передачу
	while(I2c.state != I2C_IT_STATE_NAC);	//Ждем завершения передачи кадра данных
	I2c.state = I2C_IT_STATE_READY;			//Сброс состояния I2C
}
//*******************************************************************************************
// Function    : _getChecksum()
// Description : расчет контрольной суммы. Полученные блоки байтов данных подвергаются операции XOR.
//			     Байт, содержащий вычисленное XOR всех предыдущих байтов, добавляется в конец каждого
//				 сообщения (байт контрольной суммы). При выполнении XOR всех полученных байтов данных и
//				 контрольной суммы результат в конце пакета должен быть 0x00.
// Parameters  : buf  - указатель на буфер
//				 size - кол-во байтов для расчета, от 1 байта
// RetVal      : XOR байтов
//*****************************************
static uint8_t _getChecksum(uint8_t *buf, uint32_t size){

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
// Function    : _getStartAddress()
// Description : собирает 4-хбайтовый адрес из принятых байтов
// Parameters  : buf - указатель на младший байт принятого адреса
// RetVal      : 4-хбайтовый адрес
//*****************************************
static uint32_t _getStartAddress(uint8_t *buf){

	return (uint32_t)((buf[0]<<24) |
			  	  	  (buf[1]<<16) |
					  (buf[2]<< 8) |
					  (buf[3]<< 0));
}
//*******************************************************************************************
// Function    : _cleareRxFrame()
// Description : очистка первых байтов приемного буфера
// Parameters  : None
// RetVal      :
//*****************************************
static void _cleareRxFrame(void){

	bootBuf[0] = 0;
	bootBuf[1] = 0;
}
//*******************************************************************************************
// Function    : _cmd_CMD_BOOT_GetVersion()
// Description : заглушка
// Parameters  : None
// RetVal      :
//*****************************************
static uint8_t _getROPState(void){

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
uint8_t _cmd_Get(void){

	//Передача ACK
	_sendByte(CMD_ACK);

	//Сборка кадра данных
	bootBuf[0]	= 19;			//N = Number of bytes to follow - 1, except current and ACKs
	bootBuf[1]	= 0x01;			//bootloader version
	bootBuf[2] 	= CMD_BOOT_Get;	//list of supported commands
	bootBuf[3] 	= CMD_BOOT_GetVersion;
	bootBuf[4] 	= CMD_BOOT_GetID;
	bootBuf[5] 	= CMD_BOOT_RM;
	bootBuf[6] 	= CMD_BOOT_Go;
	bootBuf[7] 	= CMD_BOOT_WM;
	bootBuf[8] 	= CMD_BOOT_NS_WM;
	bootBuf[9] 	= CMD_BOOT_ERASE;
	bootBuf[10] = CMD_BOOT_NS_Erase;
	bootBuf[11] = CMD_BOOT_WP;
	bootBuf[12] = CMD_BOOT_NS_WP;
	bootBuf[13] = CMD_BOOT_WUP;
	bootBuf[14] = CMD_BOOT_NS_WUP;
	bootBuf[15] = CMD_BOOT_RP;
	bootBuf[16] = CMD_BOOT_NS_RP;
	bootBuf[17] = CMD_BOOT_RUP;
	bootBuf[18] = CMD_BOOT_NS_RUP;
	bootBuf[19] = CMD_BOOT_NS_GetMemCs;

	_sendBuf(20); //передадим 20 байт

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
uint8_t _cmd_GetVersion(void){

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
uint8_t _cmd_GetID(void){

	//Передача ACK
	_sendByte(CMD_ACK);

	//Передача кадра данных - идентификатор чипа
	//STM32F103CBT6 - Medium-density - PID = 0x410
	bootBuf[0] = 1;		//N = the number of bytes-1 (for STM32, N = 1), except for current byte and ACKs
	bootBuf[1] = 0x04;	//product ID MSB
	bootBuf[2] = 0x10;	//product ID LSB
	_sendBuf(3);		//3 байта на передачу

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
uint8_t _cmd_RM(void){

	uint32_t startAddr;
	uint32_t size;
	//-----------------------
	//Проверка ROP (Readout Protect) и передача ACK/NACK
	if(_getROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//ROP отключена
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) with checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Проверка Сhecksum
	if(_getChecksum(bootBuf, 5) != 0)//Контрольная сумма не совпала
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK
	startAddr = _getStartAddress(bootBuf);//Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	_sendByte(CMD_ACK);					  //Передача ACK
	//-----------------------
	//Receive data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Send data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	//bootBuf[0] = (uint8_t)(size - 1);		  //The number of bytes to be read-1
	//bootBuf[1] = (uint8_t)(size - 1) ^ 0xFF;//Checksum

	//Проверка Сhecksum
	if(_getChecksum(bootBuf, 2) != 0xFF)//Контрольная сумма не совпала
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK
	size = bootBuf[0] + 1;	//Сохраняем количество передаваемых данных
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Send data frame: requested data to the host
	STM32_Flash_ReadBufU8((void*)startAddr, (void*)bootBuf, size);//копируем нужное кол-во байтов в буфер передачи

	_sendBuf(size);//кол-во байт на передачу
	//-----------------------
	return 1; //команда выполнена.
}
//*******************************************************************************************
// Function    : _cmd_BOOT_Go()
// Description : Переход к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
// Parameters  : None
// RetVal      :1 - команда выполнена; 0 - команда не выполнена.
//*****************************************
uint8_t _cmd_Go(void){

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
	if(_getROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//ROP отключена
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) and checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Проверка Сhecksum
	if(_getChecksum(bootBuf, 5) != 0)//Контрольная сумма не совпала
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK. Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	appAddr = _getStartAddress(bootBuf);

	//Application start address
	if(appAddr == APP_PROGRAM_START_ADDR)
	{
		//по стартовому адресу приложения должно лежать значение вершины стека
		//приложения, т.е. значение больше 0x2000 0000. Если это не так, значит
		//приложение отсутсвует
		if((STM32_Flash_ReadWord(appAddr) & 0x2FFC0000) == 0x20000000)
		{
			_sendByte(CMD_ACK);			//Передача ACK

			//TODO... Сделать провреку CRC приложения

			//---------------
			BOOTLOADER_ResetAppState();				 //Сбросим признак запуска основного приложения
			BOOTLOADER_SetAppState(APP_OK_AND_START);//Установим признак запуска приложения после ресета
			BOOTLOADER_GoToApp(appAddr);			 //Переход на функцию Reset_Handler приложения
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
uint8_t _cmd_WM(void){

	uint32_t startAddr;
	uint32_t nBytesMinusOne;
	//-----------------------
	//Проверка ROP (Readout Protect) и передача ACK/NACK
	if(_getROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//ROP отключена
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame: start address (4 bytes) with checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Проверка Сhecksum
	if(_getChecksum(bootBuf, 5) != 0)//Контрольная сумма не совпала
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//Контрольная сумма OK. Соберам принятый адрес. Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	startAddr = _getStartAddress(bootBuf);
	_sendByte(CMD_ACK);		//Передача ACK
	//-----------------------
	//Receive data frame:
	//- number of bytes to be written
	//- data to be written
	//- checksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	nBytesMinusOne = bootBuf[0]; //N - количество записываемых байт минус 1 (количество принимаемых байтов = N+1),
	//Проверка Сhecksum
	//+3 т.к. +байт_N +(N+1)байтов_данных и +байт Сhecksum
	if(_getChecksum(bootBuf, (nBytesMinusOne+1+1+1)) != 0)//Контрольная сумма не совпала
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
		STM32_Flash_WriteBuf((void*)&bootBuf[1], (void*)startAddr, nBytesMinusOne+1);
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
// Function    : _cmd_ERASE() --- не отлажена!!!!!!
// Description : Стирает от одной до всех страниц или секторов флэш-памяти,
//				 используя режим двухбайтовой адресации.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_ERASE(void){

	uint32_t pageAddr;
	uint32_t nPages;
	//-----------------------
	//Проверка ROP (Readout Protect) и передача ACK/NACK
	if(_getROPState() == 1)//ROP активна
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//ROP отключена. Передача ACK
	_sendByte(CMD_ACK);
	//-----------------------
	//Receive data frame:
	//2 байта - кол-во страниц или секторов, которые необходимо стереть минус 1
	//1 байт  - Сhecksum
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Проверка Сhecksum
	if(_getChecksum(bootBuf, 3) != 0)//Контрольная сумма не совпала
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//Контрольная сумма OK. Сохраним кол-во страниц и передадим ACK
	nPages = (uint32_t)((bootBuf[0]<<8) | bootBuf[1])+1;//кол-во страниц
	_sendByte(CMD_ACK);
	//-----------------------
	//- в случае стирания N страниц или секторов (это наш случай) загрузчик получает (2 x N) байтов,
	//каждое полуслово которых содержит номер страницы или сектора, закодированный двумя байтами,
	//причем старший бит идет первым. Затем передается байт контрольгой суммы. Если контрольная сумма верна,
	//загрузчик стирает память и отправляет хосту байт ACK, в противном случае он отправляет байт NACK хосту и команда прерывается.
	//Example of I2C frame:
	//– erase page 1: 0x44 0xBB Wait ACK 0x00 0x00 0x00 Wait ACK 0x00 0x01 0x01 Wait ACK
	//– erase page 1 and page 2:
	//0x44 0xBB Wait ACK 0x00 0x01 0x01 Wait ACK 0x00 0x01 0x00 0x02 0x03 Wait ACK

	//Принимаем номера страниц и Сhecksum.
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	//Проверка Сhecksum
	if(_getChecksum(bootBuf, nPages*2+1) != 0)//Контрольная сумма не совпала
	{
		_sendByte(CMD_NACK);//Передача NACK
		return 0; 			//команда не выполнена.
	}
	//Контрольная сумма OK. Стираем страницы памяти.
	STM32_Flash_Unlock();
	for(uint32_t i=0; i < nPages; i++)
	{
		pageAddr = (uint32_t)((bootBuf[i*2]<<8) | bootBuf[i*2+1]);	//Номер стираемой страницы
		pageAddr = pageAddr & 0x000000FF;							//Маска
		pageAddr = pageAddr * 1024;									//смещение страницы относительно базового адреса
		pageAddr = pageAddr + FLASH_BASE;							//адрес стираемой страницы
		//STM32_Flash_ErasePage(pageAddr);							//Стирание одной страницы flash
	}
	STM32_Flash_Lock();
	//Передача ACK
	_sendByte(CMD_ACK);
	//-----------------------
	return 1; //команда выполнена.
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void I2C_IT_SlaveRxCpltCallback(I2C_IT_t *i2cIt){

	LED_PC13_Toggel();	//мигнем светодиодом
}
//************************************************************
void I2C_IT_SlaveTxCpltCallback(I2C_IT_t *i2cIt){

}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void BOOTLOADER_Init(void){

	//Инициализация I2C Slave для работы по прерываниям.
	I2c.i2c		  = BOOT_I2C;			//используемый порт I2C
	I2c.i2cMode	  = I2C_MODE_SLAVE;		//режим SLAVE
	I2c.gpioRemap = I2C_GPIO_NOREMAP;	//нет ремапа ножек порта I2C
	I2c.i2cSpeed  = BOOT_I2C_SPEED;		//скорость работы порта I2C
	I2c.slaveAddr = BOOT_I2C_ADDR;		//адрес устройства на нине I2C
	I2C_IT_Init(&I2c);

	//Прикрепим буфер загрузчика к обработчику прерываний I2C
	I2C_IT_SetpBuf(&I2c, bootBuf);
}
//**********************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
uint32_t BOOTLOADER_GetStateI2C(void){

	return I2c.state;
}
//**********************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
void BOOTLOADER_GoToApp(uint32_t appAddr){

	//Проверка наличия приложения.
	//По стартовому адресу приложения должно лежать значение вершины стека
	//приложения, т.е. значение больше 0x2000 0000. Если это не так, значит
	//приложение отсутсвует
	if((STM32_Flash_ReadWord(appAddr) & 0x2FFC0000) != 0x20000000) return;

	//Глобальное отключение прерываний.
	__disable_irq();

	//Отключение периферии.
	I2C_IT_DeInit(&I2c);

	//Переход на приложение
	uint32_t addrResetHandler = *(__IO uint32_t*)(appAddr + 4);	//Адрес функции Reset_Handler приложения
	void(*app)(void) = (void(*)(void))addrResetHandler; 		//Указатель на функцию Reset_Handler приложения
	SCB->VTOR = appAddr; 										//Адрес таблицы прерываний
	__set_MSP(*(__IO uint32_t*)appAddr);            			//Устанавливаем указатель стека SP приложения
	app();								   						//Переход на функцию Reset_Handler приложения
}
//**********************************************************
// Function   : AppAvailableCheck()
// Description: по стартовому адресу приложения должно лежать значение вершины стека
//			    приложения, т.е. значение больше 0x2000 0000. Если это не так, значит
//				приложение отсутсвует
// Parameters : Нет
// RetVal     : 1 - приложение есть; 0 - приложения нет.
//*****************************************
uint32_t BOOTLOADER_AppAvailableCheck(uint32_t appAddr){

	if((STM32_Flash_ReadWord(appAddr) & 0x2FFC0000) != 0x20000000) return 0;
	return 1;
}
//**********************************************************
// Function      BOOTLOADER_SetAppState()
// Description   Sets bootloader key
// Parameters    None
// RetVal        None
//*****************************************
void BOOTLOADER_SetAppState(uint32_t state){

	STM32_Flash_Unlock();
	STM32_Flash_WriteWord(APP_CONDITION_ADDR, state);
	STM32_Flash_Lock();
}
//**********************************************************
// Function      BOOTLOADER_GetAppState()
// Description   Reads bootloader key value
// Parameters    None
// RetVal        None
//*****************************************
uint32_t BOOTLOADER_GetAppState(void){

  return STM32_Flash_ReadWord(APP_CONDITION_ADDR);
}
//**********************************************************
// Function      BOOTLOADER_ResetAppState()
// Description   Resets bootloader key
// Parameters    None
// RetVal        None
//*****************************************
void BOOTLOADER_ResetAppState(){

	STM32_Flash_Unlock();
	STM32_Flash_ErasePage(APP_CONDITION_ADDR);
	STM32_Flash_Lock();
}
//**********************************************************
uint32_t BOOTLOADER_Loop(void){

	//Ждем завершения приема кадра команды (Command frame) от хоста.
	_waitEndRx();

	//Кадр команды (Command frame) от хоста состояит из кода команды и инверсии кода команды.
	//После чего хост ждет ACK или NACK.
	//Проверка команды. Если команда не верна то передаем NACK и выходим
	uint8_t invertCmd = invertCmd = ~bootBuf[0];//0-й байт - код команды
	if(invertCmd != bootBuf[1])				  	//1-й байт - инверсия кода команды
	{
		_sendByte(CMD_NACK);	//передаем NACK
		_cleareRxFrame();		//Сброс приемного буфера
		return 0;
	}
	//Команда верна! Обработаем команду.
	//LED_PC13_Toggel();	//мигнем светодиодом
	switch(bootBuf[0])	//Выполнение принятой команды
	{
		//-------------------
	 	//Передача версии загрузчика и поддерживаемые команды.
		case(CMD_BOOT_Get):
			_cmd_Get();
		break;
		//-------------------
		//Передача версии загрузчика.
		case(CMD_BOOT_GetVersion):
			_cmd_GetVersion();
		break;
		//-------------------
		//Передача идентификатора чипа
		case(CMD_BOOT_GetID):
			_cmd_GetID();
		break;
		//-------------------
		//Read Memory - Читает до 256 байт памяти,
		//начиная с указанного адреса.
		case(CMD_BOOT_RM):
			_cmd_RM();
		break;
		//-------------------
		//Переход к коду пользовательского приложения,
		//расположенному во внутренней флэш-памяти.
		case(CMD_BOOT_Go):
			_cmd_Go();
		break;
		//-------------------
		//Write Memory - Записывает в память до 256 байт,
		//начиная с указанного адреса.
		case(CMD_BOOT_WM):
			_cmd_WM();
		break;
		//-------------------
		//Стирает от одной до всех страниц или секторов флэш-памяти,
		//используя режим двухбайтовой адресации.
		case(CMD_BOOT_ERASE):
			_cmd_ERASE();
		break;
		//-------------------
		case(CMD_BOOT_NS_WM):
		//-------------------
		case(CMD_BOOT_NS_Erase):
		//----------------------------------------
		//Передаем NACK на незнакомую команда
		default:
			_sendByte(CMD_NACK);	//передаем NACK
			return 0;
		break;
		//----------------------------------------
	}
	//----------------------------------------
	return 1;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
