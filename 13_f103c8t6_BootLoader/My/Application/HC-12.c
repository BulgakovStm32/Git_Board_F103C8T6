/*
 * HC-12.c
 *
 *  Created on: 14 окт. 2022 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

#include "HC-12.h"

//*******************************************************************************************
//*******************************************************************************************
const uint32_t HC12_BaudRateArr[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
static char str1[16] = {0};
static char str2[16] = {0};
//*******************************************************************************************
//*******************************************************************************************
static void _hc12_UsartInit(uint32_t baudRate){

	USART_DMA_Init(HC12_USART, baudRate);
}
//**********************************************************
static void _hc12_ClearStringBuff(char *str){

	while(*str != 0)
	{
		*(str++) = 0;
	}
}
//**********************************************************
static void _hc12_SendStringToUsart(char *str){

	Txt_Print(str);
	DMAxChxStartTx(DMA1_Channel7, Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
	DELAY_milliS(50);    //подождем завершения передачи буфера.
}
//**********************************************************
//копирование принятой строки
static void _hc12_CopyRxBuffFromUsart(char *buffDestination){

	RING_BUFF_CopyRxBuff(buffDestination);
}
//**********************************************************
static uint32_t _hc12_GetCurrentBaudRate(void){

	uint32_t count =  0;
	//--------------------
	//Очистка буферов
	_hc12_ClearStringBuff(str1);
	_hc12_ClearStringBuff(str2);
	//Перебираем скорости обмена модуля HC-12
	do{
		if(count > 7) return 0;//Модуль отсутствует
		_hc12_UsartInit(HC12_BaudRateArr[count++]);
		//Проверка связи с модулем. Передаем строку "AT\r" и ждем ответа "OK\r\n".
		_hc12_SendStringToUsart("AT\r");
		_hc12_CopyRxBuffFromUsart(str1);
	}
	while(!PARS_EqualStr(str1, "OK\r\n"));
	return HC12_BaudRateArr[count-1];
}
//*******************************************************************************************
//*******************************************************************************************
uint32_t HC12_Init(HC12_BaudRate_Enum baudRate){

	//Очистка буферов
	_hc12_ClearStringBuff(str1);
	_hc12_ClearStringBuff(str2);
	//Переход в командный режим
	HC12_SET_Low();
	DELAY_milliS(50);//паузу не менее 40 мс.
	//Перебираем скорости обмена модуля HC-12
	if(_hc12_GetCurrentBaudRate() == 0)//Модуль отсутствует
	{
		HC12_SET_High();
		return FALSE;
	}
	//Установка скорости обмена модуля HC-12.
	Txt_BuildStr("AT+B", baudRate, "\r", str1);//Сборка строки-команды типа "AT+B115200\r"
	_hc12_SendStringToUsart(str1);
	_hc12_CopyRxBuffFromUsart(str1);
	Txt_BuildStr("OK+B", baudRate, "\r\n", str2);//Сборка строки-ответа типа "OK+B115200\r\n"
	//Проверяем ответ от модуля
	if(PARS_EqualStr(str1, str2))
	{
		//установка нужной скорости USART
		_hc12_UsartInit(baudRate);
		//Переход в режиме передачи данных.
		HC12_SET_High();
		DELAY_milliS(100); //паузу не менее 80 мс, необходимую для применения новых настроек и переключения режимов.
		return TRUE;
	}
	return FALSE;
}
//**********************************************************
uint32_t HC12_GetBaudRate(void){

	uint32_t temp;
	//--------------------
	//Переход в командный режим
	HC12_SET_Low();
	DELAY_milliS(50);//паузу не менее 40 мс.

	temp = _hc12_GetCurrentBaudRate();

	//Переход в режиме передачи данных.
	HC12_SET_High();
	DELAY_milliS(100); //паузу не менее 80 мс,
	return temp;
}
//*******************************************************************************************
//*******************************************************************************************


