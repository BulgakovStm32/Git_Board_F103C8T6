/*
 * 	main.c
 *
 *  Created on: 19 октября 2021 года.
 *  Autho     : Беляев А.А.
 *
 *	Описание        :
 *  Датчики         :
 *  Вывод информации:
 *
 */
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
//static uint8_t  flashBuf[FLASH_PAGE_SIZE] = {0}; //буфер для принятых данных (= странице памяти)
//static uint32_t writePageCount = 0;				 //счетчик записанных страниц, используется для вычисления актуального адреса записи
//static uint32_t readPageCount  = 0;				 //счетчик записанных страниц, используется для вычисления актуального адреса записи
static uint32_t appSize_Bytes  = 0;              //размер приложения в байтах

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//static void _initAll(void){
//
//	STM32_Clock_Init();
//	GPIO_Init();
//	DELAY_Init();
//}
//*******************************************************************************************
// Function      SetAppState()
// Description   Sets bootloader key
// Parameters    None
// RetVal        None
//*****************************************
void SetAppState(uint32_t state){

	STM32_Flash_Unlock();
	STM32_Flash_WriteWord(state, APP_CONDITION_ADDR);
	STM32_Flash_Lock();
}
//*******************************************************************************************
// Function      ResetKey()
// Description   Resets bootloader key
// Parameters    None
// RetVal        None
//*****************************************
void ResetAppState(){

	STM32_Flash_Unlock();
	STM32_Flash_ErasePage(APP_CONDITION_ADDR);
	STM32_Flash_Lock();
}
//*******************************************************************************************
// Function      GetAppState()
// Description   Reads bootloader key value
// Parameters    None
// RetVal        None
//*****************************************
uint32_t GetAppState(){

  return STM32_Flash_ReadWord(APP_CONDITION_ADDR);
}
//*******************************************************************************************
// Function   : AppAvailableCheck()
// Description: по стартовому адресу приложения должно лежать значение вершины стека
//			    приложения, т.е. значение больше 0x2000 0000. Если это не так, значит
//				приложение отсутсвует
// Parameters : Нет
// RetVal     : 1 - приложение есть; 0 - приложения нет.
//*****************************************
//static uint32_t AppAvailableCheck(void){
//
//	if((STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR) & 0x2FFC0000) != 0x20000000) return 0;
//	return 1;
//}
//*******************************************************************************************
// Function      GoToApp
// Description
// Parameters
// RetVal
//*****************************************
//static void GoToApp(uint32_t appAddr){
//
//	void(*goToApp)(void);
//	uint32_t addrResetHandler;
//	//------------------
//	//Дополнительная проверка:
//	//по стартовому адресу приложения должно лежать значение вершины стека приложения
//	//т.е. значение больше 0x2000 0000
//	if(!AppAvailableCheck()) return;
//
//	__disable_irq();
//	__set_MSP(*(volatile uint32_t*)appAddr);               	//Устанавливаем указатель стека SP приложения
//	addrResetHandler = *(volatile uint32_t*)(appAddr + 4); 	//Адрес функции Reset_Handler приложения
//	goToApp = (void(*)(void))addrResetHandler; 			  	//Указатель на функцию Reset_Handler приложения
//	goToApp();								   				//Переход на функцию Reset_Handler приложения
//}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	__disable_irq();
	STM32_Clock_Init();
	GPIO_Init();
	DELAY_Init();
	//***********************************************
	//Мигнем три раза - индикация запуска загрузчика.
	for(uint32_t i = 0; i < 3; i++)
	{
		DELAY_milliS(250);
		LED_PC13_On();
		DELAY_milliS(250);
		LED_PC13_Off();
	}
	//***********************************************
	//Проверка наличия приложения. Если приложения нет - бесконечно мигаем.
//	if(!AppAvailableCheck())
//	{
//		while(1)
//		{
//			LED_PC13_Toggel();
//			DELAY_milliS(50);
//		}
//	}
	//***********************************************
	//Подсчитаем сколько байт занимает приложение.
	appSize_Bytes = 0;
	while(STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR + appSize_Bytes) != 0xFFFFFFFF)
	{
		appSize_Bytes += 4; //шагаем по 4 байта
	}
	//в appSize_Bytes лежит размер приложения в байтах.
	//Округлим в большую сторону и сделаем кратным 4м
	while((appSize_Bytes % 4) != 0) { appSize_Bytes++; }
	//appSize_Bytes = appSize_Bytes % 4; //проверка
	//***********************************************
	//Тут начинается работа протокола передачи прошивки.
	BL_EMULATOR_I2CInit();

	uint8_t blVersion = BL_EMULATOR_Cmd_GetVersion();//Прочитаем версию загрузчика.
	//Ждем ответа загрузчика.
	while(blVersion == CMD_NACK)
	{
		LED_PC13_Toggel();
		DELAY_milliS(1000);
		blVersion = BL_EMULATOR_Cmd_GetVersion();//Прочитаем версию загрузчика.
	}
	LED_PC13_Off();
	//**************************************************************
	while(1)
	{
		if(BL_EMULATOR_BaseLoop() != CMD_NACK) DELAY_milliS(50);
		else 								   DELAY_milliS(500);

		//Мигаем ...
		LED_PC13_Toggel();
	}
	//**************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_IT_Handler(void){


}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************






