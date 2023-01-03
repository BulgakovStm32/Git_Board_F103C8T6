/*
 *
 *
 *    Created on:
 *        Author:
 *  Редактировал: Беляев А.А. 17.10.2021
 *
 * За основу взят планировщик задач с сайта ChipEnable.ru
 * http://chipenable.ru/index.php/programming-avr/item/110-planirovschik.html
 */

#include "rtos.h"

//*******************************************************************************************
//*******************************************************************************************

volatile static Task_t   taskArray[MAX_TASKS];// очередь задач
volatile static uint32_t taskArrayTail = 0;	  // "хвост" очереди

volatile static uint32_t rtosTickCount = 0;	  //

//*******************************************************************************************
//*******************************************************************************************
//Глобальное запрещение прерываний.
__INLINE static void _disableInterrupt(void){

	__disable_irq();
}
//**********************************************************
//Глобальное разрешение рерываний.
__INLINE static void _enableInterrupt(void){

	__enable_irq();
}
//*******************************************************************************************
//*******************************************************************************************
/* Инициализация РТОС.
 *
 */
void RTOS_Init(void){

//   TCCR0        |= (1<<CS01)|(1<<CS00);         // прескалер - 64
//   TIFR         = (1<<TOV0);                   // очищаем флаг прерывания таймера Т0
//   TIMSK        |= (1<<TOIE0);                  // разрешаем прерывание по переполнению
//   TIMER_COUNTER = TIMER_START;                 // загружаем начальное зн. в счетный регистр

	taskArrayTail = 0;//"хвост" в 0
}
//**********************************************************
/* Добавление задачи в список
 *
 */
void RTOS_SetTask(void(*taskFunc)(void), uint32_t taskDelay, uint32_t taskPeriod){
   
	volatile Task_t *task = &taskArray[0];
	//-----------------------------
	if(!taskFunc) return;
	if(taskDelay != 0) taskDelay -= 1;//Без этого задержка запуска задачи на 1мС больше.
									  //Почему не понятно!!!
	//Поиск задачи в текущем списке
	for(uint32_t i = 0; i < taskArrayTail; i++)
	{
		if(task->pFunc == taskFunc)// если нашли, то обновляем переменные
		{
			_disableInterrupt();//Глобальное запрещение прерываний.
			task->delay  = taskDelay;
			task->period = taskPeriod;
			task->state  = STOP;
			_enableInterrupt();//Глобальное разрешение рерываний.
			return;
		}
		task++;
	}
	//Если такой задачи в списке нет и есть место,то добавляем в конец массива
	if(taskArrayTail < MAX_TASKS)
	{
		_disableInterrupt();//Глобальное запрещение прерываний.
		task 		 = &taskArray[taskArrayTail];
		task->pFunc  = taskFunc;
		task->delay  = taskDelay;
		task->period = taskPeriod;
		task->state  = STOP;
		taskArrayTail++;   //увеличиваем "хвост"
		_enableInterrupt();//Глобальное разрешение рерываний.
	}
}
//**********************************************************
/* Удаление задачи из списка
 *
 */
void RTOS_DeleteTask(void(*taskFunc)(void)){

	//проходим по списку задач
	for(uint32_t i=0; i < taskArrayTail; i++)
	{
		if(taskArray[i].pFunc == taskFunc) // если задача в списке найдена
		{
			_disableInterrupt();//Глобальное запрещение прерываний.
			if(i != (taskArrayTail-1))//переносим последнюю задачу на место удаляемой
			{
				taskArray[i] = taskArray[taskArrayTail-1];
			}
			taskArrayTail--;   //уменьшаем указатель "хвоста"
			_enableInterrupt();//Глобальное разрешение рерываний.
			return;
		}
	}
}
//**********************************************************
/* Диспетчер РТОС, вызывается в main
 *
 */
void RTOS_DispatchLoop(void){

	void(*function)(void);
	volatile Task_t *task = &taskArray[0];
	//-----------------------------
	//проходим по списку задач
	for(uint32_t i=0; i < taskArrayTail; i++)
	{
		//если флаг на выполнение взведен,
		if(task->state == RUN)
		{
			_disableInterrupt();   //Глобальное запрещение прерываний.
			function = task->pFunc;//запоминаем задачу, т.к. во время выполнения может измениться индекс
			if(task->period == 0) RTOS_DeleteTask(task->pFunc);//если период = 0 - удаляем задачу.
			else
			{
				task->state = STOP; //иначе снимаем флаг запуска
				//если задача не изменила задержку задаем ее
				//задача для себя может сделать паузу   ????????? не понятно
				if(task->delay == 0) task->delay = task->period-1;
			}
			_enableInterrupt();//Глобальное разрешение рерываний.
			//выполняем задачу
			(*function)();
		}
		task++;
	}
}
//**********************************************************
/* Таймерная служба РТОС (прерывание аппаратного таймера)
 *
 */
//ISR(RTOS_ISR)
//{
//   u08 i;
//
//   TIMER_COUNTER = TIMER_START;                       // задаем начальное значение таймера
//
//   for (i=0; i<arrayTail; i++)                        // проходим по списку задач
//   {
//      if  (TaskArray[i].delay == 0)                   // если время до выполнения истекло
//           TaskArray[i].run = 1;                      // взводим флаг запуска,
//      else TaskArray[i].delay--;                      // иначе уменьшаем время
//   }
//}

void RTOS_TimerServiceLoop(void){

	volatile Task_t *task = &taskArray[0];
	//-----------------------------
	rtosTickCount++;
	//проходим по списку задач
	for(uint32_t i=0; i < taskArrayTail; i++)
	{
	    if(task->delay == 0) task->state = RUN;//если время до выполнения истекло взводим флаг запуска,
	    else task->delay--;                    //иначе уменьшаем время
	    task++;
	}
}
//**********************************************************
/* Значение счетчика тиков RTOS.
 *
 */
uint32_t RTOS_GetTickCount(void){

	return rtosTickCount;
}
//*******************************************************************************************
//*******************************************************************************************



