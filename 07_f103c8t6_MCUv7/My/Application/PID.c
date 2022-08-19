/*
 * PID.c
 *
 *  Created on: 21 февр. 2022 г.
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************

#include "PID.h"

//*******************************************************************************************
//*******************************************************************************************
/*! \brief Initialisation of PID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param p_factor  Proportional term.
 *  \param i_factor  Integral term.
 *  \param d_factor  Derivate term.
 *  \param pid  	 Struct with PID status.
 */
void PID_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, PID_t *pid){

	// Start values for PID controller
	pid->sumError 		  = 0;
	pid->lastProcessValue = 0;

	// Tuning constants for PID loop
	pid->P_Factor = p_factor;
	pid->I_Factor = i_factor;
	pid->D_Factor = d_factor;

	// Limits to avoid overflow
	pid->maxError 	 = MAX_INT 	  / (pid->P_Factor + 1);
	pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}
//************************************************************
/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid_st  PID status struct.
 */
int32_t PID_Controller(int16_t setPoint, int16_t processValue, PID_t *pid){

	int16_t Kp = pid->P_Factor;
	int16_t Ki = pid->I_Factor;
	int16_t Kd = pid->D_Factor;

	int16_t error, P, D;
	int32_t I, ret, temp;
	//-------------------
	//Calculate error
	error = setPoint - processValue;

	//Calculate Pterm and limit error overflow
		 if(error >  pid->maxError) P =  MAX_INT;
	else if(error < -pid->maxError) P = -MAX_INT;
	else 							P = Kp * error;

	//Calculate Iterm and limit integral runaway
	temp = pid->sumError + error;//Накопелние ошибки

	if(temp > pid->maxSumError)
	{
		I = MAX_I_TERM;
		pid->sumError = pid->maxSumError;
	}
	else if(temp < -pid->maxSumError)
	{
		I = -MAX_I_TERM;
		pid->sumError = -pid->maxSumError;
	}
	else
	{
		pid->sumError = temp;
		I = Ki * pid->sumError;
	}

	//Calculate Dterm
//	D = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);
//	pid_st->lastProcessValue = processValue;

	D = Kd * (error - pid->lastError);
	pid->lastError = error;

	//Calculate PID
	ret = (P + I + D) / SCALING_INT16_FACTOR;
		 if(ret >  MAX_INT) ret =  MAX_INT;
	else if(ret < -MAX_INT) ret = -MAX_INT;

	return((int16_t)ret);
}
//************************************************************
/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void PID_ResetIntegrator(PID_t *pid){

	pid->sumError = 0;
}
//*******************************************************************************************
//*******************************************************************************************

