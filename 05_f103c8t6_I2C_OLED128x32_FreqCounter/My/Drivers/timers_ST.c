/*
 * timers_ST.c
 *
 *  Created on: 23 янв. 2021 г.
 *      Author: Zver
 */
//*******************************************************************************************
//*******************************************************************************************

#include "timers_ST.h"

//*******************************************************************************************
//*******************************************************************************************
void TIM3_InitForPWM(void){

	//Включение тактирования таймера.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	//Выбор источника тактирования.

	//Прескаллер.
	//APB1_CLK = 36MHz, TIM3_CLK = APB1_CLK * 2 = 72MHz.
	TIM3->PSC = 2;//(1000 - 1);//таймер будет тактироваться с частотой 72МГц/1000=72кГц.
	//Auto reload register. - это значение, до которого будет считать таймер.
	TIM3->ARR = (72 - 1);
	//Задаем режим работы - PWM mode on OC1
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | //OC1M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC1PE;						 //OC1PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.
	//Enable CC1 - включение первого канала
	TIM3->CCER |= TIM_CCER_CC1E;

	//Настройка ножки микроконтроллера.
	//Используется порт PA6(TIM3_CH1)
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRL |= GPIO_CRL_CNF6_1;//PA6(TIM3_CH1) - выход, альтернативный режим push-pull.
	GPIOA->CRL |= GPIO_CRL_MODE6; //PA6(TIM3_CH1) - тактирование 50МГц.

	//Включение DMA для работы с таймером.
	TIM3->DIER |= TIM_DIER_CC1DE;
	//Включение таймера
	TIM3->CR1 |= TIM_CR1_CEN;
}
//*******************************************************************************************
//*******************************************************************************************
void TIM1_InitForCapture(void){

	//Включение тактирования таймера.
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	//Прескаллер.
	//APB2_CLK = 72MHz, TIM1_CLK = APB2_CLK * 1 = 72MHz.
	TIM1->PSC = 1000;//(1000 - 1);//таймер будет тактироваться с частотой 72МГц/1000=72кГц.
	//Auto reload register. - это значение, до которого будет считать таймер.
	TIM1->ARR = (72 - 1);
	//Задаем режим работы - PWM mode 1 on OC1
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | //OC1M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC2PE;						 //OC1PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.
	//Enable CC1 - включение первого канала
	TIM1->CCER |= TIM_CCER_CC2E;
	//Main output enable.
	TIM1->BDTR |= TIM_BDTR_AOE;

	//Настройка ножки микроконтроллера.
	//Используется порт PA9(TIM1_CH2)
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRH |= GPIO_CRH_CNF9_1;//PA9(TIM1_CH2) - выход, альтернативный режим push-pull.
	GPIOA->CRH |= GPIO_CRH_MODE9; //PA9(TIM1_CH2) - тактирование 50МГц.

	//Включение таймера
	TIM1->CR1 |= TIM_CR1_CEN;//CEN: Counter enable
}
//*******************************************************************************************
//*******************************************************************************************
void TIM4_Init(void){

	//Включение тактирования таймера.
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	//Настройка ножки микроконтроллера.
	//Используется порт PB8(TIM4_CH3)
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

	GPIOB->CRH |= GPIO_CRH_CNF8_1;//PB8(TIM4_CH3) - выход, альтернативный режим push-pull.
	GPIOB->CRH |= GPIO_CRH_MODE8; //PB8(TIM4_CH3) - тактирование 50МГц.

	//Прескаллер.
	//APB1_CLK = 36MHz, TIM4_CLK = APB1_CLK * 2 = 72MHz.
	TIM4->PSC = (72 - 1);//таймер будет тактироваться с частотой 72МГц/72 = 1МГц.
	TIM4->ARR = (90 - 1);//Auto reload register - это значение, до которого будет считать таймер.

	//Режим управления выходом
	//OCxM=’011′ для переключения сигнала на выходе при совпадении счётчика и значения в регистре CCRx
	TIM4->CCMR2 |= (0b011 << TIM_CCMR2_OC3M_Pos);

	//Включения выхода.
	//записываем CCxE=’1′ для включения выхода (чтобы на выходном пине появился сигнал OCx) (в регистре TIMx_CCER)
	TIM4->CCER  |= TIM_CCER_CC3E;

	//TIM4->DIER |= TIM_DIER_UIE;//Update interrupt enable
//	TIM4->CR1 = TIM_CR1_ARPE | //Auto-reload preload enable
//				TIM_CR1_CEN;   //Counter enable

	//Разрешение прерывания от TIM4.
	//NVIC_SetPriority(TIM4_IRQn, 15);
	//NVIC_EnableIRQ(TIM4_IRQn);
}
//************************************************************
/*
 * вход: freq - частота в Гц
 */
void TIM4_SetFreq(uint32_t freq){

	TIM4->CR1 &= ~(TIM_CR1_CEN);//Counter disable.
	TIM4->ARR  = (uint32_t)(1000000 / freq) - 1;//Auto reload register - это значение, до которого будет считать таймер.
	TIM4->CR1 |= TIM_CR1_CEN;   //Counter enable.
}
//*******************************************************************************************
//*******************************************************************************************




















