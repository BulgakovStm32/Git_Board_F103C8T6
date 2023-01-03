/*
 * 	main.c
 *
 *  Created on: 3 января 2023 года.
 *  Autho     : Беляев А.А.
 *
 *	Описание        :Работа с синтезатором Si5351
 *
 */
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
Time_t	  Time;
Encoder_t Encoder;

//DS18B20_t Sensor_1;
//DS18B20_t Sensor_2;
//DS18B20_t Sensor_3;
//DS2782_t  DS2782;

static uint32_t redaction = 0;
//Работа с Si5351
static uint32_t si5351_xtalFreq	= 0;
static uint32_t si5351_stepFreq = 0;
static uint32_t si5351_freq 	= 3650 * 1000;
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
/*
 * Функция пропорционально переносит значение (value) из текущего диапазона значений (fromLow .. fromHigh)
 * в новый диапазон (toLow .. toHigh), заданный параметрами.
 * Функция map() не ограничивает значение рамками диапазона, как это делает функция constrain().
 *
 * Contrain() может быть использован до или после вызова map(), если необходимо ограничить допустимые значения заданным диапазоном.
 * Обратите внимание, что "нижняя граница" может быть как меньше, так и больше "верхней границы".
 * Это может быть использовано для того чтобы "перевернуть" диапазон:
 * y = map(x, 1, 50, 50, 1);
 *
 * Возможно использование отрицательных значений:
 * y = map(x, 1, 50, 50, -100);
 *
 * Функция map() оперирует целыми числами.
 * При пропорциональном переносе дробная часть не округляется по правилами, а просто отбрасывается.
 *
 * Параметры
 * value   : значение для переноса
 * in_min  : нижняя граница текущего диапазона
 * in_max  : верхняя граница текущего диапазона
 * out_min : нижняя граница нового диапазона, в который переноситься значение
 * out_max : верхняя граница нового диапазона
 * Возвращаемое значение:
 * Значение в новом диапазоне */

