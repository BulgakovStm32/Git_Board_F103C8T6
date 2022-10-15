#ifndef RTOS_H
#define RTOS_H

/******************************************************************************************
 * За основу взят планировщик задач с сайта ChipEnable.ru                                 *
 * http://chipenable.ru/index.php/programming-avr/item/110-planirovschik.html             *
 *                                                                                        *
 * Доработал Шибанов Владимир aka KontAr                                                  *
 * Дата: 26.03.2014                                                                       *
 *                                                                                        *
 * Изменения:                                                                             *
 * - добавлен однократный вызов задачи                                                    *
 * - добавлено удаление задачи по имени                                                   *
 * - при повторном добавлении задачи обновляются ее переменные                            *
 * - добавлен указатель на "хвост" списка                                                 *
 * - функции РТОС скорректированы с учетом "хвоста"                                       *
 ******************************************************************************************/

#include "main.h"

//*******************************************************************************************

#define MAX_TASKS      10              // Количество задач

//********************************************************
// Структура задачи
typedef struct{
   void(*pFunc)(void);	// указатель на функцию
   uint32_t Delay;		// задержка перед первым запуском задачи
   uint32_t Period;		// период запуска задачи
   uint32_t Run;		// флаг готовности задачи к запуску
}Task_t;
//*******************************************************************************************
//*******************************************************************************************
// Прототипы фукнций
void 	 RTOS_Init(void);
void     RTOS_SetTask(void(*taskFunc)(void), uint32_t taskDelay, uint32_t taskPeriod);
void     RTOS_DeleteTask(void(*taskFunc)(void));
void     RTOS_DispatchLoop(void);
void     RTOS_TimerServiceLoop(void);
uint32_t RTOS_GetTickCount(void);
//*******************************************************************************************
//*******************************************************************************************
#endif
