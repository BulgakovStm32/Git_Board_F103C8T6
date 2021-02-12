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
typedef struct{
	uint16_t DS18B20 :1;

}Flags_t;

Flags_t	FlagsStr;
//---------------------------
typedef struct{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}Time_t;
static Time_t Time;
//---------------------------
static uint32_t secCounter  = 0;
//static uint16_t Temperature = 0;
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
void Time_Calculation(uint32_t count){

	Time.hour =  count / 3600;
	Time.min  = (count % 3600) / 60;
	Time.sec  =  count % 60;
}
//************************************************************
void Led_Blink(void){

	if(Blink(INTERVAL_100_mS)) LedPC13On();
	else					   LedPC13Off();

}
//************************************************************
void Temperature_Get(uint16_t *pData){

	if(FlagsStr.DS18B20 == 1)
		{
			FlagsStr.DS18B20 = 0;
			*pData = DS18B20_ReadTemperature();
			DS18B20_StartConvertTemperature();
		}
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
//Работа с DS2782.

#define DS2782_ADDR (0b00110100 << 1)//(0x34 << 1)
#define DS2782_I2C	I2C1

//************************************************************
//Регистры DS2782.
//typedef enum {
//   Parameter_CONTROL         = 0x60, // Control Register
//   Parameter_AB              = 0x61, // Accumulation Bias
//   Parameter_AC              = 0x62, // Aging Capacity (2 bytes)
//   Parameter_VCHG            = 0x64, // Charge Voltage
//   Parameter_IMIN            = 0x65, // Minimum Charge Current
//   Parameter_VAE             = 0x66, // Active Empty Voltage
//   Parameter_IAE             = 0x67, // Active Empty Current
//   Parameter_AE_40           = 0x68, // Active Empty 40
//   Parameter_RSNSP           = 0x69, // Sense Resistor Prime
//   Parameter_Full_40         = 0x6a, // Full 40 (2 bytes)
//   Parameter_Full_Slope      = 0x6c, // Full Slope (4 bytes)
//   Parameter_AE_Slope        = 0x70, // Active Empty Slope (4 bytes)
//   Parameter_SE_Slope        = 0x74, // Standby Empty Slope (4 bytes)
//   Parameter_RSGAIN          = 0x78, // Sense Resistor Gain (2 bytes)
//   Parameter_RSTC            = 0x7a, // Sense Resistor Temperature Coefficient
//   Parameter_FRSGAIN         = 0x7b, // Factory Gain (2 bytes)
//   // Reserved, 0x7d
//   Parameter_Address         = 0x7e, // 2-Wire Slave Address
//   // Reserved, 0x7f
// }DS2782_ParameterRegisters_t;
//---------------------------
typedef enum{
	// Name                              Acc.   Size  Description
	// Reserved, 0x00
	Register_STATUS           = 0x01, // R/W    1     Status
	Register_RAAC             = 0x02, // R      2     Remaining Active Absolute Capacity
	Register_RSAC             = 0x04, // R      2     Remaining Standby Absolute Capacity
	Register_RARC             = 0x06, // R      1     Remaining Active Relative Capacity
	Register_RSRC             = 0x07, // R      1     Remaining Standby Relative Capacity
	Register_IAVG             = 0x08, // R      2     Average Current
	Register_TEMP             = 0x0a, // R      2     Temperature
	Register_VOLT             = 0x0c, // R      2     Voltage
	Register_CURRENT          = 0x0e, // R      2     Current
	Register_ACR              = 0x10, // R/W    2     Accumulated Current (auto-saved)
	Register_ACRL             = 0x12, // R      2     Low Accumulated Current
	Register_AS               = 0x14, // R/W    1     Age Scalar (auto-saved)
	Register_SFR              = 0x15, // R/W    1     Special Feature Register
	Register_FULL             = 0x16, // R      2     Full Capacity
	Register_AE               = 0x18, // R      2     Active Empty Capacity
	Register_SE               = 0x1a, // R      2     Standby Empty Capacity
	// Reserved, 0x1c - 0x1e
	Register_EEPROM           = 0x1f, // R/W    1     EEPROM, Block 0
	Register_User_EEPROM_1    = 0x20, // R/W    16    User EEPROM, Lockable, Block 0
	Register_User_EEPROM_2    = 0x30, // R/W    8     Additional User EEPROM, Lockable, Block 0
	// Reserved, 0x38 - 0x5f
	//---------------------------------------------------
	//Parameter EEPROM Memory Block 1
	// R/W    32    Parameter EEPROM, Lockable, Block 1
	Parameter_CONTROL         = 0x60, // Control Register
	Parameter_AB              = 0x61, // Accumulation Bias
	Parameter_AC              = 0x62, // Aging Capacity (2 bytes)
	Parameter_VCHG            = 0x64, // Charge Voltage
	Parameter_IMIN            = 0x65, // Minimum Charge Current
	Parameter_VAE             = 0x66, // Active Empty Voltage
	Parameter_IAE             = 0x67, // Active Empty Current
	Parameter_AE_40           = 0x68, // Active Empty 40
	Parameter_RSNSP           = 0x69, // Sense Resistor Prime
	Parameter_Full_40         = 0x6a, // Full 40 (2 bytes)
	Parameter_Full_Slope      = 0x6c, // Full Slope (4 bytes)
	Parameter_AE_Slope        = 0x70, // Active Empty Slope (4 bytes)
	Parameter_SE_Slope        = 0x74, // Standby Empty Slope (4 bytes)
	Parameter_RSGAIN          = 0x78, // Sense Resistor Gain (2 bytes)
	Parameter_RSTC            = 0x7a, // Sense Resistor Temperature Coefficient
	Parameter_FRSGAIN         = 0x7b, // Factory Gain (2 bytes)
	// Reserved, 0x7d
	Parameter_Address         = 0x7e, // 2-Wire Slave Address
	// Reserved, 0x7f
	//---------------------------------------------------
	// Reserved, 0x80 - 0xef
	Register_Unique_ID        = 0xf0, // R      8     Unique ID (factory option)
	// Reserved, 0xf8 - 0xfd
	Register_Function_Command = 0xfe, // W      1     Function Command
	// Reserved, 0xff
 }DS2782_Registers_t;

//---------------------------
typedef struct{
	uint16_t hour;
	uint8_t  min;
	uint8_t  sec;
}DS2782_t;

static DS2782_t DS2782Str;
//---------------------------


//************************************************************
uint16_t DS2782_ReadADC(DS2782_Registers_t addrReg, uint8_t len){

	uint8_t rxBuf[2] = {0};
	//-------------------
	//Этот кусок кода работает. Обмен с DS2782 есть.
//	I2C_StartAndSendDeviceAddr(DS2782_I2C, DS2782_ADDR | I2C_MODE_WRITE);
//	I2C_SendData(DS2782_I2C, &regAddr, 1);
//	I2C_Stop(DS2782_I2C);

	//-------------------
	I2C_Read(DS2782_I2C, DS2782_ADDR, (uint8_t)addrReg, rxBuf, len);

	return ((rxBuf[1] << 8) | rxBuf[0]);
}
//************************************************************


//*******************************************************************************************
//*******************************************************************************************
int main(void){

	uint16_t dsRes = 0;
//	uint16_t BmiT  = 0;
	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	//***********************************************
	SysTick_Init();
	__enable_irq();


	I2C_Init(SSD1306_I2C);//I2C_Int_Init(SSD1306_I2C);
	//***********************************************
//	__disable_irq();
	msDelay(500);
	//***********************************************
	//OLED SSD1306
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//DS18B20
	DS18B20_Init(DS18B20_Resolution_12_bit);
	DS18B20_StartConvertTemperature();
	//***********************************************
	//DS2782.

	//***********************************************
	msDelay(500);
	//************************************************************************************
	while(1)
		{
			msDelay(10);
			//***********************************************
			//Мигание светодиодами.
			Led_Blink();
			//DS18B20.
			Temperature_Get(&dsRes);
			//Инкримент счетчика секунд.
			IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));
			//Преобразование времени
			Time_Calculation(secCounter);
			//***********************************************
			//LCD 128x64 - Работает.
			Lcd_String(1, 1);
			Lcd_Print("DS2782 Test");
			//Вывод времени.
			Lcd_String(1, 2);
			Lcd_Print("Time:");
			Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
			Lcd_Chr(':');
			Lcd_BinToDec(Time.min, 2, LCD_CHAR_SIZE_NORM); //минуты
			Lcd_Chr(':');
			Lcd_BinToDec(Time.sec, 2, LCD_CHAR_SIZE_NORM); //секунды
			//Вывод темперетуры DS18B20.
			Lcd_String(1, 3);
			Lcd_Print("DS18B20 =");
			if(DS18B20_GetTemperatureSign() & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
			else                    								Lcd_Chr('+');
			Lcd_BinToDec(dsRes/10, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('.');
			Lcd_BinToDec(dsRes%10, 1, LCD_CHAR_SIZE_NORM);
			Lcd_Print(" C");

			Lcd_String(21, 1);
			if(Blink(INTERVAL_50_mS))Lcd_Chr(':');
			else                     Lcd_Chr(' ');

			Lcd_Update();//вывод сделан для SSD1306
			Lcd_Clear();
			//***********************************************
			//Работа с микросхемой DS2782.

			//Чтение адреса DS2782.
			Lcd_String(1, 5);
			Lcd_Print("DS2782_ADDR:");
			Lcd_Print("0x");
			Lcd_u8ToHex(DS2782_ReadADC(Register_Unique_ID, 1) >> 1);

			//получение напряжения на АКБ.
			uint16_t voltTemp = DS2782_ReadADC(Register_VOLT, 2);
			voltTemp = ( ((voltTemp << 8) & 0xFF00) | ((voltTemp >> 8) & 0x00FF) );

			uint32_t adcTemp = voltTemp >> 5;   //

			adcTemp  &= 0b0000001111111111;//Уберем знак
			adcTemp  *= 488;               //это 4,88mV * 100. Это нужно чтобы избавится от запятой => получили микровольты
			//adcTemp = ((adcTemp + 50) / 100);
			adcTemp  *= 5473;              //это коэф-т деления резистивного делителя, умноженного на 100.
			adcTemp = ((adcTemp + 500000) / 1000000);

			Lcd_String(1, 6);
			Lcd_Print("DS2782_U=");
			Lcd_BinToDec(adcTemp / 100, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr(',');
			Lcd_BinToDec(adcTemp % 100, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('V');

			//получение температуры.
			uint16_t Temp   = DS2782_ReadADC(Register_TEMP, 2);
			         Temp   = ( ((Temp << 8) & 0xFF00) | ((Temp >> 8) & 0x00FF) );
			         Temp >>= 5;

			uint32_t tTemp =  Temp * 125;
					 tTemp = (tTemp + 50) / 100;

			Lcd_String(1, 7);
			Lcd_Print("DS2782_T=");
			Lcd_BinToDec(tTemp/10, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('.');
			Lcd_BinToDec(tTemp%10, 1, LCD_CHAR_SIZE_NORM);
			Lcd_Print(" C");

			//Получения тока потребления от АКБ.
			uint16_t currentTemp = DS2782_ReadADC(Register_CURRENT, 2);
			currentTemp = ( ((currentTemp << 8) & 0xFF00) | ((currentTemp >> 8) & 0x00FF) );
			currentTemp = (currentTemp ^ 0xffff) + 1;	//Уберем знак

			uint32_t currentAdcTemp = currentTemp;

			currentAdcTemp *= 1563; //
			currentAdcTemp = ((currentAdcTemp + 5000) / 10000);

			Lcd_String(1, 8);
			Lcd_Print("DS2782_I=");
			//Lcd_u32ToHex(currentAdcTemp);
			Lcd_BinToDec(currentAdcTemp, 5, LCD_CHAR_SIZE_NORM);
//			Lcd_BinToDec(adcTemp / 100, 2, LCD_CHAR_SIZE_NORM);
//			Lcd_Chr(',');
//			Lcd_BinToDec(adcTemp % 100, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Print("mA");
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

	static uint16_t msCountForDS18B20 = 0;
	//-----------------------------
	//Отсчет таймаута для датчика температуры.
	if(++msCountForDS18B20 >= 1000)
		{
			msCountForDS18B20 = 0;
			FlagsStr.DS18B20  = 1;
		}
	//-----------------------------
	msDelay_Loop();
	Blink_Loop();
	Encoder()->Loop();
}
//*******************************************************************************************
//*******************************************************************************************
