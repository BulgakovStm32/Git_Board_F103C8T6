/*
 * 	main.c
 *
 *  Created on: 19 октября 2021 года.
 *  Autho     : Беляев А.А.
 *
 *	Описание        : I2C Boot Loader
 *  Датчики         :
 *  Вывод информации:
 *
 */
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	__disable_irq();
	STM32_Clock_Init();
	GPIO_Init();
	DELAY_Init();
	BOOT_Init();
	__enable_irq();
	//***********************************************
	//Мигнем три раза - индикация запуска загрузчика.
	for(uint32_t i = 0; i < 3; i++)
	{
		DELAY_milliS(50);
		LED_PC13_On();
		DELAY_milliS(50);
		LED_PC13_Off();
	}
	//***********************************************
	//TODO ... Во время работы приложения пришла команда на обновление прошивки
	//if(BOOT_GetAppLaunch() == APP_REWRITE_ME) goto BOOT;	//переход в загрузчик

	//Подсчитаем сколько байт занимает приложение.
	uint32_t appSize = BOOT_GetAppSize();
	if(appSize == 0) goto BOOT;	//0 - приложени отсутствует => переход в загрузчик

	//Проверим crc приложения.
	uint32_t appCrc = BOOT_CalcCrc((uint32_t*)APP_PROGRAM_START_ADDR , appSize);
	if(appCrc != BOOT_ReadAppCrc()) goto BOOT;	//Приложение битое => переход в загрузчик

	//По истечении таймаута переходим в приложение
	uint32_t micros = DELAY_microSecCount();
	while(BOOT_GetStateI2C()  != I2C_IT_STATE_ADDR_WR && //нет запросов от хоста по I2C
		  BOOT_GetAppLaunch() == APP_OK_AND_START)		 //есть признак запуска приложения
	{
		//Отсчет таймаута
		if((DELAY_microSecCount() - micros) >= (BOOT_I2C_TIMEOUT_MS*1000))
		{
			//Переход в приложение
			BOOT_GoToApp(APP_PROGRAM_START_ADDR);
		}
	}
	//***********************************************
	BOOT:
	IWDG_Init(3000); //Период сторожевого таймера 3 сек.
	while(1)
	{
		BOOT_Loop(); //Загрузчик
		IWDG_Reset();
	}
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
//void SysTick_IT_Handler(void){
//
//}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************






