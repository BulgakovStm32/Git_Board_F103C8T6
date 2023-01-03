/*
 * String.c
 *
 * Created on: 18 октября 2021 г.
 *     Author: Беляев А.А.
 */

#include "MyString.h"
//*******************************************************************************************
//*******************************************************************************************

static TxtBuf_t	TxtBuf;

//*******************************************************************************************
//*******************************************************************************************
TxtBuf_t* Txt_Buf(void){

	return &TxtBuf;
}
//**********************************************************
void Txt_Chr(uint8_t ch){

  if(TxtBuf.bufIndex < TXT_BUF_SIZE) TxtBuf.buf[TxtBuf.bufIndex++] = ch;
}
//**********************************************************
uint8_t Txt_Print(char *txt){

  uint8_t i = 0;
  //--------------------
  while(*txt)
    {
	  Txt_Chr(*txt++);
      i++;
    }
  //Txt_Chr('\n');
  i++;
  return i;
}
//**********************************************************
uint8_t Txt_BinToDec(uint32_t var, uint32_t num){

	uint8_t decArray[10];
	//--------------------
	decArray[9] = (uint8_t)(var/1000000000);
	var %= 1000000000;

	decArray[8] = (uint8_t)(var/100000000);
	var %= 100000000;

	decArray[7] = (uint8_t)(var/10000000);
	var %= 10000000;

	decArray[6] = (uint8_t)(var/1000000);
	var %= 1000000;

	decArray[5] = (uint8_t)(var/100000);
	var %= 100000;

	decArray[4] = (uint8_t)(var/10000);
	var %= 10000;

	decArray[3] = (uint8_t)(var/1000);
	var %= 1000;

	decArray[2] = (uint8_t)(var/100);
	var %= 100;

	decArray[1] = (uint8_t)(var/10);
	decArray[0] = (uint8_t)(var%10);
	//--------------------
	//Вывод на дисплей
	for(uint8_t i=0; i<num; i++)
	{
		Txt_Chr(decArray[(num-1)-i]+'0');
	}
	return num+1;
}
//**********************************************************
void Txt_u8ToHex(uint8_t var){

	uint8_t ch;
	//--------------------
	ch = (var >> 4) & 0x0F;
	if(ch <= 9) ch += '0';
	else		ch += ('A' - 10);
	Txt_Chr(ch);

	ch = var & 0x0F;
	if(ch <= 9) ch += '0';
	else		ch += ('A' - 10);
	Txt_Chr(ch);
}
//**********************************************************
void Txt_u32ToHex(uint32_t var){

	Txt_Print("0x");
	Txt_u8ToHex((uint8_t)((var & 0xFF000000) >> 24));
	Txt_u8ToHex((uint8_t)((var & 0x00FF0000) >> 16));
	Txt_u8ToHex((uint8_t)((var & 0x0000FF00) >> 8));
	Txt_u8ToHex((uint8_t)( var & 0x000000FF));
}
//**********************************************************
void Txt_BuildStr(char *head, uint32_t num, char *tail, char *buffDestination){

	uint32_t i = 0;
	uint8_t  decArray[10];
	//--------------------
	//Складываем заголовок в буфер.
	while(*head) buffDestination[i++] = *head++;

	//Преобразование числа в строку.
	decArray[9] = (uint8_t)(num/1000000000);
	num %= 1000000000;

	decArray[8] = (uint8_t)(num/100000000);
	num %= 100000000;

	decArray[7] = (uint8_t)(num/10000000);
	num %= 10000000;

	decArray[6] = (uint8_t)(num/1000000);
	num %= 1000000;

	decArray[5] = (uint8_t)(num/100000);
	num %= 100000;

	decArray[4] = (uint8_t)(num/10000);
	num %= 10000;

	decArray[3] = (uint8_t)(num/1000);
	num %= 1000;

	decArray[2] = (uint8_t)(num/100);
	num %= 100;

	decArray[1] = (uint8_t)(num/10);
	decArray[0] = (uint8_t)(num%10);
	//--------------------

	uint32_t digitCount = 9;
	//Находим первое ненулевое число слева.
	while(decArray[digitCount] == 0)
	{
		if(digitCount == 0) return;
		digitCount--;
	}
	//Складываем значещие символы в буфер
	while(digitCount)
	{
		buffDestination[i++] = decArray[digitCount--]+'0';
	}
	buffDestination[i++] = decArray[0]+'0';
	//Складываем заврешающую последовательность в буфер.
	while(*tail) buffDestination[i++] = *tail++;
}
//**********************************************************

//*******************************************************************************************
//*******************************************************************************************
