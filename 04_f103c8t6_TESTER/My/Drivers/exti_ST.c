/*
 *
 *
 *  Created on:
 *      Author:
 */
#include "exti_ST.h"
//*******************************************************************************************
//*******************************************************************************************
/*Локальные переменные.*/


//*******************************************************************************************
//*******************************************************************************************
/*Локальные(статические) функции.*/
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Return         : 
*******************************************************************************/



//*******************************************************************************************
//*******************************************************************************************
/*Глобальные функции.*/
/*******************************************************************************
* Function Name  : void EXTI_Init(void)
* Description    :
* Input          :
* Return         :
***************************************/
void EXTI_Init(void){

	/* (1) Enable the peripheral clock of GPIOA */
	/* (2) Select Port A for pin 0 external interrupt by writing 0000 in EXTI0 (reset value)*/
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; /* (1) */
	//SYSCFG->EXTICR[1] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0_PA; /* (2) */

	//Настройка PA0 Input Pull Up
	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
	GPIOA->CRL |= (0x02 << GPIO_CRL_CNF0_Pos); //Вход Pull Up/Pull Down
	GPIOA->ODR |= (1 << 0);                    //Подтяжка вверх

	//EXTIx[3:0]: Конфигурация мультиплексора канала x EXTI
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0); //Нулевой канал EXTI подключен к порту PA0


	//EXTI->RTSR |= EXTI_RTSR_TR0;//Прерывание по нарастанию импульса
	EXTI->FTSR |= EXTI_FTSR_TR0;//Прерывание по спаду импульса
	EXTI->PR    = EXTI_PR_PR0;  //Сбрасываем флаг прерывания перед включением самого прерывания
	EXTI->IMR  |= EXTI_IMR_MR0; //Включаем прерывание 0-го канала EXTI

	/* Configure NVIC for External Interrupt */
	/* (6) Enable Interrupt on EXTI0_1 */
	/* (7) Set priority for EXTI0_1 */
	NVIC_EnableIRQ(EXTI0_IRQn); /* (6) */
	NVIC_SetPriority(EXTI0_IRQn,0); /* (7) */
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Return         : 
***************************************/


//*******************************************************************************************
//*******************************************************************************************



