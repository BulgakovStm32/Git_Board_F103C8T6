/*
 * 	main.c
 *
 *  Created on: 22.07.2022
 *  Autho     : Беляев А.А.
 *
 *
 *	Описание  : MCUv7
 *
 *	Последнее редактирование: 22.09.2022
 *
 */
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
Button_t PwrButton;

void DBG_SendDebugInfo(void);

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
static void STM32_Init(void){

 	STM32_Clock_Init();
 	STM32_GPIO_Init();
 	DELAY_Init();

 	STM32_GPIO_InitForOutputPushPull(LedPC13_GPIO, LedPC13_PIN);
}
//**********************************************************
static void SettingInterruptPriorities(void){

	//Приоритеты прерываний.
	//Таймер управления мотором - самый высокий приоритет = 1.
//	NVIC_SetPriority(TIM1_UP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
//	//Прерывания I2C для работы протакола - приоритет = 2
//	NVIC_SetPriority(I2C2_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
//	NVIC_SetPriority(I2C2_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
	//Системный таймер, прерывание каждую 1мс - самый низкий приоритет = 3
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0));
	//Разрешаем прерывания.
//	NVIC_EnableIRQ(TIM1_UP_IRQn);
//	NVIC_EnableIRQ(I2C2_EV_IRQn);
//	NVIC_EnableIRQ(I2C2_ER_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************




//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//--------------------------
	STM32_Init();	 //Инициализация STM32.
 	//GPIO_MCU_Init(); //Инициализация портов MCU.
	//--------------------------
	//Инициализация кнопки питания и АЦП для измерения напряжения АКБ.
	//POWER_Init();

	//Включение питания платы. Смотрим что там с напряжением АКБ.
	//POWER_TurnOnAndSupplyVoltageCheck();

	//Включение питания периферии.
	//PWR_BTN_LED_High();
	//FAN_EN_High();
	//LIDAR_EN_High();
	//LAMP_PWM_High();
	DELAY_mS(100); //Задержка для стабилизации напряжения патания.
	//--------------------------
	//Инициализация дисплея
	STM32_GPIO_InitForOutputPushPull(GPIOB, RST);
	STM32_GPIO_InitForOutputPushPull(GPIOB, BL);
	STM32_GPIO_InitForOutputPushPull(GPIOB, DC);

	STM32_GPIO_InitForOutputPushPull(GPIOA, CS);
	STM32_GPIO_InitForOutputPushPull(GPIOA, CLK);
	STM32_GPIO_InitForOutputPushPull(GPIOA, DIN);

	st7735_init(ST77XX_BLACK);

	//--------------------------
	//Ини-я диспетчера.
	RTOS_Init();
	//RTOS_SetTask(Task_POWER_Check,   1000, 100);//Опрос кнопки питания и проверка напряжения питания каждые 100мс.
	//RTOS_SetTask(Task_TEMPERATURE_Read, 0, 500);//Измерение температуры каждую 1сек.
	//RTOS_SetTask(Task_CheckRotate,      0, 50); //Проверка вращения. Не отлажена!!!

	//--------------------------
	SysTick_Init();
	SettingInterruptPriorities();//Приоритеты прерываний
	__enable_irq();				 //Глобальное разрешение прерываний

	//**************************************************************
	while(1)
	{
		RTOS_DispatchLoop();
		//__WFI(); //Sleep

//		if(Blink(INTERVAL_50_mS)) LedPC13On();
//		else					  LedPC13Off();

//		if(Blink_WithVariablePeriod(RTOS_GetTickCount(), 1000, 5)) 	LedPC13On();
//		else 														LedPC13Off();


		st7735_fill(0, 50, 0, 50, ST77XX_CYAN);
	}
	//**************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_IT_Handler(void){

	Blink_Loop();
	STM32_GPIO_CheckLoop();
	RTOS_TimerServiceLoop();//Служба таймеров.
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************










