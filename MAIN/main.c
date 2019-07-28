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
#include "my_lib.h"
#include "stdlib.h"

uint8_t mission1Start = false;
uint8_t mission2Start = false;

// 到某一个角度
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

// 在两个角度之间摆动
void Swing_Between(int a, int b, uint8_t times)
{
    for (uint8_t i = 0; i < times; i++)
    {
        Control_SetTargetAngle(b);
        while (abs(Get_AngleAverageError()) > 1)
            ;
        Control_SetTargetAngle(a);
        while (abs(Get_AngleAverageError()) > 1)
            ;
    }
    Control_SetTargetAngle(a);
}

void Key_Handle(uint8_t key)
{
    if (key == 5)
    {
        GUI_SetFlash(&TargetValue, true);
        Key_GetSetValue(&setAngle1);
        GUI_SetFlash(&TargetValue, false);
        setAngle1 = constrain_float(setAngle1, 45, 145);

        GUI_SetFlash(&Target2Value, true);
        Key_GetSetValue(&setAngle2);
        GUI_SetFlash(&Target2Value, false);
        setAngle2 = constrain_float(setAngle2, 45, 145);

        if (setAngle1 > setAngle2)
        {
            setAngle2 = setAngle1 = (setAngle1 + setAngle2) / 2;
        }

        key = GUI_ConfirmPage();
        mission1Start = (key == 5) ? true : false;
        printf("start?:%d\r\n", mission1Start);
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
    GUI_ChangeDisplay(componentsSet, componentsNumber);

    TIM1_PWMInit(999, 0);      // 分度1000 PWM
    TIM3_TimerInit(719, 1999); // 20ms PID控制
    TIM2_TimerInit(719, 99);   // 1ms 全局时间
    TIM4_TimerInit(719, 9999); // 100ms 屏幕刷新

    Control_SetTargetAngle(35.0);
    Control_SetStatus(Status_Stop);

    // while (1)
    // {
    //     Control_SetStatus(Status_Run);
    //     Swing_Between(60, 110, 2);
    //     while (1)
    //         ;
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

        if (mission1Start)
        {
            mission1Start = false;

            Control_SetStatus(Status_Run);

            Turn_To(setAngle1);
            Turn_To(setAngle2);

            Swing_Between(setAngle2, setAngle1, 3);

            Turn_To(145);

            Control_SetStatus(Status_Stop);
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
}

void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
    {
        printf("File: %s, line: %d\r\n", file, line);
        delay_ms(500);
    }
}
