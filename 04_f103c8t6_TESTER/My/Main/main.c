/*
 * main.c
 *
 *  Created on:
 *      Author:
 */
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
DS18B20_t	Sensor_1;
DS18B20_t	Sensor_2;
DS1307_t 	TimeAndCalendar;
Encoder_t	Encoder;

//*******************************************************************************************
//*******************************************************************************************
//void IncrementOnEachPass(uint32_t *var, uint16_t event){
//
//		   uint16_t riseReg  = 0;
//	static uint16_t oldState = 0;
//	//-------------------
//	riseReg  = (oldState ^ event) & event;
//	oldState = event;
//	if(riseReg) (*var)++;
//}
//************************************************************
//void Temperature_Read(void){
//
//		   uint32_t millis    = Scheduler_GetTickCount();
//	static uint32_t millisOld = 0;
//	//-------------------
//	if(millis - millisOld >= 1000)
//	{
//		millisOld = millis;
//
//		TemperatureSens_ReadTemperature(&Sensor_1);
//		TemperatureSens_ReadTemperature(&Sensor_2);
//	}
//}
//************************************************************
void Led_Blink1(uint32_t millis){

	static uint32_t millisOld = 0;
	static uint32_t flag      = 0;
	//-------------------
	if((millis - millisOld) >= (flag ? 900 : 100 ))
	{
		millisOld = millis;
		flag = !flag;
		//LedPC13Toggel();
	}
}
//************************************************************
//void Temperature_Display(DS18B20_t *sensor, uint8_t cursor_x, uint8_t cursor_y){
//
//	uint32_t temperature = sensor->TEMPERATURE;
//	//-------------------
//	Lcd_SetCursor(cursor_x, cursor_y);
//	Lcd_Print("Sens");
//	Lcd_BinToDec(sensor->SENSOR_NUMBER, 1, LCD_CHAR_SIZE_NORM);
//	Lcd_Chr('=');
//
//	if(TemperatureSens_Sign(sensor) & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
//	else                    								Lcd_Chr('+');
//
//	Lcd_BinToDec(temperature/10, 2, LCD_CHAR_SIZE_NORM);
//	Lcd_Chr('.');
//	Lcd_BinToDec(temperature%10, 1, LCD_CHAR_SIZE_NORM);
//	Lcd_Print(" C(PinA");
//	Lcd_BinToDec(sensor->GPIO_PIN, 1, LCD_CHAR_SIZE_NORM);
//	Lcd_Chr(')');
//}
//*******************************************************************************************
//*******************************************************************************************
void Task_I2C(void){

	//static uint8_t sendBuf[32] = {0,};
	//-----------------------------
	LedPC13Toggel();

	I2C_StartAndSendDeviceAddr(I2C1, 0x06);
	I2C_Stop(I2C1);

	//-----------------------------
	Scheduler_SetTimerTask(Task_I2C, 1000);
}
//************************************************************
void Task_BlinkLed(void){

	Led_Blink1(Scheduler_GetTickCount());
	//-----------------------------
	Scheduler_SetTask( Task_BlinkLed);
}
//************************************************************




//void Task_LcdUpdate(void){
//
//	static uint32_t fps      = 0;
//	static uint32_t fps_temp = 0;
//	//-----------------------------
//	//Счетчик кадров в секунду
//	if(!Blink(INTERVAL_1000_mS)) fps_temp++;
//	if(Blink(INTERVAL_1000_mS) && fps_temp != 0)
//		{
//			fps = fps_temp;
//			fps_temp = 0;
//		}
//	Lcd_SetCursor(16, 1);
//	Lcd_Print("FPS=");
//	Lcd_BinToDec(fps, 2, LCD_CHAR_SIZE_NORM);
//	//-----------------------------
//	Lcd_Update();//вывод сделан для SSD1306
//	Lcd_ClearVideoBuffer();
//	//-----------------------------
//	Scheduler_SetTask(Task_LcdUpdate);
//	//Scheduler_SetTimerTask(Task_LcdUpdate, 1000);
//}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	microDelay_Init();

	//TIM4_Init();
	//EXTI_Init();

	__enable_irq();
	msDelay(500);
	//***********************************************

	I2C_Init(I2C1);

	//***********************************************
	//Инициализация диспетчера.
	Scheduler_Init();

	Scheduler_SetTask(Task_I2C);
	Scheduler_SetTask(Task_BlinkLed);
	//***********************************************
	msDelay(500);
	//************************************************************************************
	while(1)
	{
		Scheduler_Loop();
	}
	//************************************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_Handler(void){

	Scheduler_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание TIM4.
void TIM4_IRQHandler(void){

	TIM4->SR  &= ~TIM_SR_UIF; //Сброс флага прерывания.
	//TIM4->CR1 &= ~TIM_CR1_CEN;//Counter dasable.

	//LedPC13Toggel();
	//Encoder_ScanLoop(&Encoder);
}
//*******************************************************************************************
//*******************************************************************************************
/**
  * @brief  This function handles EXTI0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void){

	if((EXTI->PR & 0x0001) != 0)  /* Check line 0 has triggered the IT */
	{
		EXTI->PR |= 0x0001; //Сброс бита в регистре EXTI_PR выполняется путем записи в соответствующий бит значения ‘1’.

		//LedPC13Toggel();
	}
//	else /* Should never occur */
//	{
//		//GPIOC->BSRR = (1<<8); /* Switch on orange led to report an error */
//	}
}
//*******************************************************************************************
//*******************************************************************************************




