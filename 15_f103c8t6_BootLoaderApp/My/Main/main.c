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
const char txt_title[] = "BOOT_APPv04";//"_AHT10_";

Encoder_t 	encoder;
DS18B20_t 	DS18B20;
Time_t	  	time = {.hour = 0,
					.min  = 0,
					.sec  = 0,
					.mSec = 0};
//*******************************************************************************************
//*******************************************************************************************
//Структура для обмена данными между приложением и загрузчиком.
//typedef struct __attribute__((packed)){
//
//	uint16_t	applLunch;
//	char 		str[8];
//
//}applLaunch_t;
////************************************************************
//applLaunch_t applLaunch __attribute__((section(".launch"))) = {
//
//	.applLunch = 0x1234,
//	.str = "launch"
//};
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
//************************************************************
void Temperature_Display(DS18B20_t *sensor, uint8_t cursor_x, uint8_t cursor_y){

	const char txt_DS18B20[] = "DS18B20";
	int32_t temperature = sensor->temperature;
	//-------------------
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_PrintBig((char*)txt_DS18B20);
	Lcd_BinToDec(sensor->sensNumber, 1, LCD_CHAR_SIZE_BIG);
	Lcd_PrintBig("= ");

	if(temperature < 0)
	{
		Lcd_ChrBig('-');
		temperature = -temperature;
	}
	else Lcd_ChrBig('+');

	Lcd_BinToDec(temperature/100, 2, LCD_CHAR_SIZE_BIG);
	Lcd_ChrBig('.');
	Lcd_BinToDec(temperature%100, 2, LCD_CHAR_SIZE_BIG);
	Lcd_Print("o ");
	Lcd_ChrBig('C');
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
	Lcd_BinToDec(time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(time.min,  2, LCD_CHAR_SIZE_NORM);//минуты
	//Lcd_Chr(':');
	if(time.sec & 1) Lcd_Chr(':');//Мигание разделительным знаком
	else			 Lcd_Chr(' ');
	Lcd_BinToDec(time.sec,  2, LCD_CHAR_SIZE_NORM);//секунды
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_MPU6050_GetData(void){

	//-------------------
	Lcd_ClearVideoBuffer();
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_MPU6050_");
	//Вывод времени.
	Time_Display(14, 1);
	//-------------------
	Temperature_Display(TEMPERATURE_SENSE_GetSens(1), 1, 3);
}
//************************************************************


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_DHT22_ReadData(void){

	DHT22_ReadData();

	//Ждем 18 мс
	if(DHT22_State() == DHT22_STATE_WAITING_PRESENCE)
	{
		RTOS_SetTask(Task_DHT22_ReadData, 18, 0);
	}
	//Следующее измерение чере 2 сек.
	else RTOS_SetTask(Task_DHT22_ReadData, 2000, 0);
}
//************************************************************
void Task_AHT10_ReadData(void){

	static uint32_t i2cNacCount = 0;
	//-------------------
	AHT10_ReadData();

	//Перенициализация I2C после 5ти NAC
	if(AHT10_GetStatus() == AHTXX_STATUS_ERR_ACK)
	{
		if(++i2cNacCount >= 5)
		{
			I2C_Master_Init(I2C1, I2C_GPIO_NOREMAP, 400000);
		}
	}
	else
	{
		i2cNacCount = 0;
	}
}
//************************************************************
void Task_AHT10_Display(void){

	int32_t temp = 0;
	//-------------------
	Lcd_ClearVideoBuffer();
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print((char*)txt_title);
	//Вывод времени.
	Time_Display(14, 1);
	//-------------------
	//Датчик DS18B20
	Temperature_Display(TEMPERATURE_SENSE_GetSens(1), 1, 2);

	//-------------------
	//Датчик AHT10
	if(AHT10_GetStatus() != AHTXX_STATUS_OK)
	{
		//Проблемы с датчиком...
		Lcd_SetCursor(1, 4);
		Lcd_Print("AHT10 problem...");
	}
	else
	{
		//Температура
		temp = AHT10_GetTemperature();
		Lcd_SetCursor(1, 4);
		Lcd_Print("AHT10_Temp =");
		if(temp < 0)
		{
			temp = -temp;
			Lcd_Chr('-');
		}
		else Lcd_Chr('+');
		Lcd_BinToDec(temp/100, 2, LCD_CHAR_SIZE_NORM);
		Lcd_Chr('.');
		Lcd_BinToDec(temp%100, 2, LCD_CHAR_SIZE_NORM);
		Lcd_Print(" C");

		//Влажность
		Lcd_SetCursor(1, 5);
		Lcd_Print("AHT10_Hum  = ");
		Lcd_BinToDec(AHT10_GetHumidity(), 3, LCD_CHAR_SIZE_NORM);
		Lcd_Print(" %");
	}
	//-------------------
	//Датчик DHT22
	Dht22_State_t state = DHT22_State();

	if(state == DHT22_STATE_WAITING_MEAS)
	{
		//Ожидание завершения измерения
		Lcd_SetCursor(1, 7);
		Lcd_Print("DHT22 measuring...");
	}
	else if(state == DHT22_STATE_PRESENCE_ERR)
	{
		//Ошибка датчика...
		Lcd_SetCursor(1, 7);
		Lcd_Print("DHT22 presence err...");
	}
	else if(state == DHT22_STATE_CHECKSUM_ERR)
	{
		//Ошибка датчика...
		Lcd_SetCursor(1, 7);
		Lcd_Print("DHT22 crc err...");
	}
	else //if(state == DHT22_STATE_OK)
	{
		//Температура
		temp = DHT22_Temperature();
		Lcd_SetCursor(1, 7);
		Lcd_Print("DHT22_Temp = ");
		Lcd_BinToDec(temp/10, 2, LCD_CHAR_SIZE_NORM);
		Lcd_Chr('.');
		Lcd_BinToDec(temp%10, 1, LCD_CHAR_SIZE_NORM);
		Lcd_Print(" C");

		//Влажность
		temp = DHT22_Humidity();
		Lcd_SetCursor(1, 8);
		Lcd_Print("DHT22_Hum  = ");
		Lcd_BinToDec(temp/10, 2, LCD_CHAR_SIZE_NORM);
		Lcd_Chr('.');
		Lcd_BinToDec(temp%10, 1, LCD_CHAR_SIZE_NORM);
		Lcd_Print(" %");
	}
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_GoToBoot(void){

	SharedMemory_SetFlag(DFU_REQUESTED_FLAG);
	NVIC_SystemReset();
}
//************************************************************
void Task_TemperatureRead(void){

	TEMPERATURE_SENSE1_ReadTemperature();
}
//************************************************************
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
	static uint32_t encodTemp = 0;
	//--------------------
	//Мигающая индикация.
	if(Blink(INTERVAL_100_mS))LED_PC13_On();
	else 					  LED_PC13_Off();

	//Перевод из мС в ЧЧ:ММ:СС
	TIME_Calculation(&time, RTOS_GetTickCount());

	//Длительное нажатие на кнопку энкодера.
	if(BUTTON_LongPress(ENCODER_GetButton(&encoder), 100)) pageIndex ^= 1;

	//Если на какой-то странице производится редактирование то выбор страницы запрешен
	//if(!redaction) ENCODER_IncDecParam(&Encoder, &pageIndex, 1, 0, 3);//Выбор сраницы
	switch(pageIndex){
		//--------------------
		case 0:
			//RTOS_SetTask(Task_MPU6050_GetData, 0, 0);
			//Task_MPU6050_GetData();
			//Config_SaveLoop();

			RTOS_SetTask(Task_AHT10_Display, 0, 0);
		break;
		//--------------------
		case 1:
			Lcd_ClearVideoBuffer();
			Lcd_SetCursor(1, 1);
			Lcd_Print(" PAGE 2 ");

			if(ENCODER_GetButton(&encoder) == ENCODER_BUT_PRESSED) encodTemp = 0;

			ENCODER_IncDecParam(&encoder, &encodTemp, 1, 0, 99);
			Lcd_SetCursor(1, 2);
			Lcd_BinToDec(encodTemp, 6, LCD_CHAR_SIZE_NORM);

			//!!! ОТЛАДКА !!!
			RTOS_SetTask(Task_GoToBoot, 0, 0);
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
//************************************************************
void Task_LedBlink(void){

	LED_PC13_Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Метаданные приложения
appMetadata_t appMetadata __attribute__((section(".app_metadata"))) = {

	.metadataVersion  = 0x1234,
	.appMagic		  = METADATA_MAGIC,
	.appType 		  = 0x56,
	.appVectorAddr 	  = 0x08002800,   //вектор сброса приложения
	.appVersion_major = 0x07,
	.appVersion_minor = 0x08,
	.appVersion_patch = 0x09,

	// GIT_SHA is generated by Makefile
	.git_sha = "12345678",

	// populated as part of a post compilation step
	.appSize = 0x12345678,
	.appCrc  = 0xabcdefde,
};
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//***********************************************
	//Это необходимо для работы с загрузчиком.
	BOOT_Init();
	//***********************************************
	STM32_Clock_Init();
	SYS_TICK_Init();
	GPIO_Init();
	I2C_Master_Init(I2C1, I2C_GPIO_NOREMAP, 400000);
	DELAY_Init();

	DELAY_milliS(100);//Эта задержка нужна для стабилизации напряжения питания.
	//***********************************************
	//Чтение настроек
	//Config_Init();

	Lcd_Init();					//Инициализация OLED SSD1306 (I2C1).
	TEMPERATURE_SENSE_Init();	//Инициализация DS18B20
	AHT10_Init();				//Инициализация AHT10
	DHT22_Init();				//Инициализация DHT22
	//***********************************************
	//Инициализация Энкодера.
	encoder.GpioPort_A 	 	= GPIOC;
	encoder.GpioPin_A       = 15;
	encoder.GpioPort_B 	 	= GPIOC;
	encoder.GpioPin_B  	 	= 14;
	encoder.GpioPort_BUTTON = GPIOA;
	encoder.GpioPin_BUTTON  = 0;
	ENCODER_Init(&encoder);
	//***********************************************
	//Инициализация диспетчера.
	RTOS_Init();
	RTOS_SetTask(Task_LcdPageSelection, 100,   10); //20);
	RTOS_SetTask(Task_TemperatureRead,    0, 1000);
	RTOS_SetTask(Task_AHT10_ReadData,     0, 1000);
	RTOS_SetTask(Task_DHT22_ReadData,  2000,    0);

	//!!! ОТЛАДКА !!!
	//RTOS_SetTask(Task_GoToBoot,  5000, 0);
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
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_IT_Handler(void){

	RTOS_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
	ENCODER_ScanLoop(&encoder);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************












