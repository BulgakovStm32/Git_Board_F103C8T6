/*
 * PID.h
 *
 *  Created on: 21 февр. 2022 г.
 *      Author: belyaev
 */
#ifndef PID_H_
#define PID_H_
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************

#define SCALING_FACTOR  128

// Maximum value of variables
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      (MAX_LONG / 2)

/*! \brief PID Status
 *
 * Setpoints and data used by the PID control algorithm
 */
typedef struct{
  int16_t lastProcessValue;	//! Last process value, used to find derivative of process value.
  int16_t lastError;		//
  int32_t sumError;			//! Summation of errors, used for integrate calculations
  int16_t P_Factor;			//! The Proportional tuning constant, multiplied with SCALING_FACTOR
  int16_t I_Factor;			//! The Integral tuning constant, multiplied with SCALING_FACTOR
  int16_t D_Factor;			//! The Derivative tuning constant, multiplied with SCALING_FACTOR
  int16_t maxError;			//! Maximum allowed error, avoid overflow
  int32_t maxSumError;		//! Maximum allowed sumerror, avoid overflow
}PID_Data_t;

//*******************************************************************************************
//*******************************************************************************************
void 	PID_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, PID_Data_t *pid);
int16_t PID_Controller(int16_t setPoint, int16_t processValue, PID_Data_t *pid_st);
void 	PID_Reset_Integrator(PID_Data_t *pid_st);

//*******************************************************************************************
//*******************************************************************************************
#endif
