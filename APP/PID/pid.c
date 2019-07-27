#include "pid.h"
#include "my_lib.h"

float Kp = 18, Ki = 0.11, Kd = 75;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0;

void PidData_Clear(void)
{
    I = 0;
    D = 0;
    PID_value = 0;
    previous_error = 0;
}

void PidData_Set(float error, float value)
{
    I = 0;
    D = 0;
    PID_value = value;
    previous_error = error;
}

void Calculate_pid(float inputError)
{
    error = inputError;
    P = error;
    I = I + error;
    D = error - previous_error;

    constrain_float(I, -200, 200); // 积分限幅

    PID_value = (Kp * P) + (Ki * I) + (Kd * D);
    PID_value = constrain_float(PID_value, -600, 600);

    previous_error = error;
}
