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

extern volatile uint32_t global_times;

// 获取当前时间戳
#define millis() (global_times)
// 返回是否超时（单位ms）
#define IsTimeOut(setTimeStamp, timeOutLimit) (millis() > (setTimeStamp + (timeOutLimit)))

// 等待某个标志位，超时则忽略
#define WaitForFlagInMs(flag, status, timeout)                        \
    do                                                                \
    {                                                                 \
        uint32_t startStamp = millis();                               \
        while ((!IsTimeOut(startStamp, timeout)) && (flag != status)) \
        {                                                             \
        };                                                            \
    } while (0)

void Timer_Init(uint16_t arr, uint16_t psc);

// 程序调用需要耗费时间，使用宏定义和内联函数减少时间开销
// warning : 目前没有进行数据接收操作，后续需要验证此改动对接收的影响

// 通过时间戳延时（精度1ms）
// 可长时间延时
static inline void delay(uint32_t ms)
{
    uint32_t startStamp = millis();
    for (;;)
    {
        if (IsTimeOut(startStamp, ms))
            break;
    }
}

void Control_SetTargetAngle(double target);
void Control_SetStatus(ControlStatus_t status);

float Get_TargetAngle(void);
double Get_CurrentAngle(void);

void TIM2_TimerInit(uint16_t arr, uint16_t psc);
void TIM3_TimerInit(uint16_t arr, uint16_t psc);
void TIM4_TimerInit(uint16_t arr, uint16_t psc);

#endif
