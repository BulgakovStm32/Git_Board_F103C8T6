/*
 * flash_ST.c
 *
 *  Created on: 9 авг. 2022 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

#include "flash_ST.h"

//*******************************************************************************************
//*******************************************************************************************



//*******************************************************************************************
//*******************************************************************************************
//Блокировка flash.
void STM32_Flash_Lock(void){

  FLASH->CR |= FLASH_CR_LOCK;
}
//*****************************************************************************
//Разблокировка работы с flash.
void STM32_Flash_Unlock(void){

  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}
//*****************************************************************************
//Стирание страницы flash.
//Перед записью необходимо стереть данные по нужным адресам,
//это особенность флеша.
//pageAddress - любой адрес, принадлежащий стираемой странице
void STM32_Flash_ErasePage(uint32_t pageAddress){

	while(FLASH->SR & FLASH_SR_BSY){};                     //Ждем окончания работы с памятю.
	if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP; //Сбрасывается бит EOP записью в него единицы.
	FLASH->CR |= FLASH_CR_PER;  //стирания страницы.
	FLASH->AR  = pageAddress;   //адрес стираемой страницы.
	FLASH->CR |= FLASH_CR_STRT; //Запуск выбранной операции
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR  =  FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}
//*****************************************************************************
void STM32_Flash_WriteWord(uint32_t word, uint32_t address){

	while(FLASH->SR & FLASH_SR_BSY){};                     //Ждем окончания работы с памятю.
	if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP; //Сбрасывается бит EOP записью в него единицы.

	//разрешение зиписи во флеш.
	FLASH->CR |= FLASH_CR_PG;

	//Программировать память можно только 16 битными словами (uint16_t).
	//Адрес, куда производится запись, также должен быть выровнен на 2 байта.
	*(__IO uint16_t*)address = (uint16_t)word;
	while(!(FLASH->SR & FLASH_SR_EOP)){};     //Ждем завершения записи
	FLASH->SR = FLASH_SR_EOP;

	*(__IO uint16_t*)(address + 2) = (uint16_t)(word >> 16);
	while(!(FLASH->SR & FLASH_SR_EOP)){};
	FLASH->SR = FLASH_SR_EOP;

	//Запрет записи во флэш.
	FLASH->CR &= ~(FLASH_CR_PG);
}
//*****************************************************************************
//Чтения 32-хбитного слова из FLASH.
uint32_t STM32_Flash_ReadWord(uint32_t address){

	return (*(__IO uint32_t*)address);
}
//*****************************************************************************
/*
 * Src  - буфер-источник данных для записи
 * Dst  - адрес странициы во влеш памяти, в которую будет производится зпись
 * size - кол-во байт на запись. Должно быть кратно 2м.
 */
void STM32_Flash_WriteBuf(void* Src, void* Dst, uint32_t size){

		     uint16_t* SrcW = (uint16_t*)Src;
	volatile uint16_t* DstW = (uint16_t*)Dst;
	//-----------------------
	while(FLASH->SR & FLASH_SR_BSY){};  				   //Ждем окончания работы с памятю.
	if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP; //Сбрасывается бит EOP записью в него единицы.
	FLASH->CR |= FLASH_CR_PG;							   //разрешение зиписи во флеш.

	while(size)
	{
		*DstW = *SrcW;							//пишем во флэш
		while((FLASH->SR & FLASH_SR_BSY) != 0); //Ждем завершения записи

		if(*DstW != *SrcW) goto EndPrg;

		DstW++;
		SrcW++;
		size = size - sizeof(uint16_t);
	}
	EndPrg:
	FLASH->CR &= ~FLASH_CR_PG; //Запрет записи во флэш.
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************


//*****************************************************************************


//*****************************************************************************


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************



















