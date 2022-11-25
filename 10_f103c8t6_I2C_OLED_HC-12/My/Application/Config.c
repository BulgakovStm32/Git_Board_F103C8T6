//********************************************************************************************
//********************************************************************************************

#include "Config.h"

//********************************************************************************************
//********************************************************************************************
static DataForFLASH_t dataForFLASH;    //Конфигурация блока.
static uint32_t 	  changeConfig = 0;//используется для отслеживания изменений в конфигурации.

//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//пишем данные в FLASH память.
void _config_SaveToFlash(uint32_t pageAddr, uint32_t size){

	uint32_t index = 0;
	//--------------------
	STM32_Flash_Unlock();
	STM32_Flash_ErasePage(CONFIG_FLASH_PAGE);
	while(index < size)
	{
		STM32_Flash_WriteWord(dataForFLASH.data32[index], pageAddr);
		index    += 1;
		pageAddr += 4;//шагаем по 4 байта
	}
	STM32_Flash_Lock();
}
//**********************************************************
//чтение данных из FLASH памяти.
void _config_ReadFromFlash(uint32_t pageAddr, uint32_t size){

	uint32_t index = 0;
	//--------------------
	while(index < size)
	{
		dataForFLASH.data32[index] = STM32_Flash_ReadWord(pageAddr);
		index    += 1;
		pageAddr += 4;//шагаем по 4 байта
	}
}
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//Конфигурация блока.
Config_t* Config(void){

	return &dataForFLASH.config;
}
//**********************************************************
//сохранение конфигурации.
Config_t* Config_Save(void){

	changeConfig += 1;//Признак изменения конфигурации.
					  //Это нужно для запись конфигурации после ее изменения.

	return &dataForFLASH.config;
}
//**********************************************************
void Config_Init(void){
	
	//чтение данных из FLASH памяти.
	_config_ReadFromFlash(CONFIG_FLASH_PAGE, CONFIG_SIZE_U32);

	//если проверочный ключ не совпадает, значит первый запуск блока и записываем заводские настройки.
	if(dataForFLASH.config.checkKey != CONFIG_CHECK_KEY_DEFINE)
	{
		//Запись заводских установок во флеш.
		dataForFLASH.config.checkKey = CONFIG_CHECK_KEY_DEFINE;

		strcpy(dataForFLASH.config.name, "0123456789ABCDEF");

		dataForFLASH.config.xtalFreq = 25000 * 1000;

		strcpy(dataForFLASH.config.SW, "SW01");
		strcpy(dataForFLASH.config.HW, "HW01");

		dataForFLASH.config.Address      = 1;
		dataForFLASH.config.Group        = 2;
		dataForFLASH.config.PowerCheckOn = 3;
		dataForFLASH.config.SpDeviation  = 4;
		dataForFLASH.config.SpCheck      = 5;
		//--------------------
		//пишем данные в память.
		dataForFLASH.sector.numWrite = 1;
		dataForFLASH.sector.checkSum = 0xAABBCCDD;
		_config_SaveToFlash(CONFIG_FLASH_PAGE, CONFIG_SIZE_U32);
	}
}
//**********************************************************
//Запись конфигурации после ее изменения.
void Config_SaveLoop(void){
	
	//если были редактирования конфигурации, то запишем конфигурацию во FLASH.
	if(changeConfig != 0)
	{
		changeConfig = 0;
		dataForFLASH.sector.numWrite += 1;
		//пишем данные в память.
		//__disable_irq();
		_config_SaveToFlash(CONFIG_FLASH_PAGE, CONFIG_SIZE_U32);
		//__enable_irq();
	}
}
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
