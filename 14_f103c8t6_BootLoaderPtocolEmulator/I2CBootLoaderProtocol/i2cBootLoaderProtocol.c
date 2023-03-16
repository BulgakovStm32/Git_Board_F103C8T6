/*
 * i2cBootLoaderProtocol.c
 *
 *  Created on: 10 февр. 2023 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

//#include "i2c_ST.h"
#include "i2cBootLoaderProtocol.h"

//*******************************************************************************************
//*******************************************************************************************
static uint8_t bootBuf[FLASH_PAGE_SIZE + 16] = {0};	//Рабочий буфер

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _startAndSendDeviceAddr()
// Description : формирование старт последоватльности, предача адреса Sleve и ожидание ответа
// Parameters  : rw - BOOT_I2C_Wr - SLA+Wr (запись в слейв)
//					  BOOT_I2C_Rd -	SLA+Rd (чтение из слейва)
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
static uint8_t _startAndSendDeviceAddr(uint8_t rw){

	uint32_t count = 0;
	//---------------------
	//Ждем ответа Slave
	while(I2C_StartAndSendDeviceAddr(BOOT_I2C, BOOT_I2C_ADDR|rw) != I2C_OK)
	{
		//Если через 3 попытки нет ответа от устройства, то выходим
		if(++count >= 3) return BOOT_I2C_NO_DEVICE;
	}
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : _sendBuf()
// Description : передача данных из буфера
// Parameters  : buf  - указатель на передаваемый буфер
//				 size - размер передаваемого буфера, от 1 байта
// RetVal      :
//*****************************************
static void _sendBuf(uint8_t *data, uint32_t size){

	I2C_SendDataWithStop(BOOT_I2C, data, size);
}
//*******************************************************************
// Function    : _readBuf()
// Description : прием данных в буфера
// Parameters  : buf  - указатель на приемный буфер
//				 size - кол-во байтов на прием, от 1 байта
// RetVal      :
//*****************************************
static void _readBuf(uint8_t *buf, uint32_t size){

	I2C_ReadData(BOOT_I2C, buf, size);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _host_waitACK()
// Description : Ожидание кадра ACK/NACK: Хост инициирует связь I2C в качестве главного получателя и
//				 получает от устройства один байт: ACK, NACK или BUSY.
// Parameters  :
// RetVal      : CMD_BOOT_ACK  - пакет принят   (команда выполнена)
//				 CMD_BOOT_NACK - пакет отброшен (команда не выполнена)
//				 CMD_BOOT_BUSY - состояние занятости (команда в процессе выполнения)
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
static uint8_t _host_waitACK(void){

	uint8_t byte = 0;
	//---------------------
	//ждем ответа
	if(_startAndSendDeviceAddr(BOOT_I2C_Rd) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_readBuf(&byte, 1);
	return byte;
}
//*******************************************************************
// Function    : _host_receiveData()
// Description : Получение кадра данных: Хост инициирует связь I2C в качестве главного приемника и
//				 получает ответ от устройства. Количество полученных байтов зависит от команды.
// Parameters  : buf  - указатель на приемный буфер
//				 size - кол-во байтов на прием, от 1 байта
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес и передало все данные
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
static uint8_t _host_receiveData(uint8_t *buf, uint32_t size){

	if(_startAndSendDeviceAddr(BOOT_I2C_Rd) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_readBuf(buf, size);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : _host_sendData()
// Description : отправка кадра данных: Хост инициирует связь I2C в качестве главного передатчика и
//				 отправляет необходимые байты на устройство. Количество передаваемых байтов зависит от команды.
// Parameters  : buf  - указатель на передающий буфер
//				 size - кол-во байтов на передачу, от 1 байта
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес и приняло все данные
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t _host_sendData(uint8_t *buf, uint32_t size){

	if(_startAndSendDeviceAddr(BOOT_I2C_Wr) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_sendBuf(buf, size);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : _host_getChecksum()
// Description : расчет контрольной суммы. Полученные блоки байтов данных подвергаются операции XOR.
//			     Байт, содержащий вычисленное XOR всех предыдущих байтов, добавляется в конец каждого
//				 сообщения (байт контрольной суммы). При выполнении XOR всех полученных байтов данных и
//				 контрольной суммы результат в конце пакета должен быть 0x00.
// Parameters  : buf  - указатель на буфер
//				 size - кол-во байтов для расчета, от 1 байта
// RetVal      : XOR байтов
//*****************************************
uint8_t _host_getChecksum(uint8_t *buf, uint32_t size){

	//Проверено - работает!!!
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
//*******************************************************************
// Function    :_host_sendCmdAndGetAck()
// Description :Отправить кадр команды и получить ответ. Хост инициирует
//				связь в качестве главного передатчика и отправляет на
//				устройство два байта: код_команды + инверсный_код_команды,
//				после чего читает ответ загрузчика.
// Parameters  :cmd - команда, которую хотим передать
// RetVal      :CMD_BOOT_ACK  		- пакет принят   (команда выполнена)
//				CMD_BOOT_NACK 		- пакет отброшен (команда не выполнена)
//				CMD_BOOT_BUSY 		- состояние занятости (команда в процессе выполнения)
//				BOOT_I2C_NO_DEVICE 	- устройства нет на шине I2C
//*****************************************
static uint8_t _host_sendCmdAndGetAck(uint8_t cmd){

	uint8_t buf[2] = {cmd, ~cmd};
	//---------------------
	//Передаем команду и ее инверсию
	if(_startAndSendDeviceAddr(BOOT_I2C_Wr) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE; //нет устройства на шине
	}
	_sendBuf(buf, 2);
	//Вернем ответ загрузчика.
	return _host_waitACK();
}
//*******************************************************************
// Function    :_host_sendDataAndGetAck()
// Description :Отправить кадр данных и получить ответ.
// Parameters  :data - буфер с данными на передачу
//				size - кол-во данных  на передачу
// RetVal      :CMD_BOOT_ACK  - пакет принят   (команда выполнена)
//				CMD_BOOT_NACK - пакет отброшен (команда не выполнена)
//				CMD_BOOT_BUSY - состояние занятости (команда в процессе выполнения)
//*****************************************
static uint8_t _host_sendDataAndGetAck(uint8_t *data, uint32_t size){

	//Передаим данные
	if(_host_sendData(data, size) == BOOT_I2C_NO_DEVICE) return CMD_NACK;	//нет устройства на шине
	//Вернем ответ загрузчика.
	return _host_waitACK();
}
//*******************************************************************
// Function    :
// Description :
// Parameters  :
// RetVal      : CMD_BOOT_ACK  - пакет принят   (команда выполнена)
//				 CMD_BOOT_NACK - пакет отброшен (команда не выполнена)
//				 CMD_BOOT_BUSY - состояние занятости (команда в процессе выполнения)
//*****************************************
static uint8_t _host_readDataAndGetAck(uint8_t *buf, uint32_t size){

	//Чтение данных
	if(_host_receiveData(buf, size) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//Вернем ответ загрузчика.
	return _host_waitACK();
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : BL_HOST_I2CInit()
// Description : инициализация Хоста загрузчика
// Parameters  :
// RetVal      :
//*****************************************
void BL_HOST_Init(void){

	I2C_Master_Init(BOOT_I2C, I2C_GPIO_NOREMAP, BOOT_I2C_SPEED);

	//Включаем модуль CRC
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
}
//*******************************************************************************************
//****************************КОМАНДЫ ХОСТА ЗАГРУЗЧИКА***************************************
// Function    : BL_HOST_Cmd_Get() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда позволяет узнать версию загрузчика и поддерживаемые команды.
// Parameters  : buf  - буфер приема
//				 size - кол-во вычитываемых данных
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_Get(uint8_t *buf, uint32_t size){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_Get) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Чтение списка поддерживаемых команд. 19 команд для версии загрузчика 1.1(такой в STM32F411)
	return _host_readDataAndGetAck(buf, size);
}
//*******************************************************************
// Function    : BL_HOST_Cmd_GetVersion() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда позволяет узнать версию загрузчика.
// Parameters  :
// RetVal      : bootLoaderVersion - версию загрузчика
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_GetVersion(void){

	uint8_t version = 0;
	//---------------------
	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_GetVersion) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Чтение версии загрузчика
	if(_host_readDataAndGetAck(&version, 1) != CMD_ACK) return CMD_NACK;
	return version;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_GetID() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда используется для получения версии чипа (product ID).
// Parameters  :
// RetVal      : PID - идентификатор чипа
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint16_t BL_HOST_Cmd_GetID(void){

	//Устройство STM32 отправляет байты следующим образом: - Byte 1: ACK
	//- Byte 2: N = the number of bytes-1 (for STM32, N = 1), except for current byte and ACKs
	//- Bytes 3-4: PID (product ID)
	//- Byte 3 = MSB
	//- Byte 4 = LSB
	//- Byte 5: ACK

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_GetID) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Чтение данных и проверка ответа. Чтение байта N и 2х байтов PID (product ID)
	if(_host_readDataAndGetAck(bootBuf, 3) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Вернем PID (product ID)
	return (uint16_t)((bootBuf[1] << 8) |	//MSB
			   	   	  (bootBuf[2] << 0));	//LSB;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_RM() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда используется для чтения данных из любого допустимого адреса
//				 (см. даташит на используемый STM32 и апноут AN2606 [2]) в RAM,
//				 памяти Flash и информационного блока (system memory или область байта опций).
// Parameters  : buf  - приемный буфер
//				 addr - адрес, с которого начинаем чтение
//				 size - сколько байт нужно прочитать
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_RM(uint8_t *buf, uint32_t addr, uint32_t size){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_RM) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Предадим адрес (Start address) откуда хотим прочитать данные
	bootBuf[0] = (uint8_t)(addr >> 24);
	bootBuf[1] = (uint8_t)(addr >> 16);
	bootBuf[2] = (uint8_t)(addr >> 8);
	bootBuf[3] = (uint8_t)(addr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);//Checksum
	//Передаим данные и проверим ответ
	if(_host_sendDataAndGetAck(bootBuf, 5) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Send data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	bootBuf[0] = (uint8_t)(size - 1);		//The number of bytes to be read-1
	bootBuf[1] = (uint8_t)(size - 1) ^ 0xFF;//Checksum
	//Передаим данные и проверим ответ
	if(_host_sendDataAndGetAck(bootBuf, 2) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Чтение данных - Receive data frame: needed data from the BL
	if(_host_receiveData(buf, size) == BOOT_I2C_NO_DEVICE) return CMD_NACK;
	//---------------------
	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_Go() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда используется для запуска загруженного или любого
//				 другого кода путем безусловного перехода по указанному адресу.
// Parameters  : addr - адрес перехода
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_Go(uint32_t addr){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_GO) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Предадим адрес перехода
	bootBuf[0] = (uint8_t)(addr >> 24);
	bootBuf[1] = (uint8_t)(addr >> 16);
	bootBuf[2] = (uint8_t)(addr >> 8);
	bootBuf[3] = (uint8_t)(addr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);//Checksum
	//Передаим данные и вернем результат
	return _host_sendDataAndGetAck(bootBuf, 5);
}
//*******************************************************************
// Function    : BL_HOST_Cmd_WM() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда используется для записи данных в любую допустимую
//				 область память, например в RAM, во Flash или в область байта опций.
// Parameters  : addr - адрес куда пишем (кратен 4м)
//				 data - буфер с данными на запись
//				 size - кол-во байтов на запись (от 1 до 256)(кратно 4м)
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_WM(uint32_t addr, uint8_t *data, uint32_t size){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_WM) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Передадим Start address куда хотим писать данные
	bootBuf[0] = (uint8_t)(addr >> 24);
	bootBuf[1] = (uint8_t)(addr >> 16);
	bootBuf[2] = (uint8_t)(addr >> 8);
	bootBuf[3] = (uint8_t)(addr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);	//Checksum
	//Передаим данные и проверим ответ
	if(_host_sendDataAndGetAck(bootBuf, 5) != CMD_ACK) return CMD_NACK;	//неверный Start address
	//---------------------
	//Передача кол-ва байтов N, которое нужно записать, минус 1(0 < N <= 255)
	bootBuf[0] = (uint8_t)(size-1);//-1 для того чтобы можно было передать число от 1 до 256

	//Перепишем N+1 байтов в буфер передачи
	STM32_Flash_ReadBufU8((void*)addr, (void*)&bootBuf[1], size);

	//Контрольная сумма
	bootBuf[size+1] = _host_getChecksum(bootBuf, size+1);//+1 байт потому что +байт N

	//Передаим данные. +2 байта потому что +байт N и +байт Checksum
	if(_host_sendData(bootBuf, size+2) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Подождем окончания записи
	//STM32F103: tprog - 16-bit programming time = 70μs МАКС
	//1024 байта запишется за ~4.5мс
	DELAY_milliS(5);

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;	//ошибка при записи
	//---------------------
	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_ERASE() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда позволяетстереть страницы памяти Flash STM32.
// Parameters  : numErasePages - количество страниц или секторов, которые необходимо стереть
//				 startPage     - страница или сектор, с которой начнется стирание
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_ERASE(uint16_t numErasePages, uint16_t startPage){

	uint16_t numMinusOne = numErasePages - 1;
	uint8_t  checksum = 0;
	//---------------------
	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_ERASE) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Передадим кол-во страниц или секторов, которые необходимо стереть минус 1 и Checksum
	bootBuf[0] = (uint8_t)(numMinusOne << 8);
	bootBuf[1] = (uint8_t)(numMinusOne << 0);
	bootBuf[2] = _host_getChecksum(bootBuf, 2);	//Checksum
	//Передаим данные и проверим ответ
	if(_host_sendDataAndGetAck(bootBuf, 3) != CMD_ACK) return CMD_NACK;	//неверное кол-во страниц или секторов
	//---------------------
	//- в случае стирания N страниц или секторов (это наш случай) загрузчик получает (2 x N) байтов,
	//каждое полуслово которых содержит номер страницы или сектора, закодированный двумя байтами,
	//причем старший бит идет первым. Затем передается байт контрольгой суммы.
	//Example of I2C frame:
	//– erase page 1: 0x44 0xBB Wait ACK 0x00 0x00 0x00 Wait ACK 0x00 0x01 0x01 Wait ACK
	//– erase page 1 and page 2:
	//0x44 0xBB Wait ACK 0x00 0x01 0x01 Wait ACK 0x00 0x01 0x00 0x02 0x03 Wait ACK

	//формируем буфер с номера страниц для стирания
	for(uint16_t i=0; i < numErasePages; i++)
	{
		bootBuf[i*2]   = (uint8_t)((startPage+i) << 8);
		bootBuf[i*2+1] = (uint8_t)((startPage+i) << 0);
		checksum = checksum ^ bootBuf[i*2] ^ bootBuf[i*2+1];//считаем Checksum
	}
	bootBuf[numErasePages*2] = checksum;//положим Checksum в конце буфера
	//передача буфера
	if(_host_sendData(bootBuf, numErasePages*2+1) == BOOT_I2C_NO_DEVICE) return CMD_NACK; 	//нет устройства на шине

	//Подождем, пока производится стирание
	//STM32F411:
	// tERASE16KB  - Sector(16 KB)  erase time = 800ms  MAX
	// tERASE64KB  - Sector(64 KB)  erase time = 2400ms MAX
	// tERASE128KB - Sector(128 KB) erase time = 2.6sec MAX
	// tME         - Mass           erase time = 16sec  MAX
	//DELAY_milliS(2400 * numErasePages);

	//STM32F103:
	//tERASE - Page(1KB) erase time = 40ms MAX
	//tME    - Mass      erase time = 40ms MAX
	DELAY_milliS(numErasePages * 45);

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK)	return CMD_NACK;
	//---------------------
	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_RP --- !!! В ОТЛАДКЕ !!!
// Description : Readout Protect - Включает защиту от чтения.
// Parameters  :
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint32_t BL_HOST_Cmd_RP(void){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_RP) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Проверим ответ.
	return _host_waitACK();
}
//*******************************************************************
// Function    : BL_HOST_Cmd_RUP --- !!! В ОТЛАДКЕ !!!
// Description : Readout Unprotect - Отключает защиту от чтения.
//				 !!! ВНИМАНИЕ !!! Данная команда стирает ВЕСЬ FLASH!!!
//				 В том числе и загрузчик!!!
// Parameters  :
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint32_t BL_HOST_Cmd_RUP(void){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_RUP) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Проверим ответ.
	return _host_waitACK();
}







//*******************************************************************
// Function    : BL_HOST_Cmd_NS_GetCheckSum --- !!! В ОТЛАДКЕ !!!
// Description : No-Stretch Get Memory Checksum - расчет значения CRC для области памяти.
// Parameters  : addr - адрес (кратный 4м) области во флеш памяти для которой расчитывается CRC
//				 size - размер (кратный 4м) области во флеш памяти для которой расчитывается CRC
// RetVal      : значение CRC для заданного области.
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint32_t BL_HOST_Cmd_NS_GetCheckSum(uint32_t addr, uint32_t size){

	//Передача команды и проверка ответа
	if(_host_sendCmdAndGetAck(CMD_BOOT_NS_GetCheckSum) != CMD_ACK) return CMD_NACK;
	//---------------------
	//Send address (4 bytes, MSB first) and its XOR
	bootBuf[0] = (uint8_t)(addr >> 24);
	bootBuf[1] = (uint8_t)(addr >> 16);
	bootBuf[2] = (uint8_t)(addr >> 8);
	bootBuf[3] = (uint8_t)(addr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);
	//Передаим данные и проверим ответ
	if(_host_sendDataAndGetAck(bootBuf, 5) != CMD_ACK) return CMD_NACK;	//неверный адрес
	//---------------------
	//Send memory size to check (4 bytes, MSB first) and its XOR
	bootBuf[0] = (uint8_t)(size >> 24);
	bootBuf[1] = (uint8_t)(size >> 16);
	bootBuf[2] = (uint8_t)(size >> 8);
	bootBuf[3] = (uint8_t)(size >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);
	//Передаим данные и проверим ответ
	if(_host_sendDataAndGetAck(bootBuf, 5) != CMD_ACK) return CMD_NACK; //неверный размер
	//---------------------
	//Get calculated checksum (4 bytes, MSB first) and its XOR
	//Ждем завершения расчета CRC
	while(_host_waitACK() == CMD_BUSY)
	{
		DELAY_microS(1000);//пауза между запросами.
	}
	//---------------------
	//Вычитываем 4 байта CRC и 1 байт XOR
	if(_host_receiveData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//Проверим байт XOR
	if(_host_getChecksum(bootBuf, 5) != 0) return CMD_NACK; //CRC битая
	//Вернем CRC
	return ((uint32_t)(bootBuf[0] << 24) |
		    (uint32_t)(bootBuf[1] << 16) |
		    (uint32_t)(bootBuf[2] << 8)  |
		    (uint32_t)(bootBuf[3] << 0));
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function   : BL_HOST_GetAppSize()
// Description: подсчет размера приложения, в байтах
// Parameters : Нет
// RetVal     : рахмер приложения в байтах
//*****************************************
uint32_t BL_HOST_GetAppSize(uint32_t addr){

	//Проверка наличия приложения. По стартовому адресу приложения должно
	//лежать значение вершины стека приложения, т.е. значение больше 0x2000 0000.
	//Если это не так, значит приложение отсутсвует
	if((STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR) & 0x2FFC0000) != 0x20000000) return 0;

	//Подсчитаем сколько байт занимает приложение.
	uint32_t appSize_Bytes = 0;
	while(STM32_Flash_ReadWord(addr + appSize_Bytes) != 0xFFFFFFFF)
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
// Function      BL_HOST_CalcCrc()
// Description   расчет контрольной суммы записанного приложения
// Parameters	 addr - адрес (кратен 4м) области, с которого начинаем считать CRC
//				 size - размер (кратен 4м) области, для которой считаем CRC
// RetVal        crc области данных
//*****************************************
uint32_t BL_HOST_CalcCrc(uint32_t *addr, uint32_t size){

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
// Function    : BL_HOST_Loop()
// Description : Основной цикл Хоста протокола загрузчика.
//				 Тут генерируются команды для передачи прошивки загрузчику.
// Parameters  :
// RetVal      :
//*****************************************
uint32_t BL_HOST_BaseLoop(void){

	uint32_t ack  = CMD_NACK;
	uint32_t addr = 0;
	uint32_t temp;
	//---------------------
	//Получим версию загрузчика.
	temp = BL_HOST_Cmd_GetVersion();
	if(temp == CMD_NACK) return CMD_NACK;
	DELAY_milliS(5);

	if(temp == BOOT_I2C_VERSION)
	{
		//Получим версию и разрешенные команды загрузчика
		ack = BL_HOST_Cmd_Get(bootBuf, 12);//прочитаем 12 байтов
		if(ack == CMD_NACK) return CMD_NACK;
		DELAY_milliS(5);

		//TODO ... Тут можно сделать разбор поддерживаемых команд.

	}

	//---------------------
	//Получим идентификатор чипа
	ack = BL_HOST_Cmd_GetID();
	if(ack == CMD_NACK) return CMD_NACK;
	DELAY_milliS(5);

	//---------------------
	//Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
	//ack = BL_HOST_Cmd_RM(bootBuf, APP_LAUNCH_CONDITIONS_ADDR, 4);
	//if(ack == CMD_NACK) return CMD_NACK;
	//DELAY_milliS(5);
	//---------------------
	//Размер приложения в байтах
	uint32_t appSize_Bytes = BL_HOST_GetAppSize(APP_PROGRAM_START_ADDR);

	//Размер приложения в страницах (1024 байта)
	uint32_t appSize_Pages = appSize_Bytes / 1024;
	if((appSize_Bytes % 1024) != 0) appSize_Pages += 1;

	//---------------------
	//сотрем область приложения - appSize_Pages страниц, начиная со страницы 10
	ack = BL_HOST_Cmd_ERASE(appSize_Pages, 10);
	if(ack == CMD_NACK) return CMD_NACK;
	DELAY_milliS(5);

	//сотрем область Условие запуска приложения - 1 страница, начиная со страницы 9
	//ack = BL_HOST_Cmd_ERASE(1, 9);
	//if(ack == CMD_NACK) return CMD_NACK;
	//DELAY_milliS(5);

	//---------------------
	//TODO ... Сделать проверку CRC приложения
	uint32_t appCrc = BL_HOST_CalcCrc((uint32_t*)APP_PROGRAM_START_ADDR, appSize_Bytes);

	//---------------------
	//Запишем приложение
	//Всего 128 страниц флэш-памяти по 1024 байта каждая.
	//Размер загрузчика 10КБ (10240 байт = 0х2800)
	//(9КБ - это сам загрузчик, 1КБ - это обасть хранения состояния приложения).
	for(uint32_t i=0; i < appSize_Pages; i++)//
	{
		addr = APP_PROGRAM_START_ADDR + 1024*i; //шагаем по 1024 байта
		//Запишем страницу флэш-памяти (1024 байта)
		ack = BL_HOST_Cmd_WM((addr+256*0), 0, 256);
		ack = BL_HOST_Cmd_WM((addr+256*1), 0, 256);
		ack = BL_HOST_Cmd_WM((addr+256*2), 0, 256);
		ack = BL_HOST_Cmd_WM((addr+256*3), 0, 256);

		DELAY_milliS(30);
		LED_PC13_Toggel();
		if(ack == CMD_NACK) return CMD_NACK;
	}

	//---------------------
	//Получим от загрузчика CRC приложения.
	ack = BL_HOST_Cmd_NS_GetCheckSum(APP_PROGRAM_START_ADDR, appSize_Bytes);
	if(ack == CMD_NACK) return CMD_NACK;
	//Сравним CRC
	if(appCrc != ack)
	{
		while(1)
		{
			//Мигаем...
			LED_PC13_Toggel();
			DELAY_milliS(1000);
		}
	}

	//---------------------
	//Включим защиту от чтения
	//ack = BL_HOST_Cmd_RP();
	//if(ack == CMD_NACK) return CMD_NACK;

	//---------------------
	//Переход на приложение после записи его в память
	//ack = BL_HOST_Cmd_Go(APP_PROGRAM_START_ADDR);
	//if(ack == CMD_NACK) return CMD_NACK;

	//Переход на приложение произойдет после сброса МК
	//по сторожевому таймеру (чере 3сек.)

	while(1)
	{
		//Мигаем...
		LED_PC13_Toggel();
		DELAY_milliS(3000);
	}
	//---------------------
	return CMD_ACK;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************













