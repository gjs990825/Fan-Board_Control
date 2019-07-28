#include "pid.h"
#include "my_lib.h"
#include "usart.h"
#include "stdio.h"

// float Kp = 18, Ki = 0.21, Kd = 75;
// 重物 // 25.0, 0.50, 255.0
// 无重物 // 25.0, 0.30, 205.0

float Kp = 28, Ki = 0.0, Kd = 175.0;

float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0;



void Set_PIDParamFromUSART(void)
{
    if (USART_RECEIVED)
    {
        USART_CLR_REC();

        printf("%s\r\n", USART_RX_BUF);

        float p, i, d;
        if (sscanf((char *)USART_RX_BUF, "%f, %f, %f", &p, &i, &d) == 3)
        {
            printf("P:%f\tI:%f\tD:%f\t\r\n", p, i, d);
            PID_SetParam(p, i, d);
        }
        else
        {
            printf("Error\r\n");
        }
    }
}


void PidData_Clear(void)
{
    I = 0;
    D = 0;
    PID_value = 0;
    previous_error = 0;
}

void PID_SetParam(float p, float i, float d)
{
    Kp = p;
    Ki = i;
    Kd = d;
    PidData_Clear();
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

    I = constrain_float(I, -100.0, 100.0); // 积分限幅

    PID_value = (Kp * P) + (Ki * I) + (Kd * D);
    PID_value = constrain_float(PID_value, -500, 500);

    previous_error = error;
}
