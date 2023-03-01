/*
 * i2cBootLoaderProtocol.c
 *
 *  Created on: 10 февр. 2023 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

#include "i2c_ST.h"

//*******************************************************************************************
//*******************************************************************************************
static uint8_t bootBuf[FLASH_PAGE_SIZE + 16] = {0};	//

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
		I2C_Stop(BOOT_I2C);
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
static void _sendBuf(uint8_t *buf, uint32_t size){

	I2C_SendDataWithStop(BOOT_I2C, buf, size);
	//I2C_SendDataWithoutStop(BOOT_I2C, buf, size);
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
//*******************************************************************
// Function    : _bl_emulator_FillBuf()
// Description : заполнение буфера нужным значением
// Parameters  : buf    - указатель на буфер
//				 size   - кол-во байтов в буфере, которое нужно заполнить
//				 filler - значение, которым будет заполнен буфер
// RetVal      :
//*****************************************
//static void _bl_emulator_FillBuf(uint8_t *buf, uint32_t size, uint8_t filler){
//
//	for(uint32_t i=0; i < size; i++)
//	{
//		buf[i] = filler;
//	}
//}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _host_sendCmd()
// Description : Отправить кадр команды: Хост инициирует связь в качестве главного передатчика и
//				 отправляет на устройство два байта: код_команды + инверсный_код_команды.
// Parameters  : cmd - команда, которую хотим передать
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
static uint8_t _host_sendCmd(uint8_t cmd){

	uint8_t buf[2] = {cmd, ~cmd};
	//---------------------
	//Передаем команду и ее инверсию
	if(_startAndSendDeviceAddr(BOOT_I2C_Wr) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_sendBuf(buf, 2);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
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
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : BL_HOST_I2CInit()
// Description : инициализация I2C
// Parameters  :
// RetVal      :
//*****************************************
void BL_HOST_Init(void){

	I2C_Master_Init(BOOT_I2C, I2C_GPIO_NOREMAP, BOOT_I2C_SPEED);
}
//*******************************************************************
// Function    : BL_EMULATOR_CheckDevice()
// Description : проверка присутсвия загрузчика на шине I2C
// Parameters  :
// RetVal      : BOOT_I2C_DEVICE_OK - загрузчик ответил,
//				 BOOT_I2C_NO_DEVICE - загрузчика нет на шине.
//*****************************************
uint8_t BL_HOST_CheckDevice(void){

	uint32_t count = 0;
	//---------------------
	while(!I2C_Master_CheckSlave(BOOT_I2C, BOOT_I2C_ADDR))
	{
		DELAY_microS(10);
		if(++count >= 3) return BOOT_I2C_NO_DEVICE;
	}
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//****************************КОМАНДЫ ЭМУЛЯТОРА ЗАГРУЗЧИКА***********************************
// Function    : BL_HOST_Cmd_Get() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Получает версию и разрешенные команды, поддерживаемые текущей версией загрузчика.
// Parameters  :
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_Get(void){

	//uint8_t num;
	//---------------------
	//Передача команды
	if(_host_sendCmd(CMD_BOOT_Get) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Чтение списка поддерживаемых команд.
	//19 команд для версии загрузчика 1.1(такой в STM32F411)
	if(_host_receiveData(bootBuf, 19) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//if(BL_EMULATOR_ReceiveData(bootBuf, num) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_GetVersion() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Получает версию загрузчика.
// Parameters  :
// RetVal      : bootLoaderVersion - версию загрузчика
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_GetVersion(void){

	uint8_t version = 0;
	//---------------------
	//STM32 отправляет байты следующим образом: - Byte 1: ACK
	//- Byte 2: Bootloader version (0 < Version <= 255) (for example, 0x10 = Version 1.0)
	//- Byte 3: ACK

	//Передача команды
	if(_host_sendCmd(CMD_BOOT_GetVersion) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Чтение версии загрузчика
	if(_host_receiveData(&version, 1) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	return version;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_GetID() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Получает идентификатор чипа
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

	//Передача команды
	if(_host_sendCmd(CMD_BOOT_GetID) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Чтение данных
	if(_host_receiveData(bootBuf, 3) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Вернем PID (product ID)
	return (uint16_t)((bootBuf[1] << 8) |	//MSB
			   	   	  (bootBuf[2] << 0));	//LSB;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_RM() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
// Parameters  : addr - адрес, с которого начинаем чтение
//				 size - сколько байт нужно прочитать
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_RM(uint32_t addr, uint32_t size){

	//Хост отправляет байты на STM32 следующим образом:
	//- Byte 1: 0x11
	//- Byte 2: 0xEE
	//Wait for ACK
	//- Bytes 3-6: Start address (byte 3: MSB, byte 6: LSB)
	//- Byte 7: Checksum: XOR (byte 3, byte 4, byte 5, byte 6)
	//Wait for ACK
	//- Byte 8: The number of bytes to be read - 1 (0 < N <= 255)
	//- Byte 9: Checksum: XOR byte 8 (complement of byte 8)

	//Передача команды
	if(_host_sendCmd(CMD_BOOT_RM) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Предадим адрес (Start address) откуда хотим прочитать данные
	bootBuf[0] = (uint8_t)(addr >> 24);
	bootBuf[1] = (uint8_t)(addr >> 16);
	bootBuf[2] = (uint8_t)(addr >> 8);
	bootBuf[3] = (uint8_t)(addr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);//Checksum
	//Передаим данные
	if(_host_sendData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Send data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	bootBuf[0] = (uint8_t)(size - 1);		//The number of bytes to be read-1
	bootBuf[1] = (uint8_t)(size - 1) ^ 0xFF;//Checksum
	//Передадим данные
	if(_host_sendData(bootBuf, 2) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Чтение данных - Receive data frame: needed data from the BL
	if(_host_receiveData(bootBuf, size) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_Go() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Переходит к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
// Parameters  : appAddr - адрес приложения
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_Go(uint32_t appAddr){

	//Хост отправляет байты на STM32 следующим образом:
	//- Byte 1: 0x21
	//- Byte 2: 0xDE
	//Wait for ACK
	//- Byte 3 to byte 6: start address (Byte 3 - MSB; Byte 6 - LSB)
	//- Byte 7: checksum: XOR (byte 3, byte 4, byte 5, byte 6)
	//Wait for ACK

	//Передача команды
	if(_host_sendCmd(CMD_BOOT_Go) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	//Предадим Start address приложения
	bootBuf[0] = (uint8_t)(appAddr >> 24);
	bootBuf[1] = (uint8_t)(appAddr >> 16);
	bootBuf[2] = (uint8_t)(appAddr >> 8);
	bootBuf[3] = (uint8_t)(appAddr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);//Checksum
	//Передаим данные
	if(_host_sendData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;

	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_WM() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Write Memory - Записывает в память до 256 байт, начиная с адреса wrAddr
// Parameters  : wrAddr - адрес куда пишем
//				 wrBuf  - буфер с данными на запись
//				 swSize - кол-во байтов на запись (от 1 до 256)
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_WM(uint32_t wrAddr, uint8_t* wrBuf, uint32_t wrSize){

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

	//---------------------
	//Передача команды
	if(_host_sendCmd(CMD_BOOT_WM) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;	//команда не принята.
	//---------------------
	//Передадим Start address куда хотим писать данные
	bootBuf[0] = (uint8_t)(wrAddr >> 24);
	bootBuf[1] = (uint8_t)(wrAddr >> 16);
	bootBuf[2] = (uint8_t)(wrAddr >> 8);
	bootBuf[3] = (uint8_t)(wrAddr >> 0);
	bootBuf[4] = _host_getChecksum(bootBuf, 4);	//Checksum
	//Передаим данные
	if(_host_sendData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; 	//нет устройства на шине
	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK)	return CMD_NACK;	//неверный Start address
	//---------------------
	//Передача кол-ва байтов N, которое нужно записать, минус 1(0 < N <= 255)
	bootBuf[0] = (uint8_t)(wrSize-1);//-1 для того чтобы можно было передать число от 1 до 256

	//Перепишем N+1 байтов в буфер передачи
	STM32_Flash_ReadBufU8((void*)wrAddr, (void*)&bootBuf[1], wrSize);

	//Контрольная сумма
	bootBuf[wrSize+1] = _host_getChecksum(bootBuf, wrSize+1);//+1 байт потому что +байт N

	//Передаим данные. +2 байта потому что +байт N и +байт Checksum
	if(_host_sendData(bootBuf, wrSize+2) == BOOT_I2C_NO_DEVICE)	return CMD_NACK; //нет устройства на шине

	//Подождем окончания записи
	//tprog - 16-bit programming time = 70μs МАКС
	//1024 байта запишется за ~4.5мс
	DELAY_milliS(5);

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;	//ошибка при записи
	//---------------------
	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_HOST_Cmd_ERASE() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Команда стирает от одной до всех страниц или секторов флэш-памяти,
//				 используя режим двухбайтовой адресации.
// Parameters  : numErasePages - количество страниц или секторов, которые необходимо стереть
//				 startPage     - страница или сектор, с которой начнется стирание
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_HOST_Cmd_ERASE(uint16_t numErasePages, uint16_t startPage){

	//Хост отправляет байты на STM32 следующим образом:
	//- Byte 1: 0x44
	//- Byte 2: 0xBB
	//Wait for ACK

	//For Special erase:
	//- Bytes 3-4: Special erase (0xFFFx)
	//- Bytes 5: Checksum of Bytes 3-4
	//Wait for ACK

	//For Page erase:
	//- Bytes 3-4: Number of pages or sectors to be erased - 1
	//- Bytes 5: Checksum of Bytes 3-4
	//Wait for ACK

	//- (2 x N) bytes (page numbers or sectors coded on two bytes MSB first) and then the checksum for these bytes.
	//Wait for ACK

	//Example of I2C frame:
	//– erase page 1: 0x44 0xBB Wait ACK 0x00 0x00 0x00 Wait ACK 0x00 0x01 0x01 Wait ACK
	//– erase page 1 and page 2:
	//0x44 0xBB Wait ACK 0x00 0x01 0x01 Wait ACK 0x00 0x01 0x00 0x02 0x03 Wait ACK

	uint16_t numMinusOne = numErasePages - 1;
	uint8_t  checksum = 0;
	//---------------------
	//Передача команды
	if(_host_sendCmd(CMD_BOOT_ERASE) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK) return CMD_NACK;	//команда не принята.
	//---------------------
	//Передадим кол-во страниц или секторов, которые необходимо стереть минус 1 и Checksum
	bootBuf[0] = (uint8_t)(numMinusOne << 8);
	bootBuf[1] = (uint8_t)(numMinusOne << 0);
	bootBuf[2] = bootBuf[0] ^ bootBuf[1]; //_host_getChecksum(bootBuf, 2);	//Checksum
	//Передаим данные
	if(_host_sendData(bootBuf, 3) == BOOT_I2C_NO_DEVICE) return CMD_NACK; 	//нет устройства на шине
	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK)	return CMD_NACK;	//неверное кол-во страниц или секторов
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
	DELAY_milliS(500); 	//STM32F411:
						// tERASE16KB  - Sector(16 KB)  erase time = 800ms  MAX
						// tERASE64KB  - Sector(64 KB)  erase time = 2400ms MAX
						// tERASE128KB - Sector(128 KB) erase time = 2.6sec MAX
						// tME         - Mass           erase time = 16sec  MAX

	//DELAY_milliS(45); 	//STM32F103:
						//tERASE - Page(1KB) erase time = 40ms MAX
						//tME    - Mass      erase time = 40ms MAX

	//Проверим ответ.
	if(_host_waitACK() != CMD_ACK)	return CMD_NACK;
	//---------------------
	return CMD_ACK;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : BL_HOST_Loop()
// Description : Основной цикл эмулятора протокола загрузчика.
//				 Тут генерируются команды для передачи прошивки загрузчику.
// Parameters  :
// RetVal      :
//*****************************************
uint32_t BL_HOST_BaseLoop(void){

	//Машина состояний эмулятора хоста загрузчика
	static uint32_t bootState = CMD_BOOT_Get;//Начнем с команды CMD_BOOT_Get
	uint32_t ack  = CMD_NACK;
	uint32_t addr = 0;
	//---------------------
	switch(bootState){
		//------------------
		case(CMD_BOOT_Get):
			ack = BL_HOST_Cmd_Get();
			bootState = CMD_BOOT_GetVersion;
		break;
		//------------------
		case(CMD_BOOT_GetVersion):
			ack = BL_HOST_Cmd_GetVersion();
			bootState = CMD_BOOT_GetID;
		break;
		//------------------
		case(CMD_BOOT_GetID):
			ack = BL_HOST_Cmd_GetID();
			bootState = CMD_BOOT_RM;
		break;
		//------------------
		case(CMD_BOOT_RM):
			ack = BL_HOST_Cmd_RM(APP_PROGRAM_START_ADDR, 16);
			bootState = CMD_BOOT_Go;
		break;
		//------------------
		case(CMD_BOOT_Go):
			//ack = BL_HOST_Cmd_Go(0x08000000);
			bootState = CMD_BOOT_WM;
		break;
		//------------------
		case(CMD_BOOT_WM):

			//Всего 128 страниц флэш-памяти по 1024 байта каждая.
			//Размер загрузчика 10КБ (10240 байт = 0х2800)
			//(9КБ - это сам загрузчик, 1КБ - это обасть хранения состояния приложения).
			for(uint32_t i=0; i < (128-10); i++)
			{
				addr = APP_PROGRAM_START_ADDR + 1024*i; //шагаем по 1024 байта
				//Запишем страницу флэш-памяти (1024 байта)
				ack = BL_HOST_Cmd_WM((addr+256*0), 0, 256);
				ack = BL_HOST_Cmd_WM((addr+256*1), 0, 256);
				ack = BL_HOST_Cmd_WM((addr+256*2), 0, 256);
				ack = BL_HOST_Cmd_WM((addr+256*3), 0, 256);

				DELAY_milliS(30);
				LED_PC13_Toggel();
			}
			//запишем признак запуска приложения после ресета
			//....

			//Переход на приложение после записи его в память
			BL_HOST_Cmd_Go(APP_PROGRAM_START_ADDR);
			while(1);
			//bootState = CMD_BOOT_Get;
		break;
		//------------------
		case(CMD_BOOT_ERASE):
			ack = BL_HOST_Cmd_ERASE(1, 0);
			DELAY_milliS(100);
		break;
		//------------------
		default:

		break;
		//------------------
	}
	//***********************************************
	return ack;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************













