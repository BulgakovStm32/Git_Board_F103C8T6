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

#define MAX_TASKS      9               // Количество задач
#define RTOS_ISR       TIMER0_OVF_vect // Вектор прерывания
#define TIMER_COUNTER  TCNT0           // Регистр-счетчик
#define TIMER_START    130             // Константа для таймера Т0


/******************************************************************************************
 * Структура задачи
 */
typedef struct{
   void(*pFunc)(void);	// указатель на функцию
   uint16_t Delay;		// задержка перед первым запуском задачи
   uint16_t Period;		// период запуска задачи
   uint32_t Run;		// флаг готовности задачи к запуску
}Task_t;

/******************************************************************************************
 * Прототипы фукнций
 */
void 	 RTOS_Init(void);
void     RTOS_SetTask(void(*taskFunc)(void), uint16_t taskDelay, uint16_t taskPeriod);
void     RTOS_DeleteTask(void(*taskFunc)(void));
void     RTOS_DispatchLoop(void);
void     RTOS_TimerServiceLoop(void);
uint32_t RTOS_GetTickCount(void);

#endif
