/*
 *  Config.h
 *
 *  Created on:
 *      Author: Zver
 */
#ifndef _Config_H
#define _Config_H
//*******************************************************************************************
//*******************************************************************************************

//#include "stm32f10x.h"
//#include <string.h>
#include "main.h"

//*******************************************************************************************
#define CONFIG_CHECK_KEY_DEFINE 0x12345678 						  //
#define CONFIG_SIZE_U32 	    ((sizeof(DataForFLASH_t) / 4) + 1)//Размер в 32-хбитных словах.
#define CONFIG_FLASH_PAGE 		FLASH_PAGE_127					  //

//**********************************
#pragma pack(push, 1)//размер выравнивания в 1 байт
/***********Конфигурация***********/
typedef struct{
	uint32_t checkKey;//проверочная последовательность
	char 	 name[16];//Название проекта
	uint32_t xtalFreq;//Частота кварца.
	uint32_t stepFreq;
	uint32_t timeUTC; //
	//----------
	char SW[4];//Версия ПО
	char HW[4];//Версия железа
	//----------
	uint8_t Address;//адрес блока.
	uint8_t Group;	 //группа блока.
	//----------
	//Установленные на контроль линии питания блока.
	uint8_t PowerCheckOn;
	//----------
	//Параметры линий Гр.
	uint16_t SpDeviation;//Отклонение для фиксации неисправности на линии Гр.
	uint8_t  SpCheck;    //Установленные на контроль линии Гр.
	//----------
}Config_t;
//#pragma pack(pop)//вернули предыдущую настройку.
//**********************************
//
typedef struct{
	uint8_t  data[sizeof(Config_t)];//данные, записываемые в сектор памяти.
	uint32_t numWrite;         		//количество записей в данный секетор памяти.
	uint32_t checkSum;      		//контрольная сумма
}STM32_FLASH_Sector_t;
//**********************************
//
typedef union{
	Config_t             config;
	STM32_FLASH_Sector_t sector;
	uint32_t             data32[(sizeof(STM32_FLASH_Sector_t)/4) + 1];
}DataForFLASH_t;
//**********************************
#pragma pack(pop)//вернули предыдущую настройку.
//*******************************************************************************************
//*******************************************************************************************
Config_t* Config     (void);
Config_t* Config_Save(void);
void      Config_Init(void);
void      Config_SaveLoop(void);

//*******************************************************************************************
//*******************************************************************************************
#endif /*_Config_H*/














