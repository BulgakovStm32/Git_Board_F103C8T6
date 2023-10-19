/*
 * String.h
 *
 * Created on: 18 октября 2021 г.
 *     Author: Беляев А.А.
 */

#ifndef STRING_H_
#define STRING_H_
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"

//#include "core_cm3.h"

//*******************************************************************************************
#define TXT_BUF_SIZE	256

//**********************************
typedef struct{
	uint8_t  buf[TXT_BUF_SIZE];
	uint16_t bufIndex;
}TxtBuf_t;
//**********************************
//typedef enum {
//  SKIP_ROM         = 0xCC,
//  CONVERT_T        = 0x44,
//  READ_SCRATCHPAD  = 0xBE,
//  WRITE_SCRATCHPAD = 0x4E,
//  TH_REGISTER      = 0x4B,
//  TL_REGISTER      = 0x46,
//}DS18B20_Cmd_Enum;
//*******************************************************************************************
//*******************************************************************************************
TxtBuf_t* Txt_Buf(void);
void 	  Txt_Chr(uint8_t ch);
uint8_t   Txt_Print(char *txt);
uint8_t   Txt_BinToDec(uint32_t var, uint32_t num);
void      Txt_u8ToHex(uint8_t var);
void      Txt_u32ToHex(uint32_t var);

void Txt_BuildStr(char *head, uint32_t num, char *tail, char *buffDestination);

//*******************************************************************************************
//*******************************************************************************************
#endif /* DS18B20_H_ */