long map(long value, long in_min, long in_max, long out_min, long out_max){

  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//************************************************************
int32_t map_I32(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max){

  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
void Lcd_AnalogScale(uint8_t angle){

	const uint8_t markRadius = 95;// Радиус шкалы.
	int8_t x0 =  64;		  	  // Х-координата центра шкалы
	int8_t y0 = -65; 	  		  // Y-координата центра шкалы.
	float cos;
	float sin;
	//-------------------
	//Рисуем риски-метки шкалы
	for(float i = 0; i < M_PI_2; i += M_PI_2/8)
	{
		cos = cosf(i + M_PI_4);
		sin = sinf(i + M_PI_4);
		Lcd_Line(x0 +  markRadius    * cos,	//x1
				 y0 +  markRadius    * sin,	//y1
				 x0 + (markRadius-6) * cos, //x2
				 y0 + (markRadius-6) * sin, //y2
				 PIXEL_ON);
	}
	//Стрелка.
	angle = (180 - angle); 	     	   //Это нужно чтобы стрелка двигалась слева направо.
	float rad = angle * (M_PI / 180.0);//перевод углов в радианы
	cos = cosf(rad);
	sin = sinf(rad);
	//x0 += 1; //небольшое смещение по Х что бы стрелка точно поподала в среднюю риску.
	y0 += 5; //небольшое смещение по Y что бы стрелка поподала в риски.
	Lcd_Line(x0 + markRadius * cos,
			 y0 + markRadius * sin,
			 x0 + 1 * cos,
			 y0 + 1 * sin,
			 PIXEL_ON);
}
//************************************************************
//Горизонтальная шкала с рисками.
/*
 * Параметры:
 * x0	   : начальная координата шкалы по Х (мин. 0, макс. 127).
 * y0	   : начальная координата шкалы по Y (мин. 0, макс. 63).
 * sigLevel: отображаемое значение (мин. 0, макс. 127)
 *
 */
void Lcd_HorizontalProgressBar(uint8_t x0, uint8_t y0, uint8_t level){

//	uint8_t x0	      = 3;		//Начальная координата шкалы по Х.
//	uint8_t y0	      = 2;		//Начальная координата шкалы по Y.
//	uint8_t sigLevel  = (uint8_t)map(percent, 0, 100, 0, maxVal);//Отображаемый сигнал . Мин. 0, макс. 127.

	const uint8_t numMarks  = 5;   //Необходимое кол-во вертикальных рисок на шкале.
	const uint8_t scaleStep = 1;   //Шаг приращения шкалы в пикселях
	const uint8_t maxVal    = 100; //Максимальное отображаемое значение, макс. 127 (на дисплее макс 128 пикселей).
	uint8_t marksStep = maxVal / (numMarks-1);//Шаг между рисками.
	uint8_t stepCount = x0;
	//-------------------
	//Рисуем риски высотой 5 пикселей.
	for(uint8_t i = 0; i < numMarks; i++)
	{
		Lcd_Line(stepCount, y0, stepCount, y0+4, PIXEL_ON);//Первая Вертикальная палочка высотой 5 пикселей.
		//Lcd_Line(stepCount+1, 1, stepCount+1, 5, PIXEL_ON);//Вторая Вертикальная палочка высотой 5 пикселей.
		stepCount += marksStep;
	}
	//Рисуем шкалу высотой 3 пикселя.
	if(level > maxVal) level = maxVal;
	level /= scaleStep;						  //Равномерное распределение шагов на всю шкалу.
	level += x0;							  //
	Lcd_Line(x0, y0  , level, y0  , PIXEL_ON);//Горизонтальная линия высотой 1 пиксель.
	Lcd_Line(x0, y0+1, level, y0+1, PIXEL_ON);//Горизонтальная линия высотой 1 пиксель.
	Lcd_Line(x0, y0+2, level, y0+2, PIXEL_ON);//Горизонтальная линия высотой 1 пиксель.
	//Циферки над рисками шкалы
//	Lcd_SetCursor(1, 8);
//	Lcd_Print("0   25  50  75  100");
}
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
void Task_SI5351(void){

	static uint32_t oldFreq     = 0;
	static uint32_t oldXtalFreq = 0;
	//-------------------
	//проверка наличия модуля Si5351.
	if(!Si5351_Check()) return;

	//Один раз передаем параметры в Si5351
	if(si5351_freq != oldFreq  || si5351_xtalFreq != oldXtalFreq)
	{
		Si5351_SetXtalFreq(si5351_xtalFreq);//Корректировка значения кварца.
		Si5351_SetF0(si5351_freq);
		oldFreq     = si5351_freq;
		oldXtalFreq = si5351_xtalFreq;
	}
}
//************************************************************
void Task_SI5351_Setting(void){

	static uint32_t settingIndex = 0;
	static uint32_t redaction    = 0;
	//-------------------
	Lcd_ClearVideoBuffer();
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_Si5351_Setting");
	//Вывод времени.
	//Time_Display(14, 1);
	//-------------------
	//Нет модуля, то выводим сообщение
	if(!Si5351_Check())
	{
		Lcd_SetCursor(1, 3);
		Lcd_PrintBold("no module");
		return;
	}
	//-------------------
	//Мигающий символ "<=" справа от редактируемой строки
	Lcd_SetCursor(20, (2 + settingIndex));
	if(redaction)
	{
		if(Blink(INTERVAL_250_mS)) Lcd_Print("<=");
		else					   Lcd_Print("  ");
	}
	else
	{
		Lcd_Print("<=");
		//Выдор редактируемого параметра энкодером.
		ENCODER_IncDecParam(&Encoder, &settingIndex, 1, 0, 1);
	}

	//По нажатию на кнопку энкодера переход к выбору редактируемого параметра.
	IncrementOnEachPass(&redaction, ENCODER_GetButton(&Encoder), 1, 1);
	//Установка шага установки частоты
		 if(redaction && settingIndex == 0) ENCODER_IncDecParam(&Encoder, &si5351_stepFreq, 100, 100 , 100000);
	//Установка частоты кварца Si5351
	else if(redaction && settingIndex == 1) ENCODER_IncDecParam(&Encoder, &si5351_xtalFreq, 10, 23500000, 25500000);
	//-------------------
	//Отображение шага установки частоты.
	Lcd_PrintStringAndNumber(1, 2, "Step  : ", si5351_stepFreq, 6);
	Lcd_Print("   Hz");

	//Отображение значение калибровки
	Lcd_PrintStringAndNumber(1, 3, "Calibr: ", si5351_xtalFreq, 8);
	Lcd_Print(" Hz");
	//-------------------
	//Сохранение настроек во FLASH
	Config_Save()->stepFreq = si5351_stepFreq;
	Config_Save()->xtalFreq = si5351_xtalFreq;
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
	//Нет модуля, то выводим сообщение и выходим
	if(!Si5351_Check())
	{
		Lcd_SetCursor(1, 3);
		Lcd_PrintBold("no module");
		redaction = 0;
		return;
	}
	//-------------------
	//Установка частоты энкодером
	ENCODER_IncDecParam(&Encoder,
						&si5351_freq,
						si5351_stepFreq,
						SI5351_MIN_FREQ,
						SI5351_MAX_FREQ);
	//-------------------
	//Отображение установленной частота
	Lcd_SetCursor(1, 3);

	//МГц - два разряда.
	Lcd_BinToDec(si5351_freq/1000000, 2, LCD_CHAR_SIZE_BOLD);
	Lcd_ChrBold('.');

	//кГц - три разряда.
	temp = si5351_freq % 1000000;
	Lcd_BinToDec(temp/1000 , 3, LCD_CHAR_SIZE_BOLD);
	Lcd_Chr(' ');

	//Гц - три разряда.
	//Lcd_SetCursor(14, 4);
	temp = si5351_freq % 1000;
	Lcd_BinToDec(temp, 3, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" Hz");
	//-------------------
	//Отображение шага установки частоты.
	Lcd_PrintStringAndNumber(1, 5, "Step:", si5351_stepFreq, 6);
	Lcd_Print(" Hz");
	//-------------------
	//Горизонтальная шкала
	Lcd_HorizontalProgressBar(2, 0, 50);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//void IncrementOnEachPass(uint32_t *var, uint32_t event, uint32_t step, uint32_t max){
//
//		   uint32_t riseReg  = 0;
//	static uint32_t oldState = 0;
//	//-------------------
//	riseReg  = (oldState ^ event) & event;
//	oldState = event;
////	if(riseReg) (*var)++;
//	if(riseReg)
//	{
//		if(step == 0) step = 1;
//		if((*var) < max) (*var)+= step;//Проверка на  максимум.
//		else             (*var) = 0;   //Закольцовывание редактирования параметра.
//	}
//}

//Длительное нажатие на кнопку энкодера.
uint32_t BUTTON_LongPress(uint32_t butState, uint32_t delay){

	static uint32_t mScount = 0;
	static uint32_t flag    = 0;
	//--------------------
	if(butState)
	{
		if(!flag && (RTOS_GetTickCount() - mScount) >= delay)
		{
			flag = 1;
			return 1;
		}
	}
	else
	{
		flag = 0;
		mScount = RTOS_GetTickCount();
	}
	return 0;
}
//************************************************************
void Task_LcdPageSelection(void){

	static uint32_t pageIndex = 0;
	//--------------------
	//Мигающая индикация.
	if(Blink(INTERVAL_100_mS))LED_PC13_On();
	else 					  LED_PC13_Off();

	//Перевод из мС в ЧЧ:ММ:СС
	TIME_Calculation(&Time, RTOS_GetTickCount());

	//Длительное нажатие на кнопку энкодера.
	if(BUTTON_LongPress(ENCODER_GetButton(&Encoder), 2000)) pageIndex ^= 1;

	//Если на какой-то странице производится редактирование то выбор страницы запрешен
	//if(!redaction) ENCODER_IncDecParam(&Encoder, &pageIndex, 1, 0, 3);//Выбор сраницы
	switch(pageIndex){
		//--------------------
		case 0:
			RTOS_SetTask(Task_SI5351,         0, 0);
			RTOS_SetTask(Task_SI5351_Display, 0, 0);
			Config_SaveLoop();
		break;
		//--------------------
		case 1:
			RTOS_SetTask(Task_SI5351,         0, 0);
			RTOS_SetTask(Task_SI5351_Setting, 0, 0);
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
	I2C_Master_Init(I2C1, I2C_GPIO_NOREMAP, 400000);

	DELAY_Init();
	DELAY_milliS(500);//Эта задержка нужна для стабилизации напряжения патания.
					  //Без задержки LCD-дисплей не работает.
	//***********************************************
	//Чтение настроек
	Config_Init();

	//Инициализация Si5351 (I2C1).
	si5351_stepFreq = Config()->stepFreq;
	si5351_xtalFreq = Config()->xtalFreq;
	Si5351_Init();
	Si5351_SetXtalFreq(si5351_xtalFreq);
	Si5351_SetF0(si5351_freq);

	//Инициализация OLED SSD1306 (I2C1).
	SSD1306_Init(SSD1306_128x64);
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
	RTOS_SetTask(Task_LcdPageSelection, 0, 6);
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
