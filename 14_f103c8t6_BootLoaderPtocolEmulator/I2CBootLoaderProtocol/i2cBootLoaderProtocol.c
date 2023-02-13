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


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : _bl_prot_StartAndSendDeviceAddr()
// Description : формирование старт последоватльности, предача адреса Sleve и ожидание ответа
// Parameters  :
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
static uint8_t _bl_prot_StartAndSendDeviceAddr(uint8_t rw){

	uint32_t count = 0;
	//---------------------
	//Ждем ответа Slave
	while(I2C_StartAndSendDeviceAddr(BOOT_I2C, BOOT_I2C_ADDR|rw) != I2C_OK)
	{
		DELAY_milliS(5);
		//если через 100 мс нет ответа от Slave, то выходим.
		if(++count >= 20) return BOOT_I2C_NO_DEVICE;
	}
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************************************
// Function    : _bl_prot_SendBuf()
// Description : передача данных из буфера
// Parameters  : buf  - указатель на передаваемый буфер
//				 size - размер передаваемого буфера, от 1 байта
// RetVal      :
//*****************************************
static void _bl_prot_SendBuf(uint8_t *buf, uint32_t size){

	I2C_SendDataWithStop(BOOT_I2C, buf, size);
	//I2C_SendDataWithoutStop(BOOT_I2C, buf, size);
}
//*******************************************************************************************
// Function    : _bl_prot_ReadBuf()
// Description : прием данных в буфера
// Parameters  : buf  - указатель на приемный буфер
//				 size - кол-во байтов на прием, от 1 байта
// RetVal      :
//*****************************************
static void _bl_prot_ReadBuf(uint8_t *buf, uint32_t size){

	I2C_ReadData(BOOT_I2C, buf, size);
}
//*******************************************************************************************
// Function    : _bl_prot_Stop()
// Description : формирование Stop на шине I2C
// Parameters  :
// RetVal      :
//*****************************************
//static void _bl_prot_Stop(void){
//
//	I2C_Stop(BOOT_I2C);
//}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
// Function    : BL_PROT_I2CInit()
// Description : инициализация I2C
// Parameters  :
// RetVal      :
//*****************************************
void BL_PROT_I2CInit(void){

	I2C_Master_Init(BOOT_I2C, I2C_GPIO_NOREMAP, BOOT_I2C_SPEED);
}
//*******************************************************************************************
// Function    : BL_PROT_CheckDevice()
// Description : проверка присутсвия загрузчика на шине I2C
// Parameters  :
// RetVal      : 1 - загрузчик ответил, 0 - загрузчика нет на шине.
//*****************************************
uint8_t BL_PROT_CheckDevice(void){

	uint32_t count = 0;
	//---------------------
	while(!I2C_Master_CheckSlave(BOOT_I2C, BOOT_I2C_ADDR))
	{
		LED_PC13_Toggel();
		DELAY_milliS(100);
		//если через 1 секунду нет ответа от Slave, то выходим.
		if(++count >= 10) return BOOT_I2C_NO_DEVICE;
	}
	LED_PC13_Off();
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************************************
// Function    : BL_PROT_SendCmd()
// Description : Отправить кадр команды: Хост инициирует связь в качестве главного передатчика и
//				 отправляет на устройство два байта: код_команды + инверсный_код_команды.
// Parameters  : cmd - команда, которую хотим передать
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t BL_PROT_SendCmd(uint8_t cmd){

	uint8_t buf[2] = {cmd, ~cmd};
	//---------------------
	//Передаем команда и ее инверсию
	if(_bl_prot_StartAndSendDeviceAddr(BOOT_I2C_WRITE) == BOOT_I2C_NO_DEVICE) return BOOT_I2C_NO_DEVICE;
	_bl_prot_SendBuf(buf, 2);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************************************
// Function    : BL_PROT_WaitACK()
// Description : Ожидание кадра ACK/NACK: Хост инициирует связь I2C в качестве главного получателя и
//				 получает от устройства один байт: ACK, NACK или BUSY.
// Parameters  :
// RetVal      : CMD_BOOT_ACK  - пакет принят   (команда выполнена)
//				 CMD_BOOT_NACK - пакет отброшен (команда не выполнена)
//				 CMD_BOOT_BUSY - состояние занятости (команда в процессе выполнения)
//*****************************************
uint8_t BL_PROT_WaitACK(void){

	uint8_t byte;
	//---------------------
	//ждем ответа
	if(_bl_prot_StartAndSendDeviceAddr(BOOT_I2C_READ) == BOOT_I2C_NO_DEVICE) return BOOT_I2C_NO_DEVICE;
	_bl_prot_ReadBuf(&byte, 1);
	return byte;
}
//*******************************************************************************************
// Function    : BL_PROT_ReceiveData()
// Description : Получение кадра данных: Хост инициирует связь I2C в качестве главного приемника и
//				 получает ответ от устройства. Количество полученных байтов зависит от команды.
// Parameters  : buf  - указатель на приемный буфер
//				 size - кол-во байтов на прием, от 1 байта
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес и передало все данные
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t BL_PROT_ReceiveData(uint8_t *buf, uint32_t size){

	if(_bl_prot_StartAndSendDeviceAddr(BOOT_I2C_READ) == BOOT_I2C_NO_DEVICE) return BOOT_I2C_NO_DEVICE;
	_bl_prot_ReadBuf(buf, size);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************************************
// Function    : BL_PROT_SendData()
// Description : отправка кадра данных: Хост инициирует связь I2C в качестве главного передатчика и
//				 отправляет необходимые байты на устройство. Количество передаваемых байтов зависит от команды.
// Parameters  : buf  - указатель на передающий буфер
//				 size - кол-во байтов на передачу, от 1 байта
// RetVal      : BOOT_I2C_DEVICE_OK - устройство ответило на свой адрес и передало все данные
//				 BOOT_I2C_NO_DEVICE - устройства нет на шине I2C
//*****************************************
uint8_t BL_PROT_SendData(uint8_t *buf, uint32_t size){

	if(_bl_prot_StartAndSendDeviceAddr(BOOT_I2C_WRITE) == BOOT_I2C_NO_DEVICE) return BOOT_I2C_NO_DEVICE;
	_bl_prot_SendBuf(buf, size);
	return BOOT_I2C_DEVICE_OK;
}
//*******************************************************************************************
// Function    : BL_PROT_XorBuf()
// Description : расчет контрольной суммы. Полученные блоки байтов данных подвергаются операции XOR.
//			     Байт, содержащий вычисленное XOR всех предыдущих байтов, добавляется в конец каждого
//				 сообщения (байт контрольной суммы). При выполнении XOR всех полученных байтов данных и
//				 контрольной суммы результат в конце пакета должен быть 0x00.
// Parameters  : buf  - указатель на буфер
//				 size - кол-во байтов для расчета, от 1 байта
// RetVal      : XOR байтов
//*****************************************
uint8_t BL_PROT_GetXorChecksum(uint8_t *buf, uint32_t size){

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
// Function    : BL_PROT_Loop()
// Description : Оснеовной цикл эмулятора протокола загрузчика.
//				 Тут генерируются команды для передачи прошивки загрузчику.
// Parameters  :
// RetVal      :
//*****************************************
void BL_PROT_BaseLoop(void){

		//Машина состояний эмулятора для загрузчика
	static uint32_t bootState = CMD_BOOT_GetVersion;						//
	static uint8_t  bootBuf[FLASH_PAGE_SIZE + 8] = {0};	//

	switch(bootState){
		//------------------
		case(CMD_BOOT_GetVersion):
			BL_PROT_SendCmd(CMD_BOOT_GetVersion);								//Передача команды
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK) goto END_CMD_BOOT_GetVersion; //Проверим ответ.

			//Чтение версии загрузчика
			BL_PROT_ReceiveData(bootBuf, 1);

			//Проверим ответ.
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK)
			{
				//Обработка NACK
			}
			END_CMD_BOOT_GetVersion:
			//bootState = CMD_BOOT_WM;
		break;
		//------------------
		case(CMD_BOOT_WM):
			//Команда Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
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

			BL_PROT_SendCmd(CMD_BOOT_WM);								//Передача команды
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK) goto END_CMD_BOOT_WM; //Проверим ответ.

			//Предадим адрес куда хотим записать приложение
			bootBuf[0] = (uint8_t)(APP_PROGRAM_START_ADDR >> 24);
			bootBuf[1] = (uint8_t)(APP_PROGRAM_START_ADDR >> 16);
			bootBuf[2] = (uint8_t)(APP_PROGRAM_START_ADDR >> 8);
			bootBuf[3] = (uint8_t)(APP_PROGRAM_START_ADDR >> 0);
			//Checksum: XOR (byte 0, byte 1, byte 2, byte 3)
			bootBuf[4] = BL_PROT_GetXorChecksum(bootBuf, 4);

			BL_PROT_SendData(bootBuf, 4);							   //Передаим адрес
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK) goto END_CMD_BOOT_WM;//Проверим ответ.

			//Send data frame:
			bootBuf[0] = 8;																//number of bytes to be writen (0 < N <= 255)
			STM32_Flash_ReadBuf((void*)APP_PROGRAM_START_ADDR, &bootBuf[1], bootBuf[0]);//N+1 data to be written(max 256 bytes)
			bootBuf[bootBuf[0] + 1] = BL_PROT_GetXorChecksum(bootBuf, bootBuf[0]+1);	//Checksum byte: XOR (N, N+1 data bytes)

			BL_PROT_SendData(bootBuf, bootBuf[0]+2);//Передаим фрейм данных
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK)	//Проверим ответ.
			{
				//Обработка NACK
			}

			END_CMD_BOOT_WM:
			bootState = CMD_BOOT_GetVersion;
		break;
		//------------------
		case(2):

		break;
		//------------------
		default:

		break;
		//------------------
	}
	//***********************************************
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************













