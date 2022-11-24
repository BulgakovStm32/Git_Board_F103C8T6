//********************************************************************************************
//********************************************************************************************

#include "Config.h"

//********************************************************************************************
//********************************************************************************************

static DataForFLASH_t DataForFLASHStr;//Конфигурация блока.
static uint32_t 	  configSize = 0;

//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
void _config_SaveToFlash(uint32_t pageAddr, uint32_t size){

	uint32_t index = 0;
	//--------------------
	STM32_Flash_Unlock();
	STM32_Flash_ErasePage(CONFIG_FLASH_PAGE);
	while(index < size)
	{
		STM32_Flash_WriteWord(DataForFLASHStr.data32[index], pageAddr);
		index    += 1;
		pageAddr += 4;
	}
	STM32_Flash_Lock();
}
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//Конфигурация блока.
Config_t* Config(void){

	return &DataForFLASHStr.config;
}
//*****************************************************************************
//сохранение конфигурации.
Config_t* Config_Save(void){

	DataForFLASHStr.sector.ChangeConfig += 1;//Признак изменения конфигурации. Это нужно для запись конфигурации после ее изменения.
	return &DataForFLASHStr.config;
}
//*****************************************************************************
void Config_Init(void){
	
	uint32_t pageAddr = CONFIG_FLASH_PAGE;
	uint32_t index = 0;
	//uint32_t size  = 0;
	//uint32_t error = 0;
	//--------------------
	//размер вычитываемых данных.
	configSize = sizeof(DataForFLASHStr.config) / 4;
	if(sizeof(DataForFLASHStr.config) % 4) configSize += 1;
	//чтение данных из FLASH памяти.
	while(index < configSize)
	{
		DataForFLASHStr.data32[index] = STM32_Flash_ReadWord(pageAddr);
		index    += 1;
		pageAddr += 4;
	}
	//--------------------
	//если проверочный ключ не совпадает, значит первый запуск блока и записываем заводские настройки.
	if(DataForFLASHStr.config.checkKey != CONFIG_CHECK_KEY_DEFINE)
	{
		//Запись заводских установок во флеш.
		DataForFLASHStr.config.checkKey = CONFIG_CHECK_KEY_DEFINE;

		strcpy(DataForFLASHStr.config.name, "Si5351_002");

		DataForFLASHStr.config.xtalFreq = 25000 * 1000;

		strcpy(DataForFLASHStr.config.SW, "SW01");
		strcpy(DataForFLASHStr.config.HW, "HW01");

		DataForFLASHStr.config.Address = 1;
		DataForFLASHStr.config.Group   = 1;

//		DataForFLASHStr.config.PowerCheckOn = POWER_ALL_CHECK_ON;
//		DataForFLASHStr.config.SpDeviation  = SP_LINE_DEVIATION_DEFAULT;
//		DataForFLASHStr.config.SpCheck      = SP_LINE_ALL_OFF;
//
//		for(index = 0; index < FIRE_LINES_NUMBER; index++)
//		{
//			DataForFLASHStr.config.FireLineConfig[index].Type    = DEFAULT_FIRE_LINES_INPUT_TYPE;
//			DataForFLASHStr.config.FireLineConfig[index].Timeout = DEFAULT_FIRE_LINES_INPUT_TIMEOUT;
//		}
		//--------------------
		//пишем данные в память.
		//pageAddr = CONFIG_FLASH_PAGE;
		//index    = 0;
		DataForFLASHStr.sector.Nwrite = 1;
		_config_SaveToFlash(CONFIG_FLASH_PAGE, configSize);

//		STM32_Flash_Unlock();
//		STM32_Flash_ErasePage(CONFIG_FLASH_PAGE);
//		while(index < size)
//		{
//			STM32_Flash_WriteWord(DataForFLASHStr.data32[index], pageAddr);
//			index    += 1;
//			pageAddr += 4;
//		}
//		STM32_Flash_Lock();
		//--------------------
	}
}
//*****************************************************************************
//Запись конфигурации после ее изменения.
void Config_SaveLoop(void){
	
	//если были редактирования конфигурации, то запишем конфигурацию во FLASH.
	if(DataForFLASHStr.sector.ChangeConfig != 0)
	{
		DataForFLASHStr.sector.ChangeConfig = 0;
		DataForFLASHStr.sector.Nwrite      += 1;
		//пишем данные в память.
		//__disable_irq();
		_config_SaveToFlash(CONFIG_FLASH_PAGE, configSize);

//		STM32_Flash_Unlock();
//		STM32_Flash_ErasePage(CONFIG_FLASH_PAGE);
//		while(index < size)
//		{
//			STM32_Flash_WriteWord(DataForFLASHStr.data32[index], pageAddr);
//			index    += 1;
//			pageAddr += 4;
//		}
//		STM32_Flash_Lock();
		//--------------------
		//__enable_irq();
	}
}
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
