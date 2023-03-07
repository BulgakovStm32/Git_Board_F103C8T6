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
static I2C_IT_t	I2c;					//Рабочий I2C

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
// Function    : _getROPState()
// Description :
// Parameters  :
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
	bootBuf[0]	= (9 - 1);			//N = Number of bytes to follow - 1, except current and ACKs
	bootBuf[1]	= BOOT_I2C_VERSION;	//bootloader version
	//list of supported commands
	bootBuf[2] 	= CMD_BOOT_Get;
	bootBuf[3] 	= CMD_BOOT_GetVersion;
	bootBuf[4] 	= CMD_BOOT_GetID;
	bootBuf[5] 	= CMD_BOOT_RM;
	bootBuf[6] 	= CMD_BOOT_GO;
	bootBuf[7] 	= CMD_BOOT_WM;
	bootBuf[8] 	= CMD_BOOT_ERASE;
	bootBuf[9] 	= CMD_BOOT_NS_GetCheckSum;

//	bootBuf[10] = CMD_BOOT_NS_Erase;
//	bootBuf[11] = CMD_BOOT_WP;
//	bootBuf[12] = CMD_BOOT_NS_WP;
//	bootBuf[13] = CMD_BOOT_WUP;
//	bootBuf[14] = CMD_BOOT_NS_WUP;
//	bootBuf[15] = CMD_BOOT_RP;
//	bootBuf[16] = CMD_BOOT_NS_RP;
//	bootBuf[17] = CMD_BOOT_RUP;
//	bootBuf[18] = CMD_BOOT_NS_RUP;
//	bootBuf[19] = CMD_BOOT_NS_GetCheckSum;
	_sendBuf(10); //передадим ... байт

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
uint8_t _cmd_GO(void){

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
			_sendByte(CMD_ACK);	//Передача ACK
			//---------------
			BOOT_ErasePageAppState();				//Сбросим признак запуска основного приложения
			BOOT_SetAppLaunch(APP_OK_AND_START);	//Установим признак запуска приложения после ресета
			BOOT_CalcAndWriteAppCrc();				//Расчет и запись во флеш CRC приложения
			BOOT_GoToApp(appAddr);			 		//Переход на функцию Reset_Handler приложения
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
		STM32_Flash_ErasePage(pageAddr);							//Стирание одной страницы flash
	}
	STM32_Flash_Lock();
	//Передача ACK
	_sendByte(CMD_ACK);
	//-----------------------
	return 1; //команда выполнена.
}
//*******************************************************************************************
// Function    : _cmd_NS_GetMemCs --- не отлажена!!!!!!
// Description : Получает значение контрольной суммы CRC для области памяти.
// Parameters  : None
// RetVal      : 1 - команда выполнена; 0 - команда выполняется.
//*****************************************
uint8_t _cmd_NS_GetCheckSum(void){

	uint32_t addr;
	uint32_t size;
	uint32_t crc;
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
	//Receive address (4 bytes, MSB first) and its XOR
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	addr = _getStartAddress(bootBuf);	//Соберем адрес.

	//Address valid, within the memory, and XOR OK?
	if(_getChecksum(bootBuf, 5) != 0 ||	//Контрольная сумма не совпала
	   addr < FLASH_BASE)				//Неверный адрес
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Контрольная сумма OK и верный адрес
	_sendByte(CMD_ACK);				  	//Передача ACK
	//-----------------------
	//Receive memory size to check (4 bytes, MSB first) and its XOR
	_waitEndRx();	//Ждем завершения приема фрейма данных от хоста.

	size = _getStartAddress(bootBuf);	//Соберем размер.

	//XOR OK, Size != 0 and multiple of 4, (Address + Size) valid ?
	if(_getChecksum(bootBuf, 5) != 0 ||			//Контрольная сумма не совпала
	    size == 0					 || 		//размер = 0
	   (size % 4) != 0				 || 		//некратно 4м
	   (addr + size) > (FLASH_BASE+128*1024)) 	//Address+Size больше размера флеш памяти
	{
		_sendByte(CMD_NACK);	//Передача NACK
		return 0; 				//команда не выполнена.
	}
	//Все ОК!
	_sendByte(CMD_ACK);	//Передача ACK
	//-----------------------
	//Calculate checksum. Operation completed? Передача BUSY при длительном расчете CRC
	bootBuf[0] = CMD_BUSY;		//при запросе состояния от Хоста передадим ему состояния BUSY
	I2C_IT_SetDataSize(&I2c, 1);//
	//Сброс счетчика CRC
	CRC->CR |= CRC_CR_RESET;
	//Считаем CRC
	for(uint32_t i=0; i < size; i+=4)
	{
		CRC->DR = *(uint32_t*)(addr + i);
		//----------
		//Пока считаем CRC пердаем хосту состояние BUSY
		if(I2c.state == I2C_IT_STATE_NAC)//Хост вычитал ответ, сформируем новый
		{
			I2c.state = I2C_IT_STATE_READY;	//Сброс состояния I2C
			bootBuf[0] = CMD_BUSY;			//передаем BUSY
			I2C_IT_SetDataSize(&I2c, 1);	//1 байт на передачу
		}
		//----------
	}
	//Сохраним CRC и передадим ACK
	crc = CRC->DR;
	_sendByte(CMD_ACK);
	//-----------------------
	//Send calculated memory checksum (4 bytes, MSB first) + XOR byte
	bootBuf[0] = (uint8_t)(crc >> 24);
	bootBuf[1] = (uint8_t)(crc >> 16);
	bootBuf[2] = (uint8_t)(crc >> 8);
	bootBuf[3] = (uint8_t)(crc >> 0);
	bootBuf[4] = _getChecksum(bootBuf, 4);
	_sendBuf(5);	//кол-во байтов на передачу
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
// Function   : BOOT_Init()
// Description: инициализация загрузчика
// Parameters :
// RetVal     :
//*****************************************
void BOOT_Init(void){

	//Инициализация I2C Slave для работы по прерываниям.
	I2c.i2c		  = BOOT_I2C;			//используемый порт I2C
	I2c.i2cMode	  = I2C_MODE_SLAVE;		//режим SLAVE
	I2c.gpioRemap = I2C_GPIO_NOREMAP;	//нет ремапа ножек порта I2C
	I2c.i2cSpeed  = BOOT_I2C_SPEED;		//скорость работы порта I2C
	I2c.slaveAddr = BOOT_I2C_ADDR;		//адрес устройства на нине I2C
	I2C_IT_Init(&I2c);
	I2C_IT_SetpBuf(&I2c, bootBuf);		//Прикрепим буфер загрузчика к обработчику прерываний I2C

	//Включаем модуль CRC
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
}
//**********************************************************
// Function   : AppAvailableCheck()
// Description: по стартовому адресу приложения должно лежать значение вершины стека
//			    приложения, т.е. значение больше 0x2000 0000. Если это не так, значит
//				приложение отсутсвует
// Parameters : Нет
// RetVal     : 1 - приложение есть; 0 - приложения нет.
//*****************************************
uint32_t BOOT_AppAvailableCheck(uint32_t appAddr){

	if((STM32_Flash_ReadWord(appAddr) & 0x2FFC0000) != 0x20000000) return 0;
	return 1;
}
//**********************************************************
// Function   : BOOT_GetAppSize()
// Description: подсчет размера приложения, в байтах
// Parameters : Нет
// RetVal     : рахмер приложения в байтах
//*****************************************
uint32_t BOOT_GetAppSize(void){

	//Проверка наличия приложения. По стартовому адресу приложения должно
	//лежать значение вершины стека приложения, т.е. значение больше 0x2000 0000.
	//Если это не так, значит приложение отсутсвует
	if((STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR) & 0x2FFC0000) != 0x20000000) return 0;

	//Подсчитаем сколько байт занимает приложение.
	uint32_t appSize_Bytes = 0;
	while(STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR + appSize_Bytes) != 0xFFFFFFFF)
	{
		appSize_Bytes += 4; //шагаем по 4 байта
	}

	//в appSize_Bytes лежит размер приложения в байтах.
	//Округлим в большую сторону и сделаем кратным 4м
	//while((appSize_Bytes % 4) != 0) { appSize_Bytes++; }
	//appSize_Bytes = appSize_Bytes % 4; //проверка

	return appSize_Bytes;
}
//**********************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
uint32_t BOOT_GetStateI2C(void){

	return I2c.state;
}
//**********************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      :
//*****************************************
void BOOT_GoToApp(uint32_t appAddr){

	//Проверка наличия приложения. По стартовому адресу приложения должно
	//лежать значение вершины стека приложения, т.е. значение больше 0x2000 0000.
	//Если это не так, значит приложение отсутсвует
	if((STM32_Flash_ReadWord(appAddr) & 0x2FFC0000) != 0x20000000) return;

	//Глобальное отключение прерываний.
	__disable_irq();

	//Отключение периферии.
	//I2C_IT_DeInit(&I2c);

	//Reset the RCC clock configuration to the default reset state(for debug purpose).
	RCC->CR	  |= (uint32_t)0x00000001;	/* Set HSION bit */
	RCC->CFGR &= (uint32_t)0xF0FF0000;	/* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
	RCC->CR   &= (uint32_t)0xFEF6FFFF;	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR   &= (uint32_t)0xFFFBFFFF;	/* Reset HSEBYP bit */
	RCC->CFGR &= (uint32_t)0xFF80FFFF;	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
	RCC->CIR   = 0x009F0000;  			/* Disable all interrupts and clear pending bits  */

	RCC->APB1RSTR = 0xFFFFFFFF;	//Сбрасываем всю периферию на APB1
	RCC->APB1RSTR = 0x0;

	RCC->APB2RSTR = 0xFFFFFFFF;	//Сбрасываем всю периферию на APB2
	RCC->APB2RSTR = 0x0;

	RCC->APB1ENR  = 0x0; 		//Выключаем всю периферию на APB1
	RCC->APB2ENR  = 0x0; 		//Выключаем всю периферию на APB2
	RCC->AHBENR   = 0x0; 		//Выключаем всю периферию на AHB

	//Переход на приложение
	uint32_t addrResetHandler = *(__IO uint32_t*)(appAddr + 4);	//Адрес функции Reset_Handler приложения
	void(*app)(void) = (void(*)(void))addrResetHandler; 		//Указатель на функцию Reset_Handler приложения
	SCB->VTOR = appAddr; 										//Адрес таблицы прерываний
	__set_MSP(*(__IO uint32_t*)appAddr);            			//Устанавливаем указатель стека SP приложения
	app();								   						//Переход на функцию Reset_Handler приложения
}
//**********************************************************
// Function      BOOTLOADER_SetAppLaunch()
// Description   установить условие запуска приложения
// Parameters    None
// RetVal        None
//*****************************************
void BOOT_SetAppLaunch(uint32_t launch){

	STM32_Flash_Unlock();
	STM32_Flash_WriteWord(APP_LAUNCH_CONDITIONS_ADDR, launch);
	STM32_Flash_Lock();
}
//**********************************************************
// Function      BOOTLOADER_GetAppLaunch()
// Description   получение условие запуска приложения
// Parameters    None
// RetVal        None
//*****************************************
uint32_t BOOT_GetAppLaunch(void){

	return STM32_Flash_ReadWord(APP_LAUNCH_CONDITIONS_ADDR);
}
//**********************************************************
// Function      BOOTLOADER_CalcAppCrc()
// Description   расчет контрольной суммы записанного приложения
// Parameters	 addr - адрес (кратен 4м) области, с которого начинаем считать CRC
//				 size - размер (кратен 4м) области, для которой считаем CRC
// RetVal        crc области данных
//*****************************************
uint32_t BOOT_CalcCrc(uint32_t *addr, uint32_t size){

	//uint16_t Get_Crc16(uint8_t *buf, uint8_t len){

	//  //uint8_t  i;
	//  //--------------------
	//  CRC->CR  |= CRC_CR_RESET; //Делаем сброс...
	//  //for(i = 0; i < len; i++)
	//	while (len--)
	//    {
	//      CRC->DR = (uint32_t)(*buf & 0x000000FF); //Загоняем данные из буфера в регистр данных
	//    }
	//  return (uint16_t)(CRC->DR & 0x0000FFFF); //Читаем контрольную сумму и возвращаем её
	//}

	//Сброс счетчика CRC
	CRC->CR |= CRC_CR_RESET;
	//Считаем CRC
	for(uint32_t i=0; i < size; i+=4)
	{
		CRC->DR = *(uint32_t*)(APP_PROGRAM_START_ADDR + i);
	}
	//Читаем контрольную сумму и возвращаем её
	return CRC->DR;
}
//**********************************************************
// Function      BOOTLOADER_ReadAppCrc()
// Description	 чтение из флэша записанной ране контрольной
//				 суммы приложения
// Parameters
// RetVal        crc приложения
//*****************************************
uint32_t BOOT_ReadAppCrc(void){

	return STM32_Flash_ReadWord(APP_CRC_ADDR);
}
//**********************************************************
// Function      BOOT_CalcAndWriteAppCrc()
// Description	 расчет и запись во флеш контрольной суммы приложения
// Parameters
// RetVal
//*****************************************
void BOOT_CalcAndWriteAppCrc(void){

	uint32_t appSize = BOOT_GetAppSize();
	uint32_t appCrc  = BOOT_CalcCrc((uint32_t*)APP_PROGRAM_START_ADDR ,appSize);

	//запись во флеш контрольной суммы приложения
	STM32_Flash_Unlock();
	STM32_Flash_WriteWord(APP_CRC_ADDR, appCrc);
	STM32_Flash_Lock();
}
//**********************************************************
// Function      BOOTLOADER_ErasePageAppState()
// Description   Очистка страницы памяти (1024байта) со структурой
//				 состояния приложения
// Parameters    None
// RetVal        None
//*****************************************
void BOOT_ErasePageAppState(){

	STM32_Flash_Unlock();
	STM32_Flash_ErasePage(APP_STATE_ADDR);
	STM32_Flash_Lock();
}
//**********************************************************
uint32_t BOOT_Loop(void){

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
		case(CMD_BOOT_GO):
			_cmd_GO();
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
		case(CMD_BOOT_NS_GetCheckSum):
			_cmd_NS_GetCheckSum();
		break;
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








