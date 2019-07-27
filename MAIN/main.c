/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "motor.h"
#include "pid.h"
#include "mpu6050.h"
#include "iic.h"
#include "timer.h"
#include <math.h>

void Turn_To(int target)
{
    Control_SetTargetAngle(target);
    Control_SetStatus(Status_Run);

    delay_ms(500);

    while (!Is_Stablilized())
        ;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    USART1_Init(115200);
    printf("Usart OK\r\n");
    LED_Init();

    TIM1_PWMInit(999, 0);
    TIM3_TimerInit(719, 1999); // 50 ms

    I2C_Soft_Init();
    MPU6050_Init();

    // while (1)
    // {
    //     LightAndBeep(10);
    //     delay_ms(1000);
    // }

    Control_SetTargetAngle(120.0);
    Control_SetStatus(Status_Run);

    while (1)
    {
        // printf("Average:%f\tVariance:%f\tSTA:%d\r\n", Get_AngleAverageError(), Get_AngleVariance(), Is_Stablilized());

        Turn_To(60);
        LightAndBeep(10);
        delay_ms(1000);
        Turn_To(120);
        LightAndBeep(10);
        delay_ms(1000);

        // printf("CurrentAngle:%f\tPID_OUT:%f\tAverage:%f\tVariance:%f\tSTA:%d\r\n",
        //        Get_CurrentAngle(),
        //        PID_value,
        //        Get_AngleAverageError(),
        //        Get_AngleVariance(),
        //        Is_Stablilized());

        // delay_ms(100);
    }

    // while (1)
    // {
    //     printf("%lf\r\n", MPU6050_GetAngle());
    //     delay_ms(100);
    // }

    // bool dir = 0;
    // int current = 0;

    // while (1)
    // {
    //     if (dir)
    //         current++;
    //     else
    //         current--;

    //     if (current < -MAX_SPEED || current > MAX_SPEED)
    //         dir = !dir;

    //     Motor_SpeedControl(current);
    //     delay_ms(10);

    //     // printf("%d\r\n", current);
    // }
    // return 0;
}

void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
    {
        printf("File: %s, line: %d\r\n", file, line);
        delay_ms(500);
    }
}
