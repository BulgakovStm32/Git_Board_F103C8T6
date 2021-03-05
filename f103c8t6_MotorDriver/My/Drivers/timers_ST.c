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
	TIM3->PSC = (3 - 1);   //таймер будет тактироваться с частотой 72МГц/1000=72кГц.
	TIM3->ARR = (1000 - 1);//Auto reload register. - это значение, до которого будет считать таймер.

	//Задаем режим работы - PWM mode on OC1
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | //OC1M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC1PE;						 //OC1PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.

	//Задаем режим работы - PWM mode on OC2
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | //OC2M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC2PE;						 //OC2PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.

	//Задаем режим работы - PWM mode on OC3
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | //OC3M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR2_OC3PE;						 //OC3PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.

	TIM3->CCER |= TIM_CCER_CC1E;//Enable CC1 - включение первого канала
	TIM3->CCER |= TIM_CCER_CC2E;//Enable CC2 - включение второго канала
	TIM3->CCER |= TIM_CCER_CC3E;//Enable CC3 - включение третьего канала.

	//Настройка ножки микроконтроллера.
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRL |= GPIO_CRL_CNF6_1;//PA6(TIM3_CH1) - выход, альтернативный режим push-pull.
	GPIOA->CRL |= GPIO_CRL_MODE6; //PA6(TIM3_CH1) - тактирование 50МГц.

	GPIOA->CRL |= GPIO_CRL_CNF7_1;//PA7(TIM3_CH2) - выход, альтернативный режим push-pull.
	GPIOA->CRL |= GPIO_CRL_MODE7; //PA7(TIM3_CH2) - тактирование 50МГц.

	GPIOB->CRL |= GPIO_CRL_CNF0_1;//PB0(TIM3_CH3) - выход, альтернативный режим push-pull.
	GPIOB->CRL |= GPIO_CRL_MODE0; //PB0(TIM3_CH3) - тактирование 50МГц.

	//Включение DMA для работы с таймером.
//	TIM3->DIER |= TIM_DIER_CC1DE;
	//Включение таймера
	TIM3->CR1 |= TIM_CR1_CEN;
}
//*******************************************************************************************
//*******************************************************************************************
void TIM4_Init(void){

	//Включение тактирования таймера.
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	//Прескаллер.
	//APB1_CLK = 36MHz, TIM4_CLK = APB1_CLK * 2 = 72MHz.
	TIM4->PSC = (72 - 1);  //таймер будет тактироваться с частотой 72МГц/72 = 1МГц.
	TIM4->ARR = (10000 - 1);//Auto reload register - это значение, до которого будет считать таймер.

	TIM4->DIER |= TIM_DIER_UIE; //Update interrupt enable
	TIM4->CR1   = TIM_CR1_ARPE |//Auto-reload preload enable
				  TIM_CR1_CEN;  //Counter enable

	//Разрешение прерывания от TIM4.
	NVIC_SetPriority(TIM4_IRQn, 15);
	NVIC_EnableIRQ(TIM4_IRQn);
}
//*******************************************************************************************
//*******************************************************************************************
void TIM1_InitForPWM(void){

	//Включение тактирования таймера.
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	//Прескаллер.
	//APB2_CLK = 72MHz, TIM1_CLK = APB2_CLK * 1 = 72MHz.
	TIM1->PSC = 0;   //таймер будет тактироваться с частотой 72МГц/1 = 72MГц.
	TIM1->ARR = (1000 - 1);//Auto reload register. - это значение, до которого будет считать таймер.

	//Задаем режим работы - PWM mode on OC1
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | //OC1M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC1PE;						 //OC1PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.

	//Задаем режим работы - PWM mode on OC2
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | //OC2M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC2PE;						 //OC2PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.

	//Задаем режим работы - PWM mode on OC3
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | //OC3M: Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR2_OC3PE;						 //OC3PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.


	//TIM1->CR1 |= TIM_CR1_ARPE;//Включен режим предварительной записи регистра автоперезагрузки
	//TIM1->CR1  |= TIM_CR1_CMS_0 | TIM_CR1_CMS_1;// Режим 3 - выравнивания по центру.
	//TIM1->BDTR |= 75;      				// Мертвое время. Константа расчитана из задержек конкретного железа.

	TIM1->CCER |= TIM_CCER_CC1E;//Enable CC1 - включение первого канала
	TIM1->CCER |= TIM_CCER_CC2E;//Enable CC2 - включение второго канала
	TIM1->CCER |= TIM_CCER_CC3E;//Enable CC3 - включение третьего канала.

	TIM1->BDTR |= TIM_BDTR_MOE;// Разрешаем вывод сигнала на выводы

	//Настройка ножки микроконтроллера.
	//Используется PORTA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRH |= GPIO_CRH_CNF8_1;//PA8(TIM1_CH1) - выход, альтернативный режим push-pull.
	GPIOA->CRH |= GPIO_CRH_MODE8; //PA8(TIM1_CH1) - тактирование 50МГц.

	GPIOA->CRH |= GPIO_CRH_CNF9_1;//PA9(TIM1_CH2) - выход, альтернативный режим push-pull.
	GPIOA->CRH |= GPIO_CRH_MODE9; //PA9(TIM1_CH2) - тактирование 50МГц.

	GPIOA->CRH |= GPIO_CRH_CNF10_1;//PA10(TIM1_CH3) - выход, альтернативный режим push-pull.
	GPIOA->CRH |= GPIO_CRH_MODE10; //PA10(TIM1_CH3) - тактирование 50МГц.

	//Включение таймера
	TIM1->CR1 |= TIM_CR1_CEN;//CEN: Counter enable
}
//*******************************************************************************************
//*******************************************************************************************






















