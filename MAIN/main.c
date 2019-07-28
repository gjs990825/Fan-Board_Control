/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "motor.h"
#include "pid.h"
#include "mpu6050.h"
#include "iic.h"
#include "timer.h"
#include <math.h>
#include "key.h"
#include "gui.h"

void Turn_To(int target)
{
    LightAndBeep(5);

    Control_SetTargetAngle(target);
    Control_SetStatus(Status_Run);

    delay_ms(500);

    while (!Is_Stablilized())
        ;
    LightAndBeep(5);
}

int main(void)
{
    uint32_t DataOutput_Stamp = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    USART1_Init(115200);
    printf("Usart OK\r\n");
    LED_Init();
    KEY_Init();
    
    LightAndBeep(2);

    TIM1_PWMInit(999, 0);
    TIM3_TimerInit(719, 1999); // 50 ms
    TIM2_Init(719, 99);        // 1ms 全局时间

    I2C_Soft_Init();
    MPU6050_Init();

    OLED_Init();
    OLED_Fill(0xAA);

    

    OLED_GUI();

    Control_SetTargetAngle(90.0);
    Control_SetStatus(Status_Run);

    // while (1)
    // {
    //     for (int16_t i = 45; i <= 135; i += 30)
    //     {
    //         Turn_To(135);
    //         Turn_To(35);
    //         Turn_To(90);
    //     }
    // }

    while (1)
    {
        Set_PIDParamFromUSART();

        uint8_t key = KEY_Scan(0);
        if (key)
        {
            printf("KEY%d\r\n", key);
        }

        if (IsTimeOut(DataOutput_Stamp, 200))
        {
            DataOutput_Stamp = millis();

            printf("Angle:%f\tOUT:%f\tAver:%f\tVari:%f\tSTA:%d\r\n",
                   Get_CurrentAngle(),
                   PID_value,
                   Get_AngleAverageError(),
                   Get_AngleVariance(),
                   Is_Stablilized());
        }
    }

    // while (1)
    // {
    //     printf("Average:%f\tVariance:%f\tSTA:%d\r\n", Get_AngleAverageError(), Get_AngleVariance(), Is_Stablilized());

    //     Turn_To(60);
    //     LightAndBeep(10);
    //     delay_ms(1000);
    //     Turn_To(120);
    //     LightAndBeep(10);
    //     delay_ms(1000);

    //     printf("CurrentAngle:%f\tPID_OUT:%f\tAverage:%f\tVariance:%f\tSTA:%d\r\n",
    //            Get_CurrentAngle(),
    //            PID_value,
    //            Get_AngleAverageError(),
    //            Get_AngleVariance(),
    //            Is_Stablilized());

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
