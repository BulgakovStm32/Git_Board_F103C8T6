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
Time_t	  Time;
Encoder_t Encoder;

void Task_UartSend(void);
//*******************************************************************************************
//*******************************************************************************************
uint32_t Led_Blink(uint32_t millis, uint32_t period, uint32_t switch_on_time){

	static uint32_t millisOld = 0;
	static uint32_t flag      = 0;
	//-------------------
	if((millis - millisOld) >= (flag ? (period - switch_on_time) : switch_on_time))
	{
		millisOld = millis;
		flag = !flag;
	}
	return flag;
}
//************************************************************
void Time_Display(uint8_t cursor_x, uint8_t cursor_y){

	//Вывод времени.
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_Print("Time: ");
	Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(Time.min,  2, LCD_CHAR_SIZE_NORM);//минуты
	//Lcd_Chr(':');
	if(Time.sec & 1) Lcd_Chr(':');//Мигание разделительным знаком
	else			 Lcd_Chr(' ');
	Lcd_BinToDec(Time.sec,  2, LCD_CHAR_SIZE_NORM);//секунды
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_TEMPERATURE_Read(void){

	static uint32_t flag = 0;
	//-------------------
	//Во время вращения запрещено чтение температуры.
//	if(MOTOR_GetSpeedRampState() != STOP) return;

	//Чтение темперартуры с датчика 2 смещенео на 500мс
	//относительно чтение темперартуры датчика 1.
	flag ^= 1;
	if(flag) TEMPERATURE_SENSE1_ReadTemperature();
	else	 TEMPERATURE_SENSE2_ReadTemperature();
}
//************************************************************
//ф-я вызыввается каждые 100мс.
void Task_POWER_Check(void){

//	uint32_t 			count;
//	uint32_t 			supplyVoltage;
//	BlinkIntervalEnum_t blinkInterval;
	//-------------------
	//Проверка напряжения АКБ.
//	supplyVoltage = POWER_GetSupplyVoltage();
//	//Напряженеи НИЖЕ миннимального (10,8В) напряжения АКБ.
//	if(supplyVoltage <= BATTERY_VOLTAGE_MIN) POWER_Flags()->f_BatteryLow = FLAG_SET;
//	//Напряжение АКБ НИЖЕ 12В
//	else if(supplyVoltage <= BATTERY_VOLTAGE_WARNING) POWER_Flags()->f_BatteryWarning = FLAG_SET;
//	//Напряжение АКБ выше 12в
//	else POWER_Flags()->f_BatteryWarning = FLAG_CLEAR;
//	//-------------------
//	//Отключение питания по нажатию кнопки или низком напряжении АКБ.
//	if(POWER_PwrButton() == PRESS || POWER_Flags()->f_BatteryLow)
//	{
//		POWER_Flags()->f_PowerOff = FLAG_SET;
//		Task_UartSend();//Отладка
//		//Отключение питания MCU.
//		MOTOR_Disable();
//		FAN_EN_Low();
//		LAMP_PWM_Low();
//		LIDAR_EN_Low();
//		GPS_EN_Low();
//
//		//Определение периода мигающей индикации.
//		if(POWER_Flags()->f_BatteryLow) blinkInterval = INTERVAL_50_mS;
//		else							blinkInterval = INTERVAL_500_mS;
//		//Мигающая индикация отключения в течении 3 сек.
//		count = RTOS_GetTickCount();
//		Blink_ResetCount();
//		PWR_BTN_LED_Low();
//		while((RTOS_GetTickCount() - count) <= 3000)
//		{
//			if(Blink(blinkInterval)) PWR_BTN_LED_Low();
//			else					 PWR_BTN_LED_High();
//		}
//		__disable_irq();
//		PWR_BTN_LED_Low();
//		MCU_POWER_OFF();
//		DELAY_milliS(1000);
//	}
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_Temperature_Display(void){

	Lcd_ClearVideoBuffer();

	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("Dbg_MCUv7");
	//Вывод ошибок обvена по I2C.
	Lcd_SetCursor(11, 1);
	Lcd_Print("I2CNAC=");
//	Lcd_BinToDec(I2C_GetNacCount(STM32_SLAVE_I2C), 4, LCD_CHAR_SIZE_NORM);
	//Вывод времени.
	Time_Display(1, 2);

	//Напряжения питания MCU
	Lcd_SetCursor(1, 3);
	Lcd_Print("MCU_Vin  = ");
//	Lcd_BinToDec(MCUv7_SupplyVoltageVal, 5, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" mV");

	//Значение энкодера MCUv7.
	Lcd_SetCursor(1, 4);
	Lcd_Print("MCU_Enc  = ");
//	Lcd_BinToDec(MCUv7_EncoderVal, 6, LCD_CHAR_SIZE_NORM);

	Lcd_SetCursor(1, 5);
	Lcd_Print("MCU_Sense= ");
//	Lcd_u32ToHex(MCUv7_Sense);
//	Lcd_BinToDec(MCUv7_EncoderVal, 6, LCD_CHAR_SIZE_NORM);

	//Енкодер.
//	static uint16_t tempReg = 0;
//	Encoder_IncDecParam(&Encoder, &tempReg, 5, 0, 100);
//	TIM3->CCR1 = tempReg; //Задаем коэф-т заполнения.

//	Lcd_SetCursor(1, 6);
//	Lcd_Print("Encoder=");
//	Lcd_BinToDec(tempReg, 4, LCD_CHAR_SIZE_NORM);

	//PID
//	Lcd_SetCursor(1, 8);
//	Lcd_Print("PID_Out=");
//	if(PIDcontrol < 0)
//	{
//		PIDcontrol = (PIDcontrol ^ 0xFFFF) + 1;//Уберем знак.
//		Lcd_Chr('-');
//	}
//	else Lcd_Chr(' ');
//	Lcd_BinToDec((uint16_t)PIDcontrol, 4, LCD_CHAR_SIZE_NORM);

//	//Вывод темперетуры DS18B20.
//	Sensor_1.SENSOR_NUMBER    = 1;
//	Sensor_1.TEMPERATURE_SIGN = I2CRxBuf[0];
//	Sensor_1.TEMPERATURE  	  = (uint32_t)((I2CRxBuf[1] << 8) | I2CRxBuf[2]);
//
//	Sensor_2.SENSOR_NUMBER    = 2;
//	Sensor_2.TEMPERATURE_SIGN = I2CRxBuf[3];
//	Sensor_2.TEMPERATURE      = (uint32_t)((I2CRxBuf[4] << 8) | I2CRxBuf[5]);
//
//	Sensor_3.SENSOR_NUMBER    = 3;
//	Sensor_3.TEMPERATURE_SIGN = I2CRxBuf[6];
//	Sensor_3.TEMPERATURE      = (uint32_t)((I2CRxBuf[7] << 8) | I2CRxBuf[8]);

//	Temperature_Display(&Sensor_1, 1, 6);
//	Temperature_Display(&Sensor_2, 1, 7);
//	Temperature_Display(&Sensor_3, 1, 6);
	//Кнопка энкодера.
//	IncrementOnEachPass(&ButtonPressCount, Encoder.BUTTON_STATE);
	Lcd_SetCursor(1, 8);
	Lcd_Print("Button=");
//	Lcd_BinToDec((uint16_t)ButtonPressCount, 4, LCD_CHAR_SIZE_NORM);
}
//************************************************************
void Task_BT(void){

	//--------------------------------
	Txt_Chr('\f');
	Txt_Print("******************\n");
	Txt_Print("_MCUv7_(+BT)\n");

	//Количество зависаний I2C
//	Txt_Print("I2C_ReInit: ");
//	Txt_BinToDec(PROTOCOL_I2C_GetResetCount(), 6);
//	Txt_Chr('\n');

	//Счетчик миллисекунд MCU
	Txt_Print("mS_count: ");
	Txt_BinToDec(RTOS_GetTickCount(), 9);//часы
	Txt_Chr('\n');

	//Вывод Напряжения питания
//	Txt_Print("SupplyVoltage = ");
//	Txt_BinToDec(POWER_GetSupplyVoltage(), 5);
//	Txt_Print(" mV\n");

	//Вывод данных энкодера.
	Txt_Print("ENCOD_OFFSET  : ");
//	Txt_BinToDec(encoderOffset, 6);
	Txt_Chr('\n');

	Txt_Print("ENCOD_CODE  : ");
//	Txt_BinToDec(encoderTicks, 6);
	Txt_Chr('\n');

	Txt_Print("ENCOD_ANGLE: ");
//	Txt_BinToDec((uint32_t)(Angle * 1000), 6);
	Txt_Chr('\n');

	Txt_Print("ENCOD_RPM   : ");
//	Txt_BinToDec((uint32_t)(RPM), 6);
	Txt_Chr('\n');

	//Вывод температуры.
	Txt_Print("TSens1= ");
	if(TEMPERATURE_SENSE_GetSens(1)->TemperatureSign == DS18B20_SIGN_POSITIVE)
	{
		 Txt_Chr('+');
	}
	else Txt_Chr('-');
	Txt_BinToDec(TEMPERATURE_SENSE_GetSens(1)->Temperature/10, 2);
	Txt_Chr('.');
	Txt_BinToDec(TEMPERATURE_SENSE_GetSens(1)->Temperature%10, 1);
	Txt_Print(" C");
	Txt_Chr('\n');

	Txt_Print("TSens2= ");
	if(TEMPERATURE_SENSE_GetSens(2)->TemperatureSign == DS18B20_SIGN_POSITIVE)
	{
		 Txt_Chr('+');
	}
	else Txt_Chr('-');
	Txt_BinToDec(TEMPERATURE_SENSE_GetSens(2)->Temperature/10, 2);
	Txt_Chr('.');
	Txt_BinToDec(TEMPERATURE_SENSE_GetSens(2)->Temperature%10, 1);
	Txt_Print(" C");
	Txt_Chr('\n');

	//Количество нажатий на кнопку.
//	Txt_Print("ButtonPress=");
//	Txt_BinToDec(ButtonPressCount, 4);

	//Состояние питания MCU.
//		 if(POWER_Flags()->f_BatteryWarning) Txt_Print("BATTERY < 12v!!!\n");
//	else if(POWER_Flags()->f_BatteryLow) 	 Txt_Print("BATTERY LOW!!!\n");
//	else if(POWER_Flags()->f_PowerOff)   	 Txt_Print("POWER OFF!!!\n");
//	else									 Txt_Print("BATTERY OK.\n");

	Txt_Chr('\n');
	Txt_Print("******************\n");
	//--------------------------------
	//USART1_TX -> DMA1_Channel4
	//USART2_TX -> DMA1_Channel7
	DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_LcdUpdate(void){

	LedPC13Toggel();

	TIME_Calculation(&Time, RTOS_GetTickCount());
	//Выбор сраницы отображения Енкодером.
	static uint32_t encoder = 0;
	//Encoder_IncDecParam(&Encoder, &encoder, 1, 0, 2);
	switch(encoder){
		//--------------------
		case 0:
			//RTOS_SetTask(Task_RequestFromMCUv7,     5, 0);
			RTOS_SetTask(Task_Temperature_Display, 10, 0);
		break;
		//--------------------
		case 1:
			//RTOS_SetTask(Task_DS2782,	  	  5,  0);
			//RTOS_SetTask(Task_DS2782_Display, 10, 0);
		break;
		//--------------------
		default:
			Lcd_ClearVideoBuffer();
			Lcd_SetCursor(1, 1);
			Lcd_Print(" EMPTY PAGE ");
		break;
		//--------------------
	}
	//Обновление изображения на экране.
	//Очистка видеобуфера производится на каждой странице.
	Lcd_Update(); //вывод сделан для LM6063D
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//***********************************************
	//Инициализация периферии STM32.
 	STM32_Clock_Init();
	GPIO_Init();
	DELAY_Init();

	DELAY_milliS(500);//500mS - Задержка для стабилизации напряжения патания.
	//***********************************************
	Lcd_Init();	//Инициализация датчиков температуры.




//	TEMPERATURE_SENSE_Init(); //Инициализация датчиков температуры.
//	OPT_SENS_Init();		  //Инициализация оптических дадчитков наличия крышки объетива и наличия АКБ.
//	ENCODER_Init();			  //Инициализация энкодера.
//	MOTOR_Init();			  //Инициализация драйвера мотора.
//	PROTOCOL_I2C_Init();	  //Инициализация протокола обмена.


	//Иницияализация USART и модуля Bluetooth HC-06. Для отладки.
//	USART_DMA_Init(USART2, 9600);
//	//Txt_Print("AT"); //Работает!! CR+LF  добовлять в конце не нужно!!!
//	/* AT+BAUDx - установка скорости, где x - код скорости из таблицы,
//	 * т.е. чтобы задать скорость 9600 надо подать по последовательному порту
//	 * модуля команду AT+BAUD4
//	 * Значение - Скорость (бод)
//	 * 1 - 1200
//	 * 2 - 2400
//	 * 3 - 4800
//	 * 4 - 9600
//	 * 5 - 19200
//	 * 6 - 38400
//	 * 7 - 57600
//	 * 8 - 115200
//	 * 9 - 230400
//	 */
//	Txt_Print("AT+BAUD8"); //CR+LF  добовлять в конце не нужно!!!
//	DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
//	Txt_Buf()->bufIndex = 0;
//	DELAY_microS(50);


	//Иницияализация USART и модуля Bluetooth HC-05. Для отладки.
	USART_DMA_Init(USART2, 9600);
//	Txt_Print("AT+UART=115200,0,0\r\n");
//	DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
//	Txt_Buf()->bufIndex = 0;
//	DELAY_microS(50);
//	USART_DMA_Init(USART2, 115200);
	//***********************************************
	//Ини-я диспетчера.
	RTOS_Init();
	RTOS_SetTask(Task_LcdUpdate, 0, 25);
	RTOS_SetTask(Task_BT, 500, 500);

	//***********************************************
	SysTick_Init();
	//Приоритет прерывания.
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
	__enable_irq();
	//**************************************************************
	while(1)
	{
		RTOS_DispatchLoop();
		//__WFI();//Sleep
	}
	//**************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_IT_Handler(void){

	RTOS_TimerServiceLoop();
	Encoder_ScanLoop(&Encoder);

	msDelay_Loop();
	Blink_Loop();
//	GPIO_CheckLoop();
}
//*******************************************************************************************
//*******************************************************************************************











