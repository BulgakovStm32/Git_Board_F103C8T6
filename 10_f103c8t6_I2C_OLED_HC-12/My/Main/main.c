/*
 * 	main.c
 *
 *  Created on: 19 октября 2021 года.
 *  Autho     : Беляев А.А.
 *
 *	Описание        :
 *  Датчики         :
 *  Вывод информации:
 *
 */
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
Time_t	  Time;

DS18B20_t Sensor_1;
DS18B20_t Sensor_2;
DS18B20_t Sensor_3;

DS2782_t  DS2782;

Encoder_t Encoder;

I2C_IT_t I2cDma;


static uint32_t ButtonPressCount = 0;
static uint32_t hc12_BaudRate    = 123;

static uint32_t redaction = 0;

//Работа с Si5351
static uint32_t calibr	 = SI5351_XTAL_FREQ_DEFAULT;
static uint32_t freq 	 = 3650 * 1000; //SI5351_MIN_FREQ;
static uint32_t stepFreq = 1000;
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void IncrementOnEachPass(uint32_t *var, uint32_t event, uint32_t step, uint32_t max){

		   uint32_t riseReg  = 0;
	static uint32_t oldState = 0;
	//-------------------
	riseReg  = (oldState ^ event) & event;
	oldState = event;
//	if(riseReg) (*var)++;
	if(riseReg)
	{
		if(step == 0) step = 1;
		if((*var) < max) (*var)+= step;//Проверка на  максимум.
		else             (*var) = 0;   //Закольцовывание редактирования параметра.
	}
}
//************************************************************
uint32_t Led_Blink(uint32_t millis, uint32_t period, uint32_t switch_on_time){

	static uint32_t millisOld = 0;
	static uint32_t flag      = 0;
	//-------------------
	if((millis - millisOld) >= (flag ? (period - switch_on_time) : switch_on_time ))
	{
		millisOld = millis;
		flag = !flag;
	}
	return flag;
}
//************************************************************
void Temperature_Display(DS18B20_t *sensor, uint8_t cursor_x, uint8_t cursor_y){

	uint32_t temperature = sensor->Temperature;
	uint32_t sensorNum   = sensor->SensorNumber;
	//-------------------
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_Print("Sens");
	Lcd_BinToDec(sensorNum, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print("= ");
	if(TEMPERATURE_SENSE_GetSens(sensorNum)->TemperatureSign == DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
	else                    														  Lcd_Chr('+');
	Lcd_BinToDec(temperature/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(temperature%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print("o ");
	Lcd_Chr('C');
}
//************************************************************
void Temperature_TxtDisplay(DS18B20_t *sensor){

	uint32_t temperature = sensor->Temperature;
	uint32_t sensorNum   = sensor->SensorNumber;
	//-------------------
	Txt_Print("Sens");
	Txt_BinToDec(sensorNum, 1);
	Txt_Print("= ");
	if(TEMPERATURE_SENSE_GetSens(sensorNum)->TemperatureSign == DS18B20_SIGN_NEGATIVE)Txt_Chr('-');
	else                    														  Txt_Chr('+');
	Txt_BinToDec(temperature/10, 2);
	Txt_Chr('.');
	Txt_BinToDec(temperature%10, 1);
	Txt_Print(" C\n");
}
//************************************************************
void Time_Display(uint8_t cursor_x, uint8_t cursor_y){

	//Вывод времени.
	Lcd_SetCursor(cursor_x, cursor_y);
//	Lcd_Print("Time: ");
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
void Task_Temperature_Read(void){

//	TemperatureSens_ReadTemperature(&Sensor_1);
//	TemperatureSens_ReadTemperature(&Sensor_2);
//	TemperatureSens_ReadTemperature(&Sensor_3);
}
//************************************************************
void Task_Temperature_Display(void){

//	static char xtxBuf[32] = {0};
	//-------------------
	Lcd_ClearVideoBuffer();

	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_HC-12_");
	//Вывод ошибок обvена по I2C.
	Lcd_SetCursor(11, 1);
	Lcd_Print("I2CNAC=");
//	Lcd_BinToDec(I2C_GetNacCount(STM32_SLAVE_I2C), 4, LCD_CHAR_SIZE_NORM);
	//Вывод времени.
	Time_Display(1, 2);

	//Скорсть обмена с модулем HС-12
	Lcd_SetCursor(1, 3);
	Lcd_Print("BaudRate = ");
	Lcd_BinToDec(hc12_BaudRate, 6, LCD_CHAR_SIZE_NORM);

	//Вывод принятой строки
//	RING_BUFF_CopyRxBuff(xtxBuf);//копирование принятых данных
//	Lcd_SetCursor(1, 4);
//	Lcd_Print(xtxBuf);


	//Значение энкодера MCUv7.
//	Lcd_SetCursor(1, 4);
//	Lcd_Print("MCU_Enc  = ");
//	Lcd_BinToDec(MCUv7_EncoderVal, 6, LCD_CHAR_SIZE_NORM);

//	Lcd_SetCursor(1, 5);
//	Lcd_Print("MCU_Sense= ");
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
	IncrementOnEachPass(&ButtonPressCount, ENCODER_GetButton(&Encoder), 1, 100);
	Lcd_SetCursor(1, 8);
	Lcd_Print("Button=");
	Lcd_BinToDec(ButtonPressCount, 4, LCD_CHAR_SIZE_NORM);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_HC12(void){

	//--------------------------------
	Txt_Chr('\f');

	Txt_Print("******************\n");

	Txt_Print("_MCUv7_(+BT)\n");

	Txt_Print("Time: ");
	Txt_BinToDec(Time.hour, 2);//часы
	Txt_Chr(':');
	Txt_BinToDec(Time.min, 2); //минуты
	Txt_Chr(':');
	Txt_BinToDec(Time.sec, 2); //секунды
	Txt_Chr('\n');

	//Вывод темперетуры DS18B20.
	Temperature_TxtDisplay(&Sensor_1);
	Temperature_TxtDisplay(&Sensor_2);
	Temperature_TxtDisplay(&Sensor_3);
	//Txt_Chr('\n');

	//--------------------------------
	//Вывод данных DS2782.
	//Вывод адреса на шине I2C
	Txt_Chr('\n');
	Txt_Print("DS2782_I2C_ADDR:");
	Txt_Print("0x");
	Txt_u8ToHex(DS2782.I2C_Address);
	Txt_Chr('\n');

	//Вывод температуры.
	Txt_Print("Bat_T=");
	Txt_BinToDec(DS2782.Temperature/10, 2);
	Txt_Chr('.');
	Txt_BinToDec(DS2782.Temperature%10, 1);
	Txt_Print(" C");
	Txt_Chr('\n');

	//Вывод напряжения на АКБ.
	Txt_Print("Bat_U=");
	Txt_BinToDec(DS2782.Voltage/100, 2);
	Txt_Chr('.');
	Txt_BinToDec(DS2782.Voltage%100, 2);
	Txt_Chr('V');
	Txt_Chr('\n');

	//Вывод тока потребления от АКБ.
	Txt_Print("Bat_I  =");
	if(DS2782.Current < 0)
	{
		DS2782.Current = (DS2782.Current ^ 0xFFFF) + 1;//Уберем знак.
		Txt_Chr('-');
	}
	else Txt_Chr(' ');
	Txt_BinToDec(DS2782.Current, 4);
	Txt_Print("mA");
	Txt_Chr('\n');
	Txt_Chr('\n');

	//Количество нажатий на кнопку.
	Txt_Print("ButtonPress=");
	Txt_BinToDec(ButtonPressCount, 4);

	Txt_Chr('\n');
	Txt_Print("******************123456\n");
	//--------------------------------
	//USART2_TX -> DMA1_Channel7
	DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_SI5351(void){

	static uint32_t oldFreq   = 0;
	static uint32_t oldCalibr = 0;
	//-------------------
	//Если производится редактирование то выбор частоты запрешен
	if(!redaction) return;

	//Установка шага установки частоты.
		 if(redaction == 2) ENCODER_IncDecParam(&Encoder, &freq, stepFreq, SI5351_MIN_FREQ , SI5351_MAX_FREQ);
	//Установка частоты энкодером
	else if(redaction == 4) ENCODER_IncDecParam(&Encoder, &stepFreq, 100, 100 , 100000);
	//Калибровка Si5351
	else if(redaction == 6) ENCODER_IncDecParam(&Encoder, &calibr, 10, 24000000 , 26000000);

	Si5351_SetXtalFreq(calibr);

	//Один раз передаем параметры в Si5351
	if(freq != oldFreq || calibr != oldCalibr) Si5351_SetF0(freq);
	oldFreq   = freq;
	oldCalibr = calibr;

//	Config_Save()->xtalFreq = 125 * 1000;
}
//************************************************************
void Task_SI5351_Display(void){

		   uint32_t temp 	 = 0;
//	static uint32_t strIndex = 0;
	//-------------------
	Lcd_ClearVideoBuffer();
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_Si5351_");
	//Вывод времени.
	Time_Display(14, 1);
	//-------------------
	//По нажатию на кнопку энкодера переход к выбору редактируемого параметра.
	IncrementOnEachPass(&redaction, ENCODER_GetButton(&Encoder), 2, 6);

	//Ходим по пунктам страницы по нажатию на кнопку энкодера.
	//if(redaction) Lcd_PrintStringAndNumber(20, (1 + redaction), "<=", 0, 0);
	if(redaction)
	{
		//Мигающий символ "<=" справа от редактируемой строки
		Lcd_SetCursor(20, (1 + redaction));
		if(Blink(INTERVAL_250_mS)) Lcd_Print("<=");
		else					   Lcd_Print("  ");
	}
	//-------------------
	//Отображение установленной частота
	Lcd_SetCursor(1, 3);

	//Единицы МГц - два разряда.
	Lcd_BinToDec(freq/1000000, 2, LCD_CHAR_SIZE_BOLD);
	Lcd_ChrBold('.');

	//кГц - три разряда.
	temp = freq % 1000000;
	Lcd_BinToDec(temp/1000 , 3, LCD_CHAR_SIZE_BOLD);
	Lcd_Chr(' ');

	//Гц - три разряда.
	temp = freq % 1000;
	Lcd_BinToDec(temp, 3, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" Hz");

	//Отображение шага установки частоты.
	Lcd_PrintStringAndNumber(1, 6, "Step  : ", stepFreq, 6);
	Lcd_Print("   Hz");

	//Отображение значение калибровки
	Lcd_PrintStringAndNumber(1, 7, "Calibr: ", calibr, 8);
	Lcd_Print(" Hz");

	//----------------------------------------------
	//Горизонтальная шкала

	//num подряд идущих двойных вертикальных высоких палочек с шагом step
	uint8_t	num    = 5;				  //Необходимое кол-во вертикальных высоких палочек на шкале.
	uint8_t step_x = 100 / (num - 1); //Шаг между палочками
	uint8_t n_x    = 2;				  //Начальная координата по Х первой палочки.

	for(uint8_t i = 0; i < num; i++)
	{
		Lcd_Line(n_x, 1, n_x, 5, PIXEL_ON); //Вертикальная палочка высотой 5 пикселей.
		//Lcd_Line(1*n_x+1, 1, 1*n_x+1, 5, PIXEL_ON);
		n_x += step_x;
	}

	//Циферки над высокими черточками
//	Lcd_SetCursor(1, 7);
//	Lcd_Print("0   25  50  75  100");

	//lowNum подряд идущих двойных вертикальных низких палочек
	uint8_t lowNum    = (uint8_t)50;//Кол-во палочек, макс 127
	uint8_t lowStep_x = 1;				 //Шаг между палочками
	uint8_t lowN_x    = 2;				 //Начальная координата по Х первой палочки.

	lowNum /= lowStep_x;//равномерное распределение шагов на всю шкалу.

	for(uint8_t i = 0; i < lowNum; i++)
	{
		Lcd_Line(lowN_x ,1 ,lowN_x ,3 ,PIXEL_ON);//Вертикальная палочка высотой 3 пикселя.
		lowN_x += lowStep_x;
	}
	//----------------------------------------------
}
//************************************************************
void Task_AnalogMeter(void){

	//-------------------
	Lcd_ClearVideoBuffer();
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("AnalogMeter");
	//Вывод времени.
	Time_Display(14, 1);
	//-------------------
	static uint32_t encod = 0;

	ENCODER_IncDecParam(&Encoder, &encod, 1, 0 , 179);

	uint8_t radius = 50;
	uint8_t angle  = encod;//Time.sec * 3; //175.0; //угол на который нужно повернуть стрелку

	float rad = (angle * M_PI) / 180.0;
	float x   = cosf(rad) * radius;
	float y   = sinf(rad) * radius;

	//-------------------
	//Полуокружность
//	Lcd_Circle(63, 63, 42, PIXEL_ON);
	//-------------------
	//Стрелка
	uint8_t x1 = 0;

	if(angle <= 90) x1 = 63 + (uint8_t)x;
	else			x1 = 63 - (uint8_t)(-1 * x);

	Lcd_Line  (63, 0, x1, (uint8_t)y, PIXEL_ON);
	Lcd_Circle(63, 63, 2, PIXEL_ON);
	//-------------------
	//риски-метки шкалы
	for(float i = 0; i < M_PI; i += M_PI / 6)
	{
		//Lcd_Line(0 - 127 * cos(i), 127 - 127 * sin(i), 0 - 115 * cos(i), 127 - 115 * sin(i), PIXEL_ON); // риски-метки шкалы
		Lcd_Line(64 + radius * cos(i),
				 0  + radius * sin(i),

				 64 + (radius-10) * cos(i),
				 0  + (radius-10) * sin(i),

				 PIXEL_ON); // риски-метки шкалы
	}
	//-------------------
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_LcdPageSelection(void){

	static uint32_t pageIndex = 0;
	//-----------------------------
	if(Blink(INTERVAL_100_mS))LED_PC13_On();
	else 					  LED_PC13_Off();

	TIME_Calculation(&Time, RTOS_GetTickCount());
	//Если на какой-то странице производится редактирование то выбор страницы запрешен
//	if(!redaction) ENCODER_IncDecParam(&Encoder, &pageIndex, 1, 0, 2);//Выбор сраницы
	switch(pageIndex){
		//--------------------
		case 0:
			RTOS_SetTask(Task_SI5351,         0, 0);
			//RTOS_SetTask(Task_SI5351_Display, 0, 0);
			RTOS_SetTask(Task_AnalogMeter, 0, 0);
		break;
		//--------------------
		case 1:
			RTOS_SetTask(Task_Temperature_Read,    0, 0);
			RTOS_SetTask(Task_Temperature_Display, 0, 0);
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
	Lcd_Update(); //вывод сделан для SSD1306
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//***********************************************
	//Drivers.
	STM32_Clock_Init();
	SYS_TICK_Init();
	GPIO_Init();
	DELAY_Init();
	I2C_Master_Init(I2C1, I2C_GPIO_NOREMAP, 400000);

	DELAY_milliS(250);//Эта задержка нужна для стабилизации напряжения патания.
					  //Без задержки LCD-дисплей не работает.
	//__enable_irq();
	//***********************************************
	Config_Init();					   //Чтение настроек
	SSD1306_Init(SSD1306_128x64);      //Инициализация OLED SSD1306 (I2C1).
	Si5351_Init();					   //Инициализация Si5351 (I2C1).

	HC12_Init(HC12_BAUD_RATE_57600);   //Инициализация HC-12 (USART2).
	hc12_BaudRate = HC12_GetBaudRate();
	//***********************************************
	//Инициализация Энкодера.
	Encoder.GpioPort_A 	 	= GPIOB;
	Encoder.GpioPin_A       = 10;
	Encoder.GpioPort_B 	 	= GPIOB;
	Encoder.GpioPin_B  	 	= 11;
	Encoder.GpioPort_BUTTON = GPIOB;
	Encoder.GpioPin_BUTTON  = 1;
	ENCODER_Init(&Encoder);
	//***********************************************
	//Инициализация диспетчера.
	RTOS_Init();
//	RTOS_SetTask(Lcd_Update, 			0, 5); //Обновление изображения на экране каждые 10мс
	RTOS_SetTask(Task_LcdPageSelection, 0, 10);
//	RTOS_SetTask(Task_HC12,      		0, 1000);
//	RTOS_SetTask(Config_SaveLoop, 	    0, 1000);
	//***********************************************
	SYS_TICK_Control(SYS_TICK_ON);
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
	msDelay_Loop();
	Blink_Loop();
	ENCODER_ScanLoop(&Encoder);
}
//*******************************************************************************************
//******************************************************************************************
