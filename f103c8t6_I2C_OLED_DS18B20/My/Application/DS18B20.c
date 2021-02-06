/*
 * ds18b20.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */
#include "DS18B20.h"
//*******************************************************************************************
//*******************************************************************************************

uint32_t DELAY_WAIT_CONVERT = DELAY_T_CONVERT;
uint8_t  TemperatureSign	= 0;

//*******************************************************************************************
//*******************************************************************************************
//Функция задержки в микросекундах.
__STATIC_INLINE void usDelay(__IO uint32_t us){

	us *= 4;  //Эти цифры подобраны эмпирическим путем для Fclk=72MHz.
	us += 4;
	while(us--);
}
//**********************************************************
static uint8_t OneWire_Reset(void){

  uint16_t status = 0;
  //---------------------
  //низкий уровень
  DS18B20_PIN_LOW();		 //низкий уровень
  usDelay(DELAY_RESET_PULSE);//задержка как минимум на 480 микросекунд
  //высокий уровень
  DS18B20_PIN_HIGH();		 //высокий уровень
  usDelay(DELAY_READ_PAUSE); //задержка как минимум на 60 микросекунд
  //проверяем уровень
  status = DS18B20_PIN_READ();   //проверяем уровень
  usDelay(DELAY_RESET_PULSE / 2);//на всякий случай подождём, так как могут быть неточности в задержке.
  return status;
}
//**********************************************************
static void OneWire_WriteBit(uint8_t bit){

	//низкий уровень
	DS18B20_PIN_LOW();
	if(bit) usDelay(DELAY_WRITE_1);
	else    usDelay(DELAY_WRITE_0);
	//высокий уровень
	DS18B20_PIN_HIGH();
	if(bit) usDelay(DELAY_WRITE_1_PAUSE);
	else    usDelay(DELAY_WRITE_0_PAUSE);
}
//**********************************************************
static void OneWire_WriteByte(uint8_t data){

	for(uint8_t i = 0; i < 8; i++)
		{
			OneWire_WriteBit((data >> i) & 0x01);
			//usDelay(DELAY_PROTECTION);
		}
}
//**********************************************************
static uint8_t OneWire_ReadBit(void){

	uint8_t bit = 0;
	//---------------------
	//шину к земле.
	DS18B20_PIN_LOW();
	usDelay(DELAY_READ_SLOT);
	//отпустили шину.
	DS18B20_PIN_HIGH();
	usDelay(DELAY_BUS_RELAX);
	//Чтение состояния линии
	if(DS18B20_PIN_READ()) bit = 1;
	usDelay(DELAY_READ_PAUSE);

	return bit;
}
//**********************************************************
static uint8_t ds18b20_getDevider(DS18B20_Resolution_Enum resolution){

	switch (resolution){
		case DS18B20_Resolution_9_bit:  return 8;
		case DS18B20_Resolution_10_bit: return 4;
		case DS18B20_Resolution_11_bit: return 2;
		case DS18B20_Resolution_12_bit:
		default: 						return 1;
	}
}
//**********************************************************
static uint16_t readTemperature(void){

	uint32_t data = 0;
	//---------------------
	for(uint8_t i = 0; i < 16; i++)
		{
			data |= (uint32_t)(OneWire_ReadBit() << i);
		}
	//Отрицательная температура.
	if(data & 0x0000F800)
		{
			TemperatureSign = DS18B20_SIGN_NEGATIVE;
			data = (data ^ 0xffff) + 1;
			data &= 0x0000FFFF;
		}
	else TemperatureSign = DS18B20_SIGN_POSITIVE;
	//Расчет температуры
	return (uint16_t)(((data * 625) + 500) / 1000);
}
//*******************************************************************************************
//*******************************************************************************************
void DS18B20_Init(DS18B20_Resolution_Enum resolution){

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	//PB1 - 50MHz output open-drain
	DS18B20_GPIO->CRL |=  GPIO_CRL_MODE3;
	DS18B20_GPIO->CRL |=  GPIO_CRL_CNF3_0;
	DS18B20_GPIO->CRL &= ~GPIO_CRL_CNF3_1;

	DS18B20_SetResolution(resolution);
}
//**********************************************************
void DS18B20_SetResolution(DS18B20_Resolution_Enum resolution){

	OneWire_Reset();//reset();
	OneWire_WriteByte(SKIP_ROM);
	OneWire_WriteByte(WRITE_SCRATCHPAD);
	OneWire_WriteByte(TH_REGISTER);
	OneWire_WriteByte(TL_REGISTER);
	OneWire_WriteByte(resolution);
	DELAY_WAIT_CONVERT = DELAY_T_CONVERT / ds18b20_getDevider(resolution);
}
//**********************************************************
void DS18B20_StartConvertTemperature(void){

	if(OneWire_Reset()) return;
	OneWire_WriteByte(SKIP_ROM);
	OneWire_WriteByte(CONVERT_T);
}
//**********************************************************
uint16_t DS18B20_ReadTemperature(void){

	if(OneWire_Reset()) return 0;//99;
	OneWire_WriteByte(SKIP_ROM);
	OneWire_WriteByte(READ_SCRATCHPAD);

	return readTemperature();
}
//**********************************************************
uint8_t DS18B20_GetTemperatureSign(void){

	return TemperatureSign;
}
//**********************************************************
//uint16_t DS18B20_GetTemperature(void){
//
//	if(ds18b20_Reset()) return 0;
//	writeByte(SKIP_ROM);
//	writeByte(CONVERT_T);
//
//	usDelay(DELAY_WAIT_CONVERT);
//
//	if(ds18b20_Reset()) return 0;
//	writeByte(SKIP_ROM);
//	writeByte(READ_SCRATCHPAD);
//
//	return readTemperature();
//}
//*******************************************************************************************
//*******************************************************************************************
