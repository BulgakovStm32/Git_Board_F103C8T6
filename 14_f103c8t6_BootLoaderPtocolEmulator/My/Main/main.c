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
static uint32_t AppAvailableCheck(void){

	if((STM32_Flash_ReadWord(APP_PROGRAM_START_ADDR) & 0x2FFC0000) != 0x20000000) return 0;
	return 1;
}
//*******************************************************************************************
// Function      GoToApp
// Description
// Parameters
// RetVal
//*****************************************
static void GoToApp(uint32_t appAddr){

	void(*goToApp)(void);
	uint32_t addrResetHandler;
	//------------------
	//Дополнительная проверка:
	//по стартовому адресу приложения должно лежать значение вершины стека приложения
	//т.е. значение больше 0x2000 0000
	if(!AppAvailableCheck()) return;

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
	//***********************************************
	//Мигнем три раза - индикация запуска загрузчика.
	for(uint32_t i = 0; i < 3; i++)
	{
		DELAY_milliS(100);
		LED_PC13_On();
		DELAY_milliS(100);
		LED_PC13_Off();
	}
//	DELAY_milliS(1000);
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
	BL_PROT_I2CInit();

	//Ждем ответа загрузчика
	while(BL_PROT_CheckDevice() == BOOT_I2C_NO_DEVICE)
	{
		LED_PC13_Toggel();
		DELAY_milliS(100);
	}

	//Машина состояний эмулятора для загрузчика
	static uint32_t bootState =  0;						//
	static uint8_t  bootBuf[FLASH_PAGE_SIZE + 8] = {0};	//

	switch(bootState){
		//------------------
		//Write Memory - Записывает в память до 256 байт, начиная с адреса, указанного приложением.
		case(0):
			BL_PROT_SendCmd(CMD_BOOT_WM);

			//Проверим ответ.
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK) goto END_CMD_BOOT_WM;

			//Предадим адрес куда хотим записать приложение
			bootBuf[0] = (uint8_t)(APP_PROGRAM_START_ADDR >> 0);
			bootBuf[1] = (uint8_t)(APP_PROGRAM_START_ADDR >> 8);
			bootBuf[2] = (uint8_t)(APP_PROGRAM_START_ADDR >> 16);
			bootBuf[3] = (uint8_t)(APP_PROGRAM_START_ADDR >> 24);
			bootBuf[4] = BL_PROT_GetChecksum(bootBuf, 4);	//Checksum: XOR (byte 0, byte 1, byte 2, byte 3)
			BL_PROT_SendData(bootBuf, 4);

			//Проверим ответ.
//			if(BL_PROT_WaitACK() != CMD_BOOT_ACK) goto END_CMD_BOOT_WM;

			//Send data frame:
			//- number of bytes to be writen
			//- data to be written
			//- checksum
			bootBuf[0] = 8;																//number of bytes to be writen (0 < N <= 255)
			STM32_Flash_ReadBuf((void*)APP_PROGRAM_START_ADDR, &bootBuf[1], bootBuf[0]);//N+1 data to be written(max 256 bytes)
			bootBuf[bootBuf[0] + 1] = BL_PROT_GetChecksum(bootBuf, bootBuf[0]);			//Checksum byte: XOR (N, N+1 data bytes)
			BL_PROT_SendData(bootBuf, bootBuf[0]+2);									//Send data frame

			//Wait for ACK
			if(BL_PROT_WaitACK() != CMD_BOOT_ACK)
			{
				//Обработка NACK
			}

			END_CMD_BOOT_WM:
		break;
		//------------------
		case(1):


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







	//***********************************************
	//Передадим одну страницу памяти по USART
	//appSize_Bytes /= 4;
	//USART1_TX -> DMA1_Channel4
	//USART2_TX -> DMA1_Channel7
	//DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
//	DMAxChxStartTx(DMA1_Channel4, flashBuf, FLASH_PAGE_SIZE);








//	SYS_TICK_Control(SYS_TICK_ON);
//	__enable_irq();

	//Переход на основное приложение.
//	GoToApp(APP_PROGRAM_START_ADDR);
	//**************************************************************
	while(1)
	{
		BL_PROT_SendCmd(CMD_BOOT_Get);
		if(BL_PROT_WaitACK() == CMD_BOOT_ACK) LED_PC13_Toggel();

		DELAY_milliS(250);
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






