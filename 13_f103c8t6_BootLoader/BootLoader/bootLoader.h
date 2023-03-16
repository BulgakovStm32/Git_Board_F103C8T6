/*
 * bootLoader.h
 *
 *  Created on: 10 февр. 2023 г.
 *      Author: belyaev
 */

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_
//*******************************************************************************************
//*******************************************************************************************

#include "i2c_ST.h"
#include "gpio_ST.h"
#include "flash_ST.h"
#include "iwdg_ST.h"
#include "crc_ST.h"
#include "Delay.h"

//*******************************************************************************************
//*******************************************************************************************
//Параметры загрузчика:
//Интерфейс        			: I2C (I2C1 или I2C зависит от устройства)
//Скорость        			: 400кГц (возможно лучше 100кГц для лучшей надежности)
//Переход в режим загрузчика: при старте МК первым запускается загрузчик
//Начальный адрес  			: 0х0800 0000
//Размер	         		: 10КБ(10240 байт = 0х2800)
//							  из них 9КБ - это сам загрузчик,
//      						     1КБ - это обасть хранения состояния приложения.

#define BOOT_I2C					I2C2 		//Используемый порт I2C
#define BOOT_I2C_SPEED				400000		//Скорость I2C
#define BOOT_I2C_ADDR				0b0111000 	//Адрес загрузчика на шине I2C. такой адрес у встроенных I2C загрузчиков STM32
#define BOOT_I2C_VERSION			0x01		//Версия загрузчика (0х01 значит версия 0.1)
#define BOOT_I2C_TIMEOUT_MS			1000		//Таймаута ожидания запросов от Хоста. Если нет запросов к загрузчику
												//и истек таймаут, то будет переход в приложение.

#define BOOT_SIZE					(9 * 1024) 					//размер загрузчика, в байтах
#define APP_STATE_SIZE				(1 * 1024) 					//размер области хранения условий запуска основного приложения, в байтах
#define APP_STATE_ADDR  			(FLASH_BASE + BOOT_SIZE) 	//адрес обасти хранения доп. информации о пилложении
#define APP_LAUNCH_CONDITIONS_ADDR	(APP_STATE_ADDR + 0)		//Условие запуска приложения
#define APP_CRC_ADDR				(APP_STATE_ADDR + 4)		//Контрольная сумма приложения

//**********************************
//Параметры приложения:
//Начальный адрес: 0х0800 0000 + 0х2800 = 0х0800 2800
//Размер         : размер_флеш_памяти_МК минус размер_загрузчика
//Условие перехода из загрузчика в приложение: таймаут (1 сек.)(если есть приложение)
//                                             по команде (команда Go загрузчика)
#define APP_PROGRAM_START_ADDR	(FLASH_BASE + BOOT_SIZE + APP_STATE_SIZE)  //начальный адрес приложения

//Условие запуска приложения
#define APP_NO				0xFFFFFFFF	//приложение отсутствует
#define APP_OK_AND_START	0xAAAA0001	//CRC в норме, можно запускать
#define APP_REWRITE_ME		0xAAAA0002	//была команда на переход в загрузчик после ресета
#define APP_CRC_ERR			0xAAAA001F	//ошибка CRC
//**********************************
//Состояния исполнения команад
#define CMD_ACK					0x79	//пакет принят   (команда выполнена)
#define CMD_NACK				0x1F	//пакет отброшен (команда не выполнена)
#define CMD_BUSY				0x76	//состояние занятости BUSY (команда в процессе выполнения)
//**********************************
//Команды загрузчика - Реализованные
#define CMD_BOOT_Get			0x00	//Получает версию и разрешенные команды, поддерживаемые текущей версией загрузчика.
#define CMD_BOOT_GetVersion		0x01	//Получает версию загрузчика.
#define CMD_BOOT_GetID			0x02	//Получает идентификатор чипа
#define CMD_BOOT_RM				0x11	//Read Memory - Читает до 256 байт памяти, начиная с адреса, указанного приложением.
#define CMD_BOOT_GO				0x21	//Переходит к коду пользовательского приложения, расположенному во внутренней флэш-памяти.
#define CMD_BOOT_WM				0x31	//Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
#define CMD_BOOT_ERASE			0x44	//Стирает от одной до всех страниц или секторов флэш-памяти, используя режим двухбайтовой адресации.
#define CMD_BOOT_RP				0x82	//Readout Protect - Включает защиту от чтения.
#define CMD_BOOT_RUP			0x92	//Readout Unprotect - Отключает защиту от чтения.
#define CMD_BOOT_NS_GetCheckSum	0xA1	//No-Stretch Get Memory Checksum - Получает значение контрольной суммы CRC для области памяти.

//Нереализованные
//#define CMD_BOOT_NS_WM			0x32	//No-Stretch Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением, и возвращает состояние занятости во время выполнения операции.
//#define CMD_BOOT_NS_Erase			0x45	//Стирает от одной до всех страниц или секторов флэш-памяти, используя режим двухбайтовой адресации, и возвращает состояние занятости во время выполнения операции.
//#define CMD_BOOT_Special			0x50	//Общая команда, которая позволяет добавлять новые функции в зависимости от ограничений продукта, не добавляя новую команду для каждой функции.
//#define CMD_BOOT_ExtSpecial		0x51	//Extended Special - Общая команда, которая позволяет пользователю отправлять больше данных по сравнению со специальной командой.
//
//#define CMD_BOOT_WP				0x63	//Write Protect - Включает защиту от записи для некоторых секторов.
//#define CMD_BOOT_NS_WP			0x64	//No-Stretch Write Protect - Включает защиту от записи для некоторых секторов и возвращает состояние занятости во время выполнения операции.
//
//#define CMD_BOOT_WUP				0x73	//Write Unprotect - Отключает защиту от записи для всех секторов флэш-памяти.
//#define CMD_BOOT_NS_WUP			0x74	//No-Stretch Write Unprotect - Отключает защиту от записи для всех секторов флэш-памяти и возвращает состояние занятости во время выполнения операции.
//
//#define CMD_BOOT_NS_RP			0x83	//No-Stretch Readout Protect - Включает защиту от чтения и возвращает состояние занятости во время выполнения операции.
//#define CMD_BOOT_NS_RUP			0x93	//No-Stretch Readout Unprotect - Отключает защиту от чтения и возвращает состояние занятости во время выполнения операции.
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void BOOT_Init(void);
void BOOT_DeinitAll(void);

uint32_t 		BOOT_AppAvailableCheck(uint32_t appAddr);
uint32_t 	 	BOOT_GetAppSize(void);
I2C_IT_State_t	BOOT_GetStateI2C(void);
void 			BOOT_GoToApp(uint32_t appAddr);

void 	 BOOT_SetAppLaunch(uint32_t launch);
uint32_t BOOT_GetAppLaunch(void);

uint32_t BOOT_CalcCrc(uint32_t *addr, uint32_t size);
uint32_t BOOT_ReadAppCrc(void);
void 	 BOOT_CalcAndWriteAppCrc(void);

void BOOT_ErasePageAppState(void);

void BOOT_Loop(void);
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
#endif /* _BOOTLOADER_H_ */















