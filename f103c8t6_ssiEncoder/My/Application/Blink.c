
#include "Blink.h"

//-----------------------------------------------------------------------------
static volatile uint16_t BlinkReg = 0;
//-----------------------------------------------------------------------------
//Процесс для мигания светодиодами.
void Blink_Loop(void){
	
	static uint16_t blinkCount = 0;
  //--------------------
	++blinkCount;
	BlinkReg ^= (1 << INTERVAL_1_mS);
	if((blinkCount % 2)    == 0) BlinkReg ^= (1 << INTERVAL_2_mS);
	if((blinkCount % 5)    == 0) BlinkReg ^= (1 << INTERVAL_5_mS);
	if((blinkCount % 10)   == 0) BlinkReg ^= (1 << INTERVAL_10_mS);
	if((blinkCount % 50)   == 0) BlinkReg ^= (1 << INTERVAL_50_mS);
	if((blinkCount % 100)  == 0) BlinkReg ^= (1 << INTERVAL_100_mS);
	if((blinkCount % 250)  == 0) BlinkReg ^= (1 << INTERVAL_250_mS);	
	if((blinkCount % 500)  == 0) BlinkReg ^= (1 << INTERVAL_500_mS);
	if((blinkCount % 1000) == 0)
		{
			BlinkReg  ^= (1 << INTERVAL_1000_mS);
			blinkCount = 0;
		}
}
//-----------------------------------------------------------------------------
uint16_t Blink(BlinkIntervalEnum_t interval){
	
	if(BlinkReg & (1 << interval)) return 0xFFFF;
	else											     return 0;
}
//-----------------------------------------------------------------------------
