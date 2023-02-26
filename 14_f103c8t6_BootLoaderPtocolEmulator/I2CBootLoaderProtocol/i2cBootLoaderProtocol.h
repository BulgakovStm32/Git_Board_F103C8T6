/*
 * i2cBootLoaderProtocol.h
 *
 *  Created on: 10 февр. 2023 г.
 *      Author: belyaev
 */

#ifndef _I2C_BOOTLOADER_PROTOCOL_H_
#define _I2C_BOOTLOADERP_ROTOCOL_H_
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
//Параметры загрузчика:
//Интерфейс        			: I2C (I2C1 или I2C зависит от устройства)
//Скорость        			: 400кГц (возможно лучше 100кГц для лучшей надежности)
//Переход в режим загрузчика: при старте МК первым запускается З
//Начальный адрес  			: 0х0800 0000
//Размер	         		: 10КБ(10240 байт = 0х2800)
//							  из них 9КБ - это сам загрузчик,
//      						     1КБ - это обасть хранения состояния приложения.

#define BOOT_SIZE				(9 * 1024) 					//размер загрузчика, в байтах
#define APP_CONDITION_SIZE		(1 * 1024) 					//размер области хранения условий запуска основного приложения, в байтах
#define APP_CONDITION_ADDR  	(FLASH_BASE + BOOT_SIZE) 	//адрес обасти хранения состояния приложения

//Состояния основного приложения
#define APP_NO					0xFFFFFFFF	//приложение отсутствует
#define APP_OK_AND_START 		0xAAAA0001	//CRC в норме, можно запускать
#define APP_REWRITE_ME			0xAAAA0002	//была команда на переход в загрузчик после ресета
#define APP_CRC_ERR 			0xAAAA001F	//ошибка CRC

//Праметры приложения:
//Начальный адрес: 0х0800 0000 + 0х2800 = 0х0800 2800
//Размер         : размер_флеш_памяти_МК - размер_загрузчика
//Условие перехода из загрузчика в приложение: таймаут (1 сек.)
//                                             по команде (команда Go загрузчика)

#define APP_PROGRAM_START_ADDR	(FLASH_BASE + BOOT_SIZE + APP_CONDITION_SIZE)  //начальный адрес приложения
//**********************************
#define BOOT_I2C				I2C1
#define BOOT_I2C_SPEED			400000
//#define BOOT_I2C_ADDR			(0b0111001 << 1) //адрес встроенных I2C загрузчиков STM32F411
#define BOOT_I2C_ADDR			(0b0111000 << 1) //адрес самописного I2C загрузчика для STM32

#define BOOT_I2C_Wr				I2C_Wr	//
#define BOOT_I2C_Rd				I2C_Rd	//

#define BOOT_I2C_DEVICE_OK		0x01	//устройство ответило на свой адрес
#define BOOT_I2C_NO_DEVICE		0x00	//устройства нет на шине I2C

//Состояния исполнения команад
#define CMD_ACK					0x79	//пакет принят   (команда выполнена)
#define CMD_NACK				0x1F	//пакет отброшен (команда не выполнена)
#define CMD_BUSY				0x76	//состояние занятости (команда в процессе выполнения)

//Команды загрузчика
#define CMD_BOOT_Get			0x00	//Получает версию и разрешенные команды, поддерживаемые текущей версией загрузчика.
#define CMD_BOOT_GetVersion		0x01	//Получает версию загрузчика.
#define CMD_BOOT_GetID			0x02	//Получает идентификатор чипа
#define CMD_BOOT_RM				0x11	//Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
#define CMD_BOOT_Go				0x21	//Переходит к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
#define CMD_BOOT_WM				0x31	//Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
#define CMD_BOOT_NS_WM			0x32	//No-Stretch Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением, и возвращает состояние занятости во время выполнения операции.
#define CMD_BOOT_Erase			0x44	//Стирает от одной до всех страниц или секторов флэш-памяти, используя режим двухбайтовой адресации.
#define CMD_BOOT_NS_Erase		0x45	//Стирает от одной до всех страниц или секторов флэш-памяти, используя режим двухбайтовой адресации, и возвращает состояние занятости во время выполнения операции.
#define CMD_BOOT_Special		0x50	//Общая команда, которая позволяет добавлять новые функции в зависимости от ограничений продукта, не добавляя новую команду для каждой функции.
#define CMD_BOOT_ExtSpecial		0x51	//Extended Special - Общая команда, которая позволяет пользователю отправлять больше данных по сравнению со специальной командой.
#define CMD_BOOT_WP				0x63	//Write Protect - Включает защиту от записи для некоторых секторов.
#define CMD_BOOT_NS_WP			0x64	//No-Stretch Write Protect - Включает защиту от записи для некоторых секторов и возвращает состояние занятости во время выполнения операции.
#define CMD_BOOT_WUP			0x73	//Write Unprotect - Отключает защиту от записи для всех секторов флэш-памяти.
#define CMD_BOOT_NS_WUP			0x74	//No-Stretch Write Unprotect - Отключает защиту от записи для всех секторов флэш-памяти и возвращает состояние занятости во время выполнения операции.
#define CMD_BOOT_RP				0x82	//Readout Protect - Включает защиту от чтения.
#define CMD_BOOT_NS_RP			0x83	//No-Stretch Readout Protect - Включает защиту от чтения и возвращает состояние занятости во время выполнения операции.
#define CMD_BOOT_RUP			0x92	//Readout Unprotect - Отключает защиту от чтения.
#define CMD_BOOT_NS_RUP			0x93	//No-Stretch Readout Unprotect - Отключает защиту от чтения и возвращает состояние занятости во время выполнения операции.
#define CMD_BOOT_NS_GetMemCs	0xA1	//No-Stretch Get Memory Checksum - Получает значение контрольной суммы CRC для области памяти на основе ее смещения и длины.

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void 	BL_EMULATOR_I2CInit(void);
uint8_t BL_EMULATOR_CheckDevice(void);
uint8_t BL_EMULATOR_SendCmd(uint8_t cmd);
uint8_t BL_EMULATOR_WaitACK(void);
uint8_t BL_EMULATOR_ReceiveData(uint8_t *buf, uint32_t size);
uint8_t BL_EMULATOR_SendData(uint8_t *buf, uint32_t size);
uint8_t BL_EMULATOR_GetXorChecksum(uint8_t *buf, uint32_t size);


uint8_t  BL_EMULATOR_Cmd_Get(void);
uint8_t  BL_EMULATOR_Cmd_GetVersion(void);
uint16_t BL_EMULATOR_Cmd_GetID(void);
uint8_t  BL_EMULATOR_Cmd_RM(uint32_t addr, uint32_t size);
uint8_t  BL_EMULATOR_Cmd_Go(uint32_t appAddr);
uint8_t  BL_EMULATOR_Cmd_WM(uint32_t addr, uint8_t* wrBuf, uint32_t size);

uint32_t BL_EMULATOR_BaseLoop(void);

//*******************************************************************************************
//*******************************************************************************************
#endif /* _I2C_BOOTLOADERP_ROTOCOL_H_ */
