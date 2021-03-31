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
void TIM1_InitForPWM(uint32_t ARRval){

	//Включение тактирования таймера.
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	//Прескаллер.
	//APB2_CLK = 72MHz, TIM1_CLK = APB2_CLK * 1 = 72MHz.
	TIM1->PSC = 0;                //PSC[15:0]: Prescaler value.
								  //The counter clock frequency (CK_CNT) is equal to fCK_PSC / (PSC[15:0] + 1).
								  //таймер будет тактироваться с частотой 72МГц/(PSC[15:0] + 1) = 72MГц.
	TIM1->ARR = ARRval;//(3600 - 1);       //Auto reload register. - это значение, до которого будет считать таймер.

	TIM1->CR1  |= TIM_CR1_ARPE;   //Включен режим предварительной записи регистра автоперезагрузки
	TIM1->CCER |= TIM_CCER_CC1E | //Enable CC1 - включение первого канала
				  TIM_CCER_CC2E | //Enable CC2 - включение второго канала
				  TIM_CCER_CC3E;  //Enable CC3 - включение третьего канала.
	//Задаем режим работы канала 1 - PWM mode on OC1
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | //OC1M : Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC1PE  |					 //OC1PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.
				   TIM_CCMR1_OC1FE;						 //OC1FE: Output Compare 1 fast enable.
	//Задаем режим работы канала 2 - PWM mode on OC2
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | //OC2M : Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR1_OC2PE  |					 //OC2PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.
				   TIM_CCMR1_OC2FE;						 //OC2FE: Output Compare 1 fast enable.
	//Задаем режим работы канала 3 - PWM mode on OC3
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | //OC3M : Output compare 1 mode - 110: PWM mode 1.
				   TIM_CCMR2_OC3PE  |					 //OC3PE: Output compare 1 preload enable. 1: Preload register on TIMx_CCR1 enabled.
				   TIM_CCMR2_OC3FE;						 //OC3FE: Output Compare 1 fast enable.

	//Режимы работы ШИМ. CMS[1:0]: Center-aligned mode selection
	TIM1->CR1 &= ~(TIM_CR1_CMS_0 | TIM_CR1_CMS_1);
	//TIM1->CR1 |=   TIM_CR1_CMS_0; 			   // выравнивания по центру - Режим 1.
	//TIM1->CR1 |=   TIM_CR1_CMS_1;				   // выравнивания по центру - Режим 2.
	//TIM1->CR1 |=   TIM_CR1_CMS_0 | TIM_CR1_CMS_1;// выравнивания по центру - Режим 3.

	//TIM1->BDTR |= TIM_BDTR_MOE;// MOE: Main output enable. Разрешаем вывод сигнала на выводы
	TIM1->BDTR |= TIM_BDTR_AOE;//AOE: Automatic output enable
	//TIM1->BDTR |= (7 << 5);         // Мертвое время. Константа расчитана из задержек конкретного железа.

	//Настройка ножки микроконтроллера.
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	//Включение тактирования PORTA.

	GPIOA->CRH |= GPIO_CRH_CNF8_1 | //PA8(TIM1_CH1)  - выход, альтернативный режим push-pull.
			      GPIO_CRH_CNF9_1 | //PA9(TIM1_CH2)  - выход, альтернативный режим push-pull.
				  GPIO_CRH_CNF10_1; //PA10(TIM1_CH3) - выход, альтернативный режим push-pull.

	GPIOA->CRH |= GPIO_CRH_MODE8 |  //PA8(TIM1_CH1)  - тактирование 50МГц.
				  GPIO_CRH_MODE9 |  //PA9(TIM1_CH2)  - тактирование 50МГц.
				  GPIO_CRH_MODE10;  //PA10(TIM1_CH3) - тактирование 50МГц.
	//Включение таймера
	TIM1->CR1 |= TIM_CR1_CEN;//CEN: Counter enable
}
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
	TIM4->PSC = (360 - 1);  	   //таймер будет тактироваться с частотой 72МГц/72 = 1МГц.
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






















