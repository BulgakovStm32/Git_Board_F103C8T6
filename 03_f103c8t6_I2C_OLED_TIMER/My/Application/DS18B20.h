/*
 * DS18B20.h
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Zver
 */

#ifndef DS18B20_H_
#define DS18B20_H_
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"

//#include "core_cm3.h"

//*******************************************************************************************
#define DS18B20_SIGN_POSITIVE	0
#define DS18B20_SIGN_NEGATIVE   1

//#define DELAY_T_CONVERT      760000
#define DELAY_RESET_PULSE    500
//#define DELAY_PROTECTION     5

#define DELAY_WRITE_1        5
#define DELAY_WRITE_1_PAUSE  60

#define DELAY_WRITE_0        65
#define DELAY_WRITE_0_PAUSE  5

#define DELAY_READ_SLOT      1
#define DELAY_BUS_RELAX      15
#define DELAY_READ_PAUSE     55//45

//**********************************
typedef enum {
  SKIP_ROM         = 0xCC,
  CONVERT_T        = 0x44,
  READ_SCRATCHPAD  = 0xBE,
  WRITE_SCRATCHPAD = 0x4E,
  TH_REGISTER      = 0x4B,
  TL_REGISTER      = 0x46,
}DS18B20_Cmd_Enum;
//**********************************
typedef enum {
  DS18B20_Resolution_9_bit  = 0x1F,
  DS18B20_Resolution_10_bit = 0x3F,
  DS18B20_Resolution_11_bit = 0x5F,
  DS18B20_Resolution_12_bit = 0x7F
}DS18B20_Resolution_Enum;
//**********************************
typedef struct{
	GPIO_TypeDef		   *GPIO_PORT;
	uint32_t	  			GPIO_PIN;
	uint32_t				SENSOR_NUMBER;
	DS18B20_Resolution_Enum RESOLUTION;
	uint32_t				TEMPERATURE_SIGN;
	uint32_t				TEMPERATURE;
}DS18B20_t;
//*******************************************************************************************
//*******************************************************************************************
//void 	 TemperatureSens_Init(void);
void 	 TemperatureSens_GpioInit(DS18B20_t *sensor);
void 	 TemperatureSens_SetResolution(DS18B20_t *sensor);
void 	 TemperatureSens_StartConvertTemperature(DS18B20_t *sensor);
void     TemperatureSens_ReadTemperature(DS18B20_t *sensor);
uint32_t TemperatureSens_Sign(DS18B20_t *sensor);
uint32_t TemperatureSens_Temperature(DS18B20_t *sensor);



//*******************************************************************************************
//*******************************************************************************************
#endif /* DS18B20_H_ */
