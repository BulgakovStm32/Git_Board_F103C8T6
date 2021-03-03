/*
 * main.c
 *
 *  Created on: Dec 18, 2020
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"



//*******************************************************************************************
//*******************************************************************************************
//Переменные
static uint32_t PWRval = 0;


//*******************************************************************************************
//*******************************************************************************************
void IncrementOnEachPass(uint32_t *var, uint16_t event){

		   uint16_t riseReg  = 0;
	static uint16_t oldState = 0;
	//-------------------
	riseReg  = (oldState ^ event) & event;
	oldState = event;
	if(riseReg) (*var)++;
}
//************************************************************
void DecrementOnEachPass(uint32_t *var, uint16_t event){

		   uint16_t riseReg  = 0;
	static uint16_t oldState = 0;
	//-------------------
	riseReg  = (oldState ^ event) & event;
	oldState = event;
	if(riseReg) (*var)--;
}
//************************************************************
void Led_Blink(void){

	if(Blink(INTERVAL_50_mS)) Led_PC13_On();
	else					  Led_PC13_Off();
}
//************************************************************
uint8_t tic(uint16_t event){

	  	   uint16_t riseReg  = 0;
	static uint16_t oldState = 0;
	//-------------------
	riseReg  = (oldState ^ event) & event;
	oldState = event;
	if(riseReg) return 1;
	else 		return 0;
}
//*******************************************************************************************
//*******************************************************************************************
#define MOTOR_DELAY 6


#define EN1	1
#define EN2	2
#define EN3	3

#define EN_ON	950
#define EN_OFF  10

//************************************************************
//************************************************************
void BLDC_Commutation(void){

	//Фаза 1.
	MOTOR_EN1_On();
	MOTOR_IN1_On();

	MOTOR_EN2_On();
	MOTOR_IN2_Off();

	MOTOR_EN3_Off();

	msDelay(MOTOR_DELAY);

	//Фаза 2.
	MOTOR_EN2_Off();

	MOTOR_EN3_On();
	MOTOR_IN3_Off();

	msDelay(MOTOR_DELAY);

	//Фаза 3.
	MOTOR_EN1_Off();

	MOTOR_EN2_On();
	MOTOR_IN2_On();

	msDelay(MOTOR_DELAY);

	//Фаза 4.
	MOTOR_EN1_On();
	MOTOR_IN1_Off();

	MOTOR_EN3_Off();

	msDelay(MOTOR_DELAY);

	//Фаза 5.
	MOTOR_EN2_Off();

	MOTOR_EN3_On();
	MOTOR_IN3_On();

	msDelay(MOTOR_DELAY);

	//Фаза 6.
	MOTOR_EN1_Off();

	MOTOR_EN2_On();
	MOTOR_IN2_Off();

	msDelay(MOTOR_DELAY);
}
//************************************************************




//************************************************************
void PWM_SetVal(uint8_t output, uint32_t pwmVal){

	if(output == EN1) TIM3->CCR1 = pwmVal;
	if(output == EN2) TIM3->CCR2 = pwmVal;
	if(output == EN3) TIM3->CCR3 = pwmVal;
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	__enable_irq();
	//***********************************************

	TIM3_InitForPWM();
//	TIM3->CCR1 = 100;
//	TIM3->CCR2 = 100;
//	TIM3->CCR3 = 100;

	TIM4_Init();
	//***********************************************
	//Инициализация таймера TIM1 для работы в режиме ШИМ.



	//***********************************************
	//__disable_irq();
	msDelay(500);
	//************************************************************************************
	while(1)
		{
			//msDelay(10);
			//***********************************************
			//Мигание светодиодами.
			Led_Blink();
			//***********************************************
			//ШИМ.
//			if(PWRval <= 100) IncrementOnEachPass(&PWRval, Blink(INTERVAL_10_mS));
//			else PWRval = 10;
//			TIM3->CCR1 = PWRval;

			//***********************************************
			//Управление двигателем.

			//MOTOR_IN1_On();
//			MOTOR_EN1_On();
//
//			if(Blink(INTERVAL_5_mS)) MOTOR_IN1_On();
//			else					 MOTOR_IN1_Off();


			//Фаза 1.
			MOTOR_IN1_On();
			PWM_SetVal(EN1, EN_ON);

			MOTOR_IN2_Off();
			PWM_SetVal(EN2, EN_ON);

			PWM_SetVal(EN3, EN_OFF);

			msDelay(MOTOR_DELAY);

			//Фаза 2.
			PWM_SetVal(EN2, EN_OFF);

			MOTOR_IN3_Off();
			PWM_SetVal(EN3, EN_ON);

			msDelay(MOTOR_DELAY);

			//Фаза 3.
			PWM_SetVal(EN1, EN_OFF);

			MOTOR_IN2_On();
			PWM_SetVal(EN2, EN_ON);

			msDelay(MOTOR_DELAY);

			//Фаза 4.
			MOTOR_IN1_Off();
			PWM_SetVal(EN1, EN_ON);

			PWM_SetVal(EN3, EN_OFF);

			msDelay(MOTOR_DELAY);

			//Фаза 5.
			PWM_SetVal(EN2, EN_OFF);

			MOTOR_IN3_On();
			PWM_SetVal(EN3, EN_ON);

			msDelay(MOTOR_DELAY);

			//Фаза 6.
			PWM_SetVal(EN1, EN_OFF);

			MOTOR_IN2_Off();
			PWM_SetVal(EN2, EN_ON);

			msDelay(MOTOR_DELAY);

			//msDelay(1000);
			//***********************************************
			/* Sleep */
			//__WFI();
		}
	//************************************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_Handler(void){

//	static uint16_t msCountForDS18B20 = 0;
//	//-----------------------------
//	//Отсчет таймаута для датчика температуры.
//	if(++msCountForDS18B20 >= 1000)
//		{
//			msCountForDS18B20 = 0;
//			FlagsStr.DS18B20  = 1;
//		}
	//-----------------------------
	msDelay_Loop();
	Blink_Loop();
	Encoder()->Loop();
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void TIM4_IRQHandler(void){

	TIM4->SR &= ~TIM_SR_UIF;

	Led_PC13_Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
