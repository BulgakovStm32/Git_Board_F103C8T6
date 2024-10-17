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
 	STM32_GPIO_SWD_Init();
 	DELAY_Init();

 	STM32_GPIO_InitForOutputPushPull(LedPC13_GPIO, LedPC13_PIN);
 	LedPC13Off();
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
//стрелочный индикатор
#define ANALOG_SCALE_ANGLE_MIN	50
#define ANALOG_SCALE_ANGLE_MAX	132

/*
 * Параметры:
 * angle :угол на который нужно повернуть стрелку (слева направо)
 *		  мин. уол  - 50  градусов.
 *		  макс.угол - 132 градуса.
 */
void Lcd_AnalogScale(uint16_t angle){

	const uint8_t markRadius = 40;					// Радиус шкалы.
	const float	  degreesToRad = 2 * M_PI / 360.0;	// коэф-т для перевода градусов в радианы
	int16_t x0 = 64;		  	  // Х-координата центра шкалы
	int16_t y0 = 96; 	  		  // Y-координата центра шкалы.
	float rad;
	float cos;
	float sin;
	//-------------------
	//Рисуем риски-метки шкалы
	for(uint32_t i = 0; i < 360; i += (360 / 12))//12 рисок, как у часов
	{
		rad = i * degreesToRad;//перевод углов в радианы
		cos = cosf(rad);
		sin = sinf(rad);

		st7735_DrawLine(x0 +  markRadius    * cos,	//x1
						y0 +  markRadius    * sin,	//y1
						x0 + (markRadius-6) * cos, //x2
						y0 + (markRadius-6) * sin, //y2
						ST77XX_COLOR_YELLOW);
	}

	//Стрелка.
	angle += 90;			   	//это нужно чтобы секундная стрелка стартовала с 12 часов.
	rad = angle * degreesToRad;	//перевод углов в радианы
	cos = cosf(rad);
	sin = sinf(rad);
	//x0 += 1; //небольшое смещение по Х что бы стрелка точно поподала в среднюю риску.
	//y0 += 5; //небольшое смещение по Y что бы стрелка поподала в риски.
//	st7735_Line(x0 + markRadius * cos,
//			 	y0 + markRadius * sin,
//				x0 + 1 * cos,
//				y0 + 1 * sin,
//				ST77XX_GREEN);

	//Секундная стрелка
	st7735_DrawLine(x0,
					y0,
					x0 - (markRadius-4) * cos,
					y0 - (markRadius-4) * sin,
					ST77XX_COLOR_GREEN);

	//Минутная стрелка
	st7735_DrawLine(x0,
					y0,
					x0 - (markRadius/2) * 1,
					y0 - (markRadius/2) * 1,
					ST77XX_COLOR_BLUE);

}
//**********************************************************
void LCD_Test(void){

	//DELAY_mS(500);
	//-------------------------------
	//Счетчик для измерения длительности выполенеия
	static uint32_t duration = 0;
	duration = RTOS_GetTickCount();

	//-------------------------------
	//Очищаем дисплей.
	//st7735_clear(ST77XX_BLACK);

	//Рисование прямоугольной залитой области
	//st7735_rectangle(0, 10, 126, 8, ST77XX_WHITE);

	//Рисование рамки для вывода текства в ней
	st7735_DrawLine(0  , 10, 127, 10, ST77XX_COLOR_WHITE);//Горизонтальная линия
	st7735_DrawLine(127, 10, 127, 22, ST77XX_COLOR_WHITE);//Вертикальная линия
	st7735_DrawLine(0  , 22, 127, 22, ST77XX_COLOR_WHITE);//Горизонтальная линия
	st7735_DrawLine(0  , 22, 0,   10, ST77XX_COLOR_WHITE);//Вертикальная линия

	//Тест : Вывод текстовых строк и значений счетчиков - Работает!!!
	static uint32_t count = 0;

	//Строка 1.
	char str1[] = "Count1=";
	uint16_t pos1_x = (sizeof(str1)-1) * Font_7x10.width + 1;

	st7735_PrintString(1, 12, str1, Font_7x10, ST77XX_COLOR_ORANGE, ST77XX_COLOR_BLACK);
	st7735_BinToDec(pos1_x, 12, count, 4, Font_7x10, ST77XX_COLOR_ORANGE, ST77XX_COLOR_BLACK);

	count++;

//		//Строка 2.
//		char str2[] = "Count2=";
//		uint16_t pos2_x = (sizeof(str2)-1) * Font_9x16.width;
//		uint16_t pos2_y = Font_7x10.height;
//
//		st7735_PrintString(0, pos2_y, str2, Font_9x16, ST77XX_GREEN, ST77XX_BLACK);
//		st7735_BinToDec(pos2_x, pos2_y, count+1, 4, Font_9x16, ST77XX_GREEN, ST77XX_BLACK);


//		st7735_WriteString(0, 26, "Vcc=123456", Font_11x18, ST77XX_BLUE, ST77XX_BLACK);
//
//		st7735_WriteString(0, 44, "Vcc=123456", Font_7x10, ST77XX_GREEN, ST77XX_BLACK);
//
//		st7735_WriteString(0, 55, "Vcc=123456", Font_16x26, ST77XX_GREEN, ST77XX_BLACK);
//
//		st7735_point(64, 130, ST77XX_WHITE);

//		//-------------------------------
//		//Тест - рисуем окружности.
//		static uint8_t radius = 5;
//
//		radius += 2;
//		if(radius >= 28) radius = 5;
//
//		st7735_Circle(64, 130, radius, ST77XX_WHITE);
//		//-------------------------------
//		//Тест : рисуем две наклонных линии
//
//		st7735_Line(1  , 160, 128, 1, ST77XX_WHITE);
//		st7735_Line(128, 160,   1, 1, ST77XX_WHITE);
//
		//-------------------------------
		//Тест : рисуем стрелочные часы
		st7735_Fill(0, 60, ST7735_WIDTH, ST7735_HEIGHT, ST77XX_COLOR_BLACK);//Очищаем область

		static uint16_t angle = 0;

		Lcd_AnalogScale(angle);

		angle += 6;
		if(angle >= 360) angle = 0;

//		//-------------------------------
//		//Тест :
//		static uint16_t color = 0;
//		color += 50;
//
//		st7735_Fill(0, 130., 64, 150, color);
//
//		//-------------------------------
	//Тест : Вывод картинки - Работает.
	//st7735_DrawImage(0, 0, 128, 160, testImg);

	//-------------------------------
	//Вывод времени исполнения кода.
	duration = RTOS_GetTickCount() - duration;

	char dur[] = "Duration=";
	st7735_PrintString(0, 27, dur, Font_9x16, ST77XX_COLOR_GREEN, ST77XX_COLOR_BLACK);
	//st7735_BinToDec((sizeof(dur)-1) * Font_9x16.width, 27, duration, 3, Font_9x16, ST77XX_GREEN, ST77XX_BLACK);
	st7735_BinToDec2((sizeof(dur)-1) * Font_9x16.width, 27, duration, Font_9x16, ST77XX_COLOR_GREEN, ST77XX_COLOR_BLACK);
}
//**********************************************************
void LowPwrMode(void){

	//Бит PDDS определяет выбор между Stop и Standby.
	//PDDS: Power down deepsleep.
	//0: Enter Stop mode when the CPU enters Deepsleep.
	//1: Enter Standby mode when the CPU enters Deepsleep.
	PWR->CR &= ~PWR_CR_PDDS;

	//флаг Wakeup должн быть очищен, иначе есть шанс проснуться немедленно
	//CWUF: Clear wakeup flag. This bit is always read as 0.
	//0: No effect
	//1: Clear the WUF Wakeup Flag after 2 System clock cycles. (write)
	PWR->CR |= PWR_CR_CWUF;

	//Для STM32L1xx
	//стабилизатор питания в low-power режим.
	//у нас в Stop потребления-то почти не будет */
	//PWR->CR |= PWR_CR_LPSDSR;

	//Для STM32L1xx
	//Источник опорного напряжения Vref выключить автоматически.
	//PWR->CR |= PWR_CR_ULP;

	//С точки зрения ядра Cortex-M, что Stop, что Standby - это режим Deep Sleep.
	//Поэтому надо в ядре включить Deep Sleep.
	//Bit 2 SLEEPDEEP
	//Controls whether the processor uses sleep or deep sleep as its low power mode:
	//0: Sleep
	//1: Deep sleep
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	//SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;//0: Do not sleep when returning to Thread mode.
	SCB->SCR |=  SCB_SCR_SLEEPONEXIT_Msk;//1: Enter sleep, or deep sleep, on return from an interrupt service routine

	//Выключили прерывания; пробуждению по ним это не помешает.
	//unsigned state = irq_disable();
	__disable_irq();

	//Останов таймера SysTick
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

	//Отключаем тактирование порта A, B, C, D и модуля альтернативных функций.
	RCC->APB2ENR &= ~(//RCC_APB2ENR_IOPAEN |
					  RCC_APB2ENR_IOPBEN |
					  //RCC_APB2ENR_IOPCEN |
					  RCC_APB2ENR_IOPDEN
					  //RCC_APB2ENR_AFIOEN
					  );


	RCC->CFGR &= ~RCC_CFGR_SW;		//SW[1:0] bits (System clock Switch)
	RCC->CFGR |=  RCC_CFGR_SW_HSI;	//HSI selected as system clock

	RCC->CR &= ~RCC_CR_PLLON;		//Disable PLL
	RCC->CR &= ~RCC_CR_HSEON;		//Disable HSE.

	//завершили незавершённые операция сохранения данных.
	__DSB();	//Data Synchronization Barrier

	//заснули...
	__WFI();	//Wait For Interrupt
	//__WFE();	//Wait For Event

	__ISB(); 	//Added by me. Instruction Synchronization Barrier

	//SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

	/* переинициализация рабочих частот */
	//init_clk();
//	STM32_Init();	//Инициализация STM32.
//
//	SysTick_Init();
//	SettingInterruptPriorities();//Приоритеты прерываний
//
	/* после просыпания восстановили прерывания */
	//irq_restore(state);
	__enable_irq();
}
//**********************************************************
void EXTI_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Тактирование GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Тактирование AFIO

	//Настройка GPIO:
	//Пин:	 PA0
	//Режим: Input Pull Up
	GPIOA->CRL &= ~(GPIO_CRL_MODE0_Msk | GPIO_CRL_CNF0_Msk);//
	GPIOA->CRL |= (0x02 << GPIO_CRL_CNF0_Pos); 				//10: Input with pull-up / pull-down
	GPIOA->ODR |= (1 << 0); 								//Подтяжка вверх

	//Настройка контроллера EXTI. AFIO->EXTICR[x] Reset value: 0x0000

	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;	//Нулевой канал EXTI подключен к порту PA0

	//EXTI->RTSR |= EXTI_RTSR_TR0; //Прерывание по нарастанию импульса
	EXTI->FTSR |= EXTI_FTSR_TR0; //Прерывание по спаду импульса

	EXTI->PR   = EXTI_PR_PR0;		//Сбрасываем флаг прерывания перед включением самого прерывания
	EXTI->IMR |= EXTI_IMR_MR0;   	//Включаем прерывание 0-го канала EXTI

	NVIC_EnableIRQ(EXTI0_IRQn);  	//Разрешаем прерывание в контроллере прерываний


	//EXTI line 17 is connected to the RTC Alarm even
	EXTI->RTSR |= EXTI_RTSR_TR17; 	//Прерывание по нарастанию импульса
	//EXTI->FTSR |= EXTI_FTSR_TR17;

	EXTI->PR   = EXTI_PR_PR17;		//Сбрасываем флаг прерывания перед включением самого прерывания
	EXTI->IMR |= EXTI_IMR_MR17;   	//Включаем прерывание 17-го канала EXTI

	NVIC_EnableIRQ(RTC_Alarm_IRQn);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//--------------------------
	STM32_Init();	//Инициализация STM32.
	RTC_Init();		//инициализация RTC

	DELAY_mS(100); 	//Задержка для стабилизации напряжения патания.
	//--------------------------
	//Инициализация дисплея
	//st7735_Init(ST77XX_COLOR_BLACK);

	//Инициализация внешних прерываний.
	EXTI_Init();	//Прерывание по низкому уровню на PA0

	//--------------------------
	//Ини-я диспетчера.
	//RTOS_Init();
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
		//RTOS_DispatchLoop();

//		if(Blink(INTERVAL_50_mS)) LedPC13On();
//		else					  LedPC13Off();

//		if(Blink_WithVariablePeriod(RTOS_GetTickCount(), 1000, 5)) 	LedPC13On();
//		else 														LedPC13Off();

		//Экспериманты с дисплеем.
		//LCD_Test();

		//Прерывание по низкому уровню на PA0
		//EXTI_Init();

		//Режимы пониженного энгергопотребления Stop.

		//SCB->SCR|=SCB_SCR_SEVONPEND_Msk;

		LowPwrMode();

		//LedPC13Toggel();
		//DELAY_mS(100);
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











