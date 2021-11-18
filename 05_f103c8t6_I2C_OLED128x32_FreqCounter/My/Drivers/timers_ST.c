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
//************************************************************
/*
 *Режим интерфейса энкодера (Encoder Interface Mode)
 */
void TIM3_InitForEncoder(void){

	/*
	 * Настройка ножки микроконтроллера.
	 * PA6 - TIM3_CH1
	 * PA7 - TIM3_CH2
	 */
	/* Тактирование GPIOA и Alternative Func */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

	//PA6 - TIM3_CH1.
	GPIOA->CRL &= ~(0b11 << GPIO_CRL_MODE6_Pos);// 00: Input mode (reset state)
	GPIOA->CRL |=  (0b10 << GPIO_CRL_CNF6_Pos); // 10: Input with pull-up / pull-down
	GPIOA->ODR |=   GPIO_ODR_ODR6;				// pull-up

	//PA7 - TIM3_CH2.
	GPIOA->CRL &= ~(0b11 << GPIO_CRL_MODE7_Pos);// 00: Input mode (reset state)
	GPIOA->CRL |=  (0b10 << GPIO_CRL_CNF7_Pos); // 10: Input with pull-up / pull-down
	GPIOA->ODR |=   GPIO_ODR_ODR7;				// pull-up

	/* Encoder Initialization */
	/* TIM3 Clock */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	//TIM3->PSC = 4;
	TIM3->ARR = 100 * 2 - 1;//Счёт выполняется от 0 до значения в регистре TIMx_ARR
						    //На каждый щелчок энкодера таймер меняется на 2.

	//CC1S[1:0]: выбор направления для канала x.
	//TIM3->CCMR1 |= (0b01 << TIM_CCMR1_CC1S_Pos);//01:канал CC1 настроен как вход, сигнал IC1 подключен к TI1
	//TIM3->CCMR1 |= (0b01 << TIM_CCMR1_CC2S_Pos);//01:канал CC2 настроен как вход, сигнал IC2 подключен к TI2

	//ICxPSC[1:0]: предделитель для входного канала ICx.
	//TIM3->CCMR1 |= (0b11 << TIM_CCMR1_IC1PSC_Pos);//11: захват выполняется каждые 8 выбранных событий на входе
	//TIM3->CCMR1 |= (0b11 << TIM_CCMR1_IC2PSC_Pos);//11: захват выполняется каждые 8 выбранных событий на входе

	//Bits 9:8 CKD: Clock division
	//00: tDTS = tCK_INT
	//01: tDTS = 2 × tCK_INT
	//10: tDTS = 4 × tCK_INT
	//11: Reserved
	TIM3->CR1 |= (0b01 << TIM_CR1_CKD_Pos);

	//IC1F[3:0]: настройка частоты сэмплирования и времени дэмпфирования для цифрового фильтра входного сигнала канала x.
	TIM3->CCMR1 |= (0b1010 << TIM_CCMR1_IC1F_Pos);//1010: fSAMPLING=fDTS/16, N=5
	TIM3->CCMR1 |= (0b1010 << TIM_CCMR1_IC2F_Pos);//1010: fSAMPLING=fDTS/16, N=5

	//CCxP: определение активных полярностей входных и выходных сигналов канала x.
	TIM3->CCER &= ~(TIM_CCER_CC1P  | TIM_CCER_CC2P);//
	TIM3->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC2NP);//

	//SMS[2:0](slave mode selection): эти биты определяют режим работы слэйв-контроллера (что и как будет делать слэйв-контроллер).
	TIM3->SMCR |= (0b001 << TIM_SMCR_SMS_Pos);//001: Encoder Mode 1 — счётчик считает вверх/вниз по фронту на TI2FP2 в зависимости от уровня на TI1FP1

	//Counter enabled.
	TIM3->CR1 |= TIM_CR1_CEN;
}
//************************************************************
uint32_t TIM3_GetCounter(void){

	return TIM3->CNT;
}
//************************************************************
void TIM3_SetCounter(uint32_t cnt){

	TIM3->CNT = cnt << 1;//На каждый щелчок энкодера таймер меняется на 2.
}
//************************************************************
void TIM3_SetARR(uint32_t arr){

	TIM3->CNT = arr * 2 - 1;//Счёт выполняется от 0 до значения в регистре TIMx_ARR
							//На каждый щелчок энкодера таймер меняется на 2.
}
//*******************************************************************************************
//*******************************************************************************************
void TIM4_Init(void){

	//Настройка ножки микроконтроллера.
	//Используется порт PB8(TIM4_CH3)
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

	GPIOB->CRH |= GPIO_CRH_CNF8_1;//PB8(TIM4_CH3) - выход, альтернативный режим push-pull.
	GPIOB->CRH |= GPIO_CRH_MODE8; //PB8(TIM4_CH3) - тактирование 50МГц.

	//Включение тактирования таймера.
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

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

//	TIM4->DIER |= TIM_DIER_UIE; //Update interrupt enable
//	TIM4->CR1   = TIM_CR1_ARPE |//Auto-reload preload enable
//				  TIM_CR1_CEN;  //Counter enable
	//Разрешение прерывания от TIM4.
//	NVIC_SetPriority(TIM4_IRQn, 15);
//	NVIC_EnableIRQ(TIM4_IRQn);
}
//************************************************************
/*
 * вход: freq - частота в Гц
 */
void TIM4_SetFreq(uint32_t freq){

	TIM4->CR1 &= ~(TIM_CR1_CEN);                //Counter disable.
	TIM4->ARR  = (uint32_t)(1000000 / freq) - 1;//Auto reload register - это значение, до которого будет считать таймер.
	TIM4->CR1 |= TIM_CR1_CEN;        			//Counter enable.
}
//*******************************************************************************************
//*******************************************************************************************




















