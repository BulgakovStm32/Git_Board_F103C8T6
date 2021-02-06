
#ifndef _Scheduler_H
#define _Scheduler_H
//-----------------------------------------------------------------------------

//#include "sys_tick_ST.h"
#include "main.h"

//-----------------------------------------------------------------------------
//Очередь задач.
#define TaskQueueSize 20                           //Макс. число задач в очереди задач.

typedef   void(*TaskPTR)(void);                    //Указатель-пустышка.
volatile static TaskPTR TaskQueue[TaskQueueSize+1];//Очередь указателей
//--------------------
//Очередь таймеров.
#define MainTimerQueueSize TaskQueueSize //

volatile static struct{
	//---------
	TaskPTR  Task;//Указатель перехода
	uint16_t Time;//Выдержка в мс
	//---------
}TaskWithTime[MainTimerQueueSize+1];// Очередь таймеров
//-----------------------------------------------------------------------------
void Scheduler_Init 	   (void);
void Scheduler_Start	   (void);
void Scheduler			   (void);
void Scheduler_TimerService(void);

uint32_t Scheduler_GetTickCount(void);
void 	 Scheduler_SetTask     (TaskPTR task);
void     Scheduler_SetTimerTask(TaskPTR task, uint16_t newTime);
//--------------------------------
// Прототипы задач 
void IdleTask(void);
//-----------------------------------------------------------------------------
#endif /*_Scheduler_H*/


