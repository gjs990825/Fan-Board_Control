#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"
#include "pid.h"

typedef enum
{
    Status_Run = 1,
    Status_Stop = 0,
} ControlStatus_t;

float Get_AngleAverageError(void);
float Get_AngleVariance(void);
bool Is_Stablilized(void);


void Control_SetTargetAngle(double target);
void Control_SetStatus(ControlStatus_t status);

double Get_CurrentAngle(void);


void TIM3_TimerInit(uint16_t arr, uint16_t psc);

#endif
