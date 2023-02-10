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

static uint8_t  flashBuf[FLASH_PAGE_SIZE] = {0}; //буфер для принятых данных (= странице памяти)
static uint32_t writePageCount = 0;				 //счетчик записанных страниц, используется для вычисления актуального адреса записи
static uint32_t readPageCount  = 0;				 //счетчик записанных страниц, используется для вычисления актуального адреса записи
static uint32_t appSize_Bytes  = 0;              //размер приложения в байтах

static uint32_t bufU32[256] = {0};
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
//*******************************************************************************************
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
//*******************************************************************************************
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
//*******************************************************************************************
uint32_t GetAppState(){

  return STM32_Flash_ReadWord(APP_CONDITION_ADDR);
}
//*******************************************************************************************

//************************************************************
static void GoToApp(uint32_t appAddr){

	void(*goToApp)(void);
	uint32_t addrResetHandler;
	//------------------
	//Дополнительная проверка:
	//по стартовому адресу приложения должно лежать значение вершины стека приложения
	//т.е. значение больше 0x2000 0000
	if((STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR) & 0x2FF30000) != 0x20000000) return;

	__disable_irq();
	__set_MSP(*(volatile uint32_t*)appAddr);               	//Устанавливаем указатель стека SP приложения
	addrResetHandler = *(volatile uint32_t*)(appAddr + 4); 	//Адрес функции Reset_Handler приложения
	goToApp = (void(*)(void))addrResetHandler; 			  	//Указатель на функцию Reset_Handler приложения
	goToApp();								   				//Переход на функцию Reset_Handler приложения
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	__disable_irq();

	STM32_Clock_Init();
	GPIO_Init();
	DELAY_Init();

//	I2C_Master_Init(I2C1, I2C_GPIO_NOREMAP, 400000);
	USART_DMA_Init(USART1, 57600);

//	__enable_irq();
	//***********************************************
	//Мигнем три раза - индикация запуска загрузчика.
	for(uint32_t i = 0; i < 3; i++)
	{
		DELAY_milliS(250);
		LED_PC13_On();
		DELAY_milliS(250);
		LED_PC13_Off();
	}
	DELAY_milliS(1000);
	//***********************************************
	//Подсчитаем сколько байт занимает приложение.
	while(STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR + appSize_Bytes) != 0xFFFFFFFF)
	{
		appSize_Bytes++;
	}
	//в appSize_Bytes лежит размер приложения в байтах.
	//Округлим в большую сторону и сделаем кратным 4м
	while((appSize_Bytes % 4) != 0)
	{
		appSize_Bytes++;
	}
	//appSize_Bytes = appSize_Bytes % 4; //проверка

	STM32_Flash_ReadBuf(APP_PROGRAM_START_ADDR, flashBuf, FLASH_PAGE_SIZE);

	//***********************************************
	//Передадим одну страницу памяти по USART
	//appSize_Bytes /= 4;
	//USART1_TX -> DMA1_Channel4
	//USART2_TX -> DMA1_Channel7
	//DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
	DMAxChxStartTx(DMA1_Channel4, flashBuf, FLASH_PAGE_SIZE);






	//Очистка всех страниц пямяти приложения
//	STM32_Flash_Unlock();
//	for(uint32_t i=0; i < (FLASH_PAGES_NUM - MAIN_PROGRAM_FLASH_PAGE_NUM); i++)
//	{
//		STM32_Flash_ErasePage(MAIN_PROGRAM_START_ADDR + FLASH_PAGE_SIZE * i);
//	}
//	STM32_Flash_Lock();
	//***********************************************
	//Тест1 - запись буфера в 1024 байта во шлэш. - Работает!!
//	for(uint32_t i=0; i < FLASH_BUF_SIZE; i++)//заполнение буфера числами от 0 до 1023
//	{
//		flashBuf[i] = i;
//	}
//
//	STM32_Flash_Unlock();
//
//	//Заполение всех страниц пямяти приложения
//	for(uint32_t i=0; i < (FLASH_PAGES_NUM - MAIN_PROGRAM_FLASH_PAGE_NUM); i++)
//	{
//		//STM32_Flash_Unlock();
//		//STM32_Flash_ErasePage(MAIN_PROGRAM_START_ADDR);
//		//STM32_Flash_WriteBuf(flashBuf, (uint32_t*)MAIN_PROGRAM_START_ADDR, FLASH_BUF_SIZE);
//		//STM32_Flash_Lock();
//
//		STM32_Flash_WriteBuf(flashBuf, (uint32_t*)(MAIN_PROGRAM_START_ADDR + FLASH_PAGE_SIZE * i), FLASH_BUF_SIZE);
//	}
//
//	//Очистка всех страниц пямяти приложения
//	for(uint32_t i=0; i < (FLASH_PAGES_NUM - MAIN_PROGRAM_FLASH_PAGE_NUM); i++)
//	{
//		STM32_Flash_ErasePage(MAIN_PROGRAM_START_ADDR + FLASH_PAGE_SIZE * i);
//	}
//
//	STM32_Flash_Lock();

//	SYS_TICK_Control(SYS_TICK_ON);
//	__enable_irq();

	//Переход на основное приложение.
	GoToApp(APP_PROGRAM_START_ADDR);
	//**************************************************************
	while(1)
	{
		LED_PC13_Toggel();
		DELAY_milliS(50);
		//__WFI();//Sleep
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





