/*
 * DHT11.c
 *
 *  Created on: 10 мар. 2023 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

#include "DHT22.h"

//*******************************************************************************************
//*******************************************************************************************
static Dht22_t dht22;
static uint8_t buf[5] = {0};

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Функция задержки в микросекундах.
__STATIC_INLINE void _singleWire_usDelay(uint32_t us){

	DELAY_microS(us);

//	us *= 4;  //Эти цифры подобраны эмпирическим путем для Fclk=72MHz.
//	us += 4;
//	while(us--);
}
//**********************************************************
//Процедура инициализации: импульсы сброса и присутствия
static Dht22_State_t _singleWire_SendStartSignal(GPIO_TypeDef *port, uint32_t pin){

	uint32_t presence = 0;
	pin = (1 << pin);
	//---------------------
	if(!port) return DHT22_STATE_RESET;	//Проверка. Не определен порт - значит нет датчика.
										//Выходим с признаком отсутствия устройства на шине.
	//---------------------
	//Формирование Start_Signal
	port->ODR &= ~pin;			//низкий уровень

	//Блокирующая задержка > 18мс
	//_singleWire_usDelay(18100);

	//Неблокирующая задержка > 18мс
	static uint8_t flag = 0;
	flag ^= 1;
	if(flag == 1)return DHT22_STATE_WAITING_PRESENCE; //возвращаем состояние ожидания

	port->ODR |= pin;	  		//отпускаем линию.
	_singleWire_usDelay(30);	//задержка 20-40мк
	//---------------------
	//Теперь DHT22 потянет линию LOW на 80 мкс, а затем HIGH на 80 мкс.
	_singleWire_usDelay(40);	//чтение состояния в середине LOW
	if((port->IDR & pin) == 0) presence += 1; //фиксируем уровень

	_singleWire_usDelay(80);	//чтение состояния в середине HIGH
	if(port->IDR & pin) 	   presence += 1; //фиксируем уровень

	_singleWire_usDelay(40);	//Пауза для окончения ответа от DHT11
	//---------------------
	if(presence == 2) return DHT22_STATE_PRESENCE_OK;
					  return DHT22_STATE_PRESENCE_ERR;
}
//**********************************************************
//Чтение байта.
static uint8_t _singleWire_ReadByte(GPIO_TypeDef *port, uint32_t pin){

	pin = (1 << pin);
	uint8_t  byte  = 0;
	uint32_t count = 0;
	//---------------------
	for(uint32_t i=0; i < 8; i++)
	{
		//---------------------
		//Ждем уровень HIGH
		count = DELAY_microSecCount();
		while((port->IDR & pin) == 0)
		{
			if((DELAY_microSecCount() - count) > 100) break; //Долго нет уровня LOW
		}
		//Ждем...
		_singleWire_usDelay(35);

		//Сохраняем заначение бита
		if(port->IDR & pin) byte |= (1 << (7-i)); //фиксируем уровень

		//Ждем окончание уровня HIGH
		count = DELAY_microSecCount();
		while(port->IDR & pin)
		{
			if((DELAY_microSecCount() - count) > 100) break;//Уровень HIGH слишком долго
		}
		//---------------------
	}
	return byte;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void DHT22_Init(void){

	dht22.port = DHT22_GPIO_PORT; 	//GPIOB;
	dht22.pin  = DHT22_GPIO_PIN; 	//12;
	GPIO_InitForOutputOpenDrain(dht22.port, dht22.pin);
	//dht22.state = DHT22_STATE_INIT;
	dht22.state = DHT22_STATE_WAITING_MEAS;
}
//**********************************************************
void DHT22_ReadData(void){

	//Неблокирующая проверка присутствия датчика на шине
	Dht22_State_t state = _singleWire_SendStartSignal(dht22.port, dht22.pin);

	//Ожидание паузы 18 мс
	if(state == DHT22_STATE_WAITING_PRESENCE)
	{
		dht22.state = DHT22_STATE_WAITING_PRESENCE;
		return;
	}
	//нет датчика на шине
	if(state != DHT22_STATE_PRESENCE_OK)
	{
		//Нет ответа от DHT11
		dht22.state = DHT22_STATE_PRESENCE_ERR;
		return;
	}
	//---------------------
	//Прочитаем данные из датчика (40 битов = 5 байтов)
	//Data format:
	//8bit integral RH data + 8bit decimal RH data +
	//8bit integral T data  + 8bit decimal T data  +
	//8bit check sum.
	for(uint32_t i=0; i < 5; i++)
	{
		buf[i] = _singleWire_ReadByte(dht22.port, dht22.pin);
	}
	//---------------------
	//Проверми контрольную сумму
	//If the data transmission is right, the check-sum should be the last 8bit of
	//"8bit integral RH data + 8bit decimal RH data +
	// 8bit integral T data  + 8bit decimal T data"
	uint8_t checkSum = buf[0] + buf[1] + buf[2] + buf[3];

	if(checkSum != buf[4])
	{
		//Ошибка контрольной суммы.
		dht22.state = DHT22_STATE_CHECKSUM_ERR;
		return;
	}
	//Контрольная сумма ОК.
	dht22.humidity 	  = (uint16_t)((buf[0] << 8) | (buf[1] << 0));
	dht22.temperature = (uint16_t)((buf[2] << 8) | (buf[3] << 0));
	dht22.state = DHT22_STATE_OK;
}
//**********************************************************
uint16_t DHT22_Temperature(void){

	return dht22.temperature;
}
//**********************************************************
uint16_t DHT22_Humidity(void){

	return dht22.humidity;
}
//**********************************************************
Dht22_State_t DHT22_State(void){

	return dht22.state;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************











