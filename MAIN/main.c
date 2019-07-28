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
#include "oled.h"

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

void Key_Handle(uint8_t key)
{

    if (key == 5)
    {
        float setTargetAngle = Get_TargetAngle();
        int8_t quitOption = 0;

        Control_SetStatus(Status_Stop);
        TargetValue.needBlink = true;

        while (1)
        {
            key = KEY_Scan(1);

            switch (key)
            {
            case 2:
                Control_SetTargetAngle(Get_TargetAngle() - 0.1);
                break;
            case 3:
                Control_SetTargetAngle(Get_TargetAngle() + 0.1);
                break;

            case 1:
                quitOption = -1;
                break;
            case 4:
                quitOption = -1;
                break;
            case 5:
                quitOption = 1;
                break;

            default:
                break;
            }

            if (quitOption)
                break;
        }

        while (key == KEY_Scan(1));

        if (quitOption == 1)
        {
            // 四舍五入
            Control_SetTargetAngle((int)(Get_TargetAngle() + 0.5));
            Control_SetStatus(Status_Run);
        }
        else
        {
            Control_SetTargetAngle(setTargetAngle);
        }

        TargetValue.needBlink = false;
    }
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

    LightAndBeep(1);

    I2C_Soft_Init();

    MPU6050_Init();
    OLED_Init();
    OLED_CLS();

    TIM1_PWMInit(999, 0);      // 分度1000 PWM
    TIM3_TimerInit(719, 1999); // 20ms PID控制
    TIM2_TimerInit(719, 99);   // 1ms 全局时间
    TIM4_TimerInit(719, 9999); // 100ms 屏幕刷新

    Control_SetTargetAngle(35.0);
    Control_SetStatus(Status_Stop);

    // while (1)
    // {
    //     // for (int16_t i = 45; i <= 135; i += 30)
    //     // {
    //     //     Turn_To(135);
    //     //     Turn_To(35);
    //     //     Turn_To(90);
    //     // }

    //     Turn_To(135);
    //     Turn_To(35);
    //     Turn_To(90);
    // }

    while (1)
    {
        Set_PIDParamFromUSART();

        uint8_t key = KEY_Scan(1);
        if (key)
        {
            while (key == KEY_Scan(1))
                ;
            printf("KEY%d\r\n", key);
            Key_Handle(key);
        }

        if (IsTimeOut(DataOutput_Stamp, 200))
        {
            DataOutput_Stamp = millis();

            printf("Angle:%f\tOUT:%f\tAver:%f\tVari:%f\tSTA:%d\r\n",
                   Get_CurrentAngle(),
                   PID_Value,
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
    //            PID_Value,
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
