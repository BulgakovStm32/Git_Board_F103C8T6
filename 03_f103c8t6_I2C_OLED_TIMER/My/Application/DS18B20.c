/*
 * ds18b20.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */
#include "DS18B20.h"
//*******************************************************************************************
//*******************************************************************************************

//static uint32_t DELAY_WAIT_CONVERT = DELAY_T_CONVERT;

//*******************************************************************************************
//*******************************************************************************************
//Функция задержки в микросекундах.
static void OneWire_usDelay(uint32_t us){

	microDelay(us);

//	us *= 4;  //Эти цифры подобраны эмпирическим путем для Fclk=72MHz.
//	us += 4;
//	while(us--);
}
//**********************************************************
static void OneWire_GpioInit(DS18B20_t *sensor){

	uint32_t pin = sensor->GPIO_PIN;
	//---------------------
	//Включение тактирования портов.
		 if(sensor->GPIO_PORT == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	else if(sensor->GPIO_PORT == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	else if(sensor->GPIO_PORT == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	else return;
	//Конфигурация выводы в режим 50MHz output open-drain.
	if(sensor->GPIO_PIN <= 7)
	{
		pin = pin * 4;
		sensor->GPIO_PORT->CRL |=  (0x03 << pin);          //GPIO_CRL_MODEx;
		sensor->GPIO_PORT->CRL |=  (0x01 << (0x02 << pin));//GPIO_CRL_CNFx_0;
		sensor->GPIO_PORT->CRL &= ~(0x02 << (0x02 << pin));//GPIO_CRL_CNFx_1;
	}
	else
	{
		pin = (pin - 8) * 4;
		sensor->GPIO_PORT->CRH |=  (0x03 << pin);          //GPIO_CRL_MODEx;
		sensor->GPIO_PORT->CRH |=  (0x01 << (0x02 << pin));//GPIO_CRL_CNFx_0;
		sensor->GPIO_PORT->CRH &= ~(0x02 << (0x02 << pin));//GPIO_CRL_CNFx_1;
	}
}
//**********************************************************
static uint8_t OneWire_Reset(DS18B20_t *sensor){

  uint16_t status = 0;
  uint32_t pin    = ( 1 << sensor->GPIO_PIN);
  //---------------------
  //низкий уровень
  sensor->GPIO_PORT->ODR &= ~pin;
  OneWire_usDelay(480);//задержка как минимум на 480 микросекунд
  //высокий уровень
  sensor->GPIO_PORT->ODR |= pin;
  OneWire_usDelay(60); //задержка как минимум на 60 микросекунд
  //проверяем уровень
  status = (sensor->GPIO_PORT->IDR & pin);
  OneWire_usDelay(DELAY_RESET_PULSE / 2);//на всякий случай подождём, так как могут быть неточности в задержке.
  return status;
}
//**********************************************************
static void OneWire_WriteBit(DS18B20_t *sensor, uint8_t bit){

	uint32_t pin = (1 << sensor->GPIO_PIN);
	//---------------------
	//низкий уровень
	sensor->GPIO_PORT->ODR &= ~pin;
	if(bit) OneWire_usDelay(DELAY_WRITE_1);
	else    OneWire_usDelay(DELAY_WRITE_0);

	//высокий уровень
	sensor->GPIO_PORT->ODR |= pin;
	if(bit) OneWire_usDelay(DELAY_WRITE_1_PAUSE);
	else    OneWire_usDelay(DELAY_WRITE_0_PAUSE);
}
//**********************************************************
static void OneWire_WriteByte(DS18B20_t *sensor, uint8_t data){

	for(uint8_t i = 0; i < 8; i++)
	{
		OneWire_WriteBit(sensor, ((data >> i) & 0x01));
	}
}
//**********************************************************
static uint8_t OneWire_ReadBit(DS18B20_t *sensor){

	uint8_t  bit = 0;
	uint32_t pin = (1 << sensor->GPIO_PIN);
	//---------------------
	//шину к земле.
	sensor->GPIO_PORT->ODR &= ~pin;
	OneWire_usDelay(DELAY_READ_SLOT);
	//отпустили шину.
	sensor->GPIO_PORT->ODR |= pin;
	OneWire_usDelay(DELAY_BUS_RELAX);
	//Чтение состояния линии
	if(sensor->GPIO_PORT->IDR & pin) bit = 1;
	OneWire_usDelay(DELAY_READ_PAUSE);

	return bit;
}
//*******************************************************************************************
//*******************************************************************************************
//static uint8_t DS18B20_GetDevider(DS18B20_t *sensor){
//
//	switch (sensor->RESOLUTION){
//		case DS18B20_Resolution_9_bit:  return 8;
//		case DS18B20_Resolution_10_bit: return 4;
//		case DS18B20_Resolution_11_bit: return 2;
//		case DS18B20_Resolution_12_bit:
//		default: 						return 1;
//	}
//}
//**********************************************************
static void DS18B20_ReadTemperature(DS18B20_t *sensor){

	uint32_t data = 0;
	//---------------------
	__disable_irq();

	for(uint8_t i = 0; i < 16; i++)
	{
		data |= (uint32_t)(OneWire_ReadBit(sensor) << i);
	}
	//Отрицательная температура.
	if(data & 0x0000F800)
	{
		sensor->TEMPERATURE_SIGN = DS18B20_SIGN_NEGATIVE;

		data  = (data ^ 0x0000FFFF) + 1;
		data &= 0x00000FFF;//Маска для выделения 12 бит.

//			data  = ~data;
//			data += 1;
//			data &= 0x00000FFF;
	}
	else sensor->TEMPERATURE_SIGN = DS18B20_SIGN_POSITIVE;

	__enable_irq();
	//Расчет температуры
	sensor->TEMPERATURE = (uint32_t)(((data * 625) + 500) / 1000);
}
//*******************************************************************************************
//*******************************************************************************************
void TemperatureSens_GpioInit(DS18B20_t *sensor){

	OneWire_GpioInit(sensor);
}
//**********************************************************
void TemperatureSens_SetResolution(DS18B20_t *sensor){

	OneWire_Reset(sensor);//reset();
	OneWire_WriteByte(sensor, SKIP_ROM);
	OneWire_WriteByte(sensor, WRITE_SCRATCHPAD);
	OneWire_WriteByte(sensor, TH_REGISTER);
	OneWire_WriteByte(sensor, TL_REGISTER);
	OneWire_WriteByte(sensor, sensor->RESOLUTION);
	//DELAY_WAIT_CONVERT = DELAY_T_CONVERT / DS18B20_GetDevider(sensor);
}
//**********************************************************
void TemperatureSens_StartConvertTemperature(DS18B20_t *sensor){

	if(OneWire_Reset(sensor)) return;
	OneWire_WriteByte(sensor, SKIP_ROM);
	OneWire_WriteByte(sensor, CONVERT_T);
}
//**********************************************************
void TemperatureSens_ReadTemperature(DS18B20_t *sensor){

	if(OneWire_Reset(sensor))
	{
		sensor->TEMPERATURE = 0;
		return;
	}
	OneWire_WriteByte(sensor, SKIP_ROM);
	OneWire_WriteByte(sensor, READ_SCRATCHPAD);
	DS18B20_ReadTemperature(sensor);

	TemperatureSens_StartConvertTemperature(sensor);
}
//**********************************************************
uint32_t TemperatureSens_Sign(DS18B20_t *sensor){

	return sensor->TEMPERATURE_SIGN;
}
//**********************************************************
uint32_t TemperatureSens_Temperature(DS18B20_t *sensor){

	return sensor->TEMPERATURE;
}
//*******************************************************************************************
//*******************************************************************************************
