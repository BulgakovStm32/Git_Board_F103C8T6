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
static uint8_t bootBuf[FLASH_PAGE_SIZE + 8] = {0};	//

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _bl_emulator_StartAndSendDeviceAddr()
// Description : формирование старт последоватльности, предача адреса Sleve и ожидание ответа
// Parameters  :
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
static uint8_t _bl_emulator_StartAndSendDeviceAddr(uint8_t rw){

	uint32_t count = 0;
	//---------------------
	//Ждем ответа Slave
	while(I2C_StartAndSendDeviceAddr(BOOT_I2C, BOOT_I2C_ADDR|rw) != I2C_OK)
	{
		I2C_Stop(BOOT_I2C);
		if(++count >= 3) return BOOT_I2C_NO_DEVICE;//Если через 3 попытки нет ответа от устройства, то выходим
	}
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : _bl_emulator_SendBuf()
// Description : передача данных из буфера
// Parameters  : buf  - указатель на передаваемый буфер
//				 size - размер передаваемого буфера, от 1 байта
// RetVal      :
//*****************************************
static void _bl_emulator_SendBuf(uint8_t *buf, uint32_t size){

	I2C_SendDataWithStop(BOOT_I2C, buf, size);
	//I2C_SendDataWithoutStop(BOOT_I2C, buf, size);
}
//*******************************************************************
// Function    : _bl_emulator_ReadBuf()
// Description : прием данных в буфера
// Parameters  : buf  - указатель на приемный буфер
//				 size - кол-во байтов на прием, от 1 байта
// RetVal      :
//*****************************************
static void _bl_emulator_ReadBuf(uint8_t *buf, uint32_t size){

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
// Function    : BL_EMULATOR_I2CInit()
// Description : инициализация I2C
// Parameters  :
// RetVal      :
//*****************************************
void BL_EMULATOR_I2CInit(void){

	I2C_Master_Init(BOOT_I2C, I2C_GPIO_NOREMAP, BOOT_I2C_SPEED);
}
//*******************************************************************
// Function    : BL_EMULATOR_CheckDevice()
// Description : проверка присутсвия загрузчика на шине I2C
// Parameters  :
// RetVal      : BOOT_I2C_DEVICE_OK - загрузчик ответил,
//				 BOOT_I2C_NO_DEVICE - загрузчика нет на шине.
//*****************************************
uint8_t BL_EMULATOR_CheckDevice(void){

	uint32_t count = 0;
	//---------------------
	while(!I2C_Master_CheckSlave(BOOT_I2C, BOOT_I2C_ADDR))
	{
		DELAY_microS(10);
		if(++count >= 3) return BOOT_I2C_NO_DEVICE;
	}
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : BL_EMULATOR_SendCmd()
// Description : Отправить кадр команды: Хост инициирует связь в качестве главного передатчика и
//				 отправляет на устройство два байта: код_команды + инверсный_код_команды.
// Parameters  : cmd - команда, которую хотим передать
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t BL_EMULATOR_SendCmd(uint8_t cmd){

	uint8_t buf[2] = {cmd, ~cmd};
	//---------------------
	//Передаем команду и ее инверсию
	if(_bl_emulator_StartAndSendDeviceAddr(BOOT_I2C_Wr) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_bl_emulator_SendBuf(buf, 2);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : BL_EMULATOR_WaitACK()
// Description : Ожидание кадра ACK/NACK: Хост инициирует связь I2C в качестве главного получателя и
//				 получает от устройства один байт: ACK, NACK или BUSY.
// Parameters  :
// RetVal      : CMD_BOOT_ACK  - пакет принят   (команда выполнена)
//				 CMD_BOOT_NACK - пакет отброшен (команда не выполнена)
//				 CMD_BOOT_BUSY - состояние занятости (команда в процессе выполнения)
//*****************************************
uint8_t BL_EMULATOR_WaitACK(void){

	uint8_t byte = 0;
	//---------------------
	//ждем ответа
	if(_bl_emulator_StartAndSendDeviceAddr(BOOT_I2C_Rd) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_bl_emulator_ReadBuf(&byte, 1);
	return byte;
}
//*******************************************************************
// Function    : BL_EMULATOR_ReceiveData()
// Description : Получение кадра данных: Хост инициирует связь I2C в качестве главного приемника и
//				 получает ответ от устройства. Количество полученных байтов зависит от команды.
// Parameters  : buf  - указатель на приемный буфер
//				 size - кол-во байтов на прием, от 1 байта
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес и передало все данные
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t BL_EMULATOR_ReceiveData(uint8_t *buf, uint32_t size){

	if(_bl_emulator_StartAndSendDeviceAddr(BOOT_I2C_Rd) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_bl_emulator_ReadBuf(buf, size);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : BL_EMULATOR_SendData()
// Description : отправка кадра данных: Хост инициирует связь I2C в качестве главного передатчика и
//				 отправляет необходимые байты на устройство. Количество передаваемых байтов зависит от команды.
// Parameters  : buf  - указатель на передающий буфер
//				 size - кол-во байтов на передачу, от 1 байта
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес и передало все данные
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t BL_EMULATOR_SendData(uint8_t *buf, uint32_t size){

	if(_bl_emulator_StartAndSendDeviceAddr(BOOT_I2C_Wr) == BOOT_I2C_NO_DEVICE)
	{
		return BOOT_I2C_NO_DEVICE;
	}
	_bl_emulator_SendBuf(buf, size);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************
// Function    : BL_EMULATOR_XorBuf()
// Description : расчет контрольной суммы. Полученные блоки байтов данных подвергаются операции XOR.
//			     Байт, содержащий вычисленное XOR всех предыдущих байтов, добавляется в конец каждого
//				 сообщения (байт контрольной суммы). При выполнении XOR всех полученных байтов данных и
//				 контрольной суммы результат в конце пакета должен быть 0x00.
// Parameters  : buf  - указатель на буфер
//				 size - кол-во байтов для расчета, от 1 байта
// RetVal      : XOR байтов
//*****************************************
uint8_t BL_EMULATOR_GetXorChecksum(uint8_t *buf, uint32_t size){

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
//****************************КОМАНДЫ ЭМУЛЯТОРА ЗАГРУЗЧИКА***********************************
// Function    : BL_EMULATOR_Cmd_Get() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Получает версию и разрешенные команды, поддерживаемые текущей версией загрузчика.
// Parameters  :
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_EMULATOR_Cmd_Get(void){

	//uint8_t num;
	//---------------------
	//Передача команды
	if(BL_EMULATOR_SendCmd(CMD_BOOT_Get) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Примем байт с количеством вычитываемых байтов.
	//if(BL_EMULATOR_ReceiveData(&num, 1) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Чтение списка поддерживаемых команд. 19 команд для версии загрузчика 1.1(такой в STM32F411)
	if(BL_EMULATOR_ReceiveData(bootBuf, 19) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//if(BL_EMULATOR_ReceiveData(bootBuf, num) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_EMULATOR_Cmd_GetVersion() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Получает версию загрузчика.
// Parameters  :
// RetVal      : bootLoaderVersion - версию загрузчика
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_EMULATOR_Cmd_GetVersion(void){

	uint8_t version = 0;
	//---------------------
	//STM32 отправляет байты следующим образом: - Byte 1: ACK
	//- Byte 2: Bootloader version (0 < Version <= 255) (for example, 0x10 = Version 1.0)
	//- Byte 3: ACK

	//Передача команды
	if(BL_EMULATOR_SendCmd(CMD_BOOT_GetVersion) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Чтение версии загрузчика
	if(BL_EMULATOR_ReceiveData(&version, 1) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	return version;
}
//*******************************************************************
// Function    : BL_EMULATOR_Cmd_GetID() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Получает идентификатор чипа
// Parameters  :
// RetVal      : PID - идентификатор чипа
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint16_t BL_EMULATOR_Cmd_GetID(void){

	//Устройство STM32 отправляет байты следующим образом: - Byte 1: ACK
	//- Byte 2: N = the number of bytes-1 (for STM32, N = 1), except for current byte and ACKs
	//- Bytes 3-4: PID (product ID)
	//- Byte 3 = MSB
	//- Byte 4 = LSB
	//- Byte 5: ACK

	//Передача команды
	if(BL_EMULATOR_SendCmd(CMD_BOOT_GetID) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Чтение данных
	if(BL_EMULATOR_ReceiveData(bootBuf, 3) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Вернем PID (product ID)
	return (uint16_t)((bootBuf[1] << 8) |	//MSB
			   	   	  (bootBuf[2] << 0));	//LSB;
}
//*******************************************************************
// Function    : BL_EMULATOR_Cmd_RM() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
// Parameters  : addr - адрес, с которого начинаем чтение
//				 size - сколько байт нужно прочитать
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_EMULATOR_Cmd_RM(uint32_t addr, uint32_t size){

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
	if(BL_EMULATOR_SendCmd(CMD_BOOT_RM) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Предадим адрес (Start address) откуда хотим прочитать данные
	bootBuf[0] = (uint8_t)(addr >> 24);
	bootBuf[1] = (uint8_t)(addr >> 16);
	bootBuf[2] = (uint8_t)(addr >> 8);
	bootBuf[3] = (uint8_t)(addr >> 0);
	bootBuf[4] = BL_EMULATOR_GetXorChecksum(bootBuf, 4);//Checksum
	//Передаим данные
	if(BL_EMULATOR_SendData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Send data frame: number of bytes to be read (1 byte) and a checksum (1 byte)
	bootBuf[0] = (uint8_t)(size - 1);		//The number of bytes to be read-1
	bootBuf[1] = (uint8_t)(size - 1) ^ 0xFF;//Checksum
	//Передадим данные
	if(BL_EMULATOR_SendData(bootBuf, 2) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Чтение данных - Receive data frame: needed data from the BL
	if(BL_EMULATOR_ReceiveData(bootBuf, size) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_EMULATOR_Cmd_Go() --- РАБОТАЕТ!!!-- ПРОВЕРЕНО на STM32F411 !!!
// Description : Переходит к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
// Parameters  : appAddr - адрес приложения
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_EMULATOR_Cmd_Go(uint32_t appAddr){

	//Хост отправляет байты на STM32 следующим образом:
	//- Byte 1: 0x21
	//- Byte 2: 0xDE
	//Wait for ACK
	//- Byte 3 to byte 6: start address (Byte 3 - MSB; Byte 6 - LSB)
	//- Byte 7: checksum: XOR (byte 3, byte 4, byte 5, byte 6)
	//Wait for ACK

	//Передача команды
	if(BL_EMULATOR_SendCmd(CMD_BOOT_Go) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	//Предадим Start address приложения
	bootBuf[0] = (uint8_t)(appAddr >> 24);
	bootBuf[1] = (uint8_t)(appAddr >> 16);
	bootBuf[2] = (uint8_t)(appAddr >> 8);
	bootBuf[3] = (uint8_t)(appAddr >> 0);
	bootBuf[4] = BL_EMULATOR_GetXorChecksum(bootBuf, 4);//Checksum
	//Передаим данные
	if(BL_EMULATOR_SendData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;

	return CMD_ACK;
}
//*******************************************************************
// Function    : BL_EMULATOR_Cmd_WM()
// Description : Write Memory - Записывает в память до 256 байт, начиная с адреса wrAddr
// Parameters  : wrAddr - адрес куда пишем
//				 wrBuf  - буфер с данными на запись
//				 swSize - кол-во байтов на запись (от 1 до 256)
// RetVal      : CMD_ACK  - пакет принят   (команда выполнена)
//				 CMD_NACK - пакет отброшен (команда не выполнена)
//*****************************************
uint8_t BL_EMULATOR_Cmd_WM(uint32_t wrAddr, uint8_t* wrBuf, uint32_t wrSize){

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
	if(BL_EMULATOR_SendCmd(CMD_BOOT_WM) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;
	//---------------------
	//Предадим Start address куда хотим писать данные
	bootBuf[0] = (uint8_t)(wrAddr >> 24);
	bootBuf[1] = (uint8_t)(wrAddr >> 16);
	bootBuf[2] = (uint8_t)(wrAddr >> 8);
	bootBuf[3] = (uint8_t)(wrAddr >> 0);
	bootBuf[4] = BL_EMULATOR_GetXorChecksum(bootBuf, 4);	//Checksum
	//Передаим данные
	if(BL_EMULATOR_SendData(bootBuf, 5) == BOOT_I2C_NO_DEVICE) return CMD_NACK; //нет устройства на шине
	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK) return CMD_NACK;
	//---------------------
	//Передача кол-ва байтов N, которое нужно записать, минус 1(0 < N <= 255)
	bootBuf[0] = (uint8_t)(wrSize-1);//-1 для того чтобы можно было передать число от 1 до 256

	//N+1 байтов на запись (max 256 bytes)
	//STM32_Flash_ReadBufU8((void*)wrAddr, (void*)&bootBuf[1], wrSize);
	STM32_Flash_ReadBufU32((void*)wrAddr, (void*)&bootBuf[1], wrSize);

	//Контрольная сумма
	bootBuf[wrSize+1] = BL_EMULATOR_GetXorChecksum(bootBuf, wrSize+1);//+1 байт потому что + байт wrSize

	//Передаим данные. +2 байта потому что + байт wrSize и + байт Checksum
	if(BL_EMULATOR_SendData(bootBuf, wrSize+2) == BOOT_I2C_NO_DEVICE)
		return CMD_NACK; //нет устройства на шине

	//Подождем окончания записи (~2мс)
	DELAY_milliS(2);

	//Проверим ответ.
	if(BL_EMULATOR_WaitACK() != CMD_ACK)
		return CMD_NACK;
	//---------------------
	return CMD_ACK;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************
// Function    : BL_EMULATOR_Loop()
// Description : Основной цикл эмулятора протокола загрузчика.
//				 Тут генерируются команды для передачи прошивки загрузчику.
// Parameters  :
// RetVal      :
//*****************************************
uint32_t BL_EMULATOR_BaseLoop(void){

	//Машина состояний эмулятора хоста загрузчика
	static uint32_t bootState = CMD_BOOT_Get;//Начнем с команды CMD_BOOT_Get
	uint32_t ack = CMD_NACK;
	//---------------------
	switch(bootState){
		//------------------
		case(CMD_BOOT_Get):
			ack = BL_EMULATOR_Cmd_Get();
			bootState = CMD_BOOT_GetVersion;
		break;
		//------------------
		case(CMD_BOOT_GetVersion):
			ack = BL_EMULATOR_Cmd_GetVersion();
			bootState = CMD_BOOT_GetID;
		break;
		//------------------
		case(CMD_BOOT_GetID):
			ack = BL_EMULATOR_Cmd_GetID();
			bootState = CMD_BOOT_RM;
		break;
		//------------------
		case(CMD_BOOT_RM):
			ack = BL_EMULATOR_Cmd_RM(APP_PROGRAM_START_ADDR, 16);
			bootState = CMD_BOOT_Go;
		break;
		//------------------
		case(CMD_BOOT_Go):
			//ack = BL_EMULATOR_Cmd_Go(0x08000000);
			bootState = CMD_BOOT_WM;
		break;
		//------------------
		case(CMD_BOOT_WM):

		//Запишем страницу (1024 байта)
		ack = BL_EMULATOR_Cmd_WM((APP_PROGRAM_START_ADDR+256*0), 0, 256);
		DELAY_milliS(5);

		ack = BL_EMULATOR_Cmd_WM((APP_PROGRAM_START_ADDR+256*1), 0, 256);
		DELAY_milliS(5);

		ack = BL_EMULATOR_Cmd_WM((APP_PROGRAM_START_ADDR+256*2), 0, 256);
		DELAY_milliS(5);

		ack = BL_EMULATOR_Cmd_WM((APP_PROGRAM_START_ADDR+256*4), 0, 256);
		DELAY_milliS(5);

			bootState = CMD_BOOT_Get;
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













