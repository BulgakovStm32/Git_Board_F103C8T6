/*
 * flash_ST.h
 *
 *  Created on: 9 авг. 2022 г.
 *      Author: belyaev
 */

#ifndef FLASH_ST_H
#define FLASH_ST_H
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
//Как же определить свободную страницу? Это зависит от размера прошивки.
//Обычно используют последнюю страницу в памяти.
//Её адрес можно вычислить как [начальный адрес] + [номер страницы - 1] * [размер страницы].
//Выберем 16 страницу: 0x0800 0000 + 15 * 0x0000 0400 = 0x0800 3C00

#define FLASH_SIZE 			64 //128 //Размер FLASH в кБ.
#define FLASH_PAGE_SIZE		1024 //Размер страницы FLASH в кБ.

#define FLASH_PAGE_127 (FLASH_BASE + (FLASH_PAGE_SIZE * (FLASH_SIZE - 0)))
#define FLASH_PAGE_126 (FLASH_BASE + (FLASH_PAGE_SIZE * (FLASH_SIZE	- 1)))

#define FLASH_PAGE127_END_ADDR 	(FLASH_PAGE_127 + FLASH_PAGE_SIZE)
#define FLASH_PAGE126_END_ADDR 	(FLASH_PAGE_126 + FLASH_PAGE_SIZE)
//*******************************************************************************************
//*******************************************************************************************
void     STM32_Flash_Lock     (void);
void     STM32_Flash_Unlock   (void);
void     STM32_Flash_ErasePage(uint32_t pageAddress);
void     STM32_Flash_WriteWord(uint32_t word, uint32_t address);
uint32_t STM32_Flash_ReadWord (uint32_t address);

void STM32_Flash_WriteBuf(void* Src, void* Dst, uint32_t size);
//*******************************************************************************************
//*******************************************************************************************
#endif /* FLASH_ST_H */




