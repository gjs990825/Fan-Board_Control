#include "timer.h"
#include "led.h"
#include "mpu6050.h"
#include "motor.h"
#include "my_lib.h"
#include <stdlib.h>
#include "gui.h"

// 毫秒时间戳
volatile uint32_t global_times = 0;

#define Stabilized_Variance 0.5f
#define Stabilized_AverageError 3.0f

// 目标角度值
double targetAngle = 90.0;

// 当前角度值
double currentAngle = 0;

// 当先运行状态
ControlStatus_t controlStatus = Status_Stop;

// 最近十次采样的角度值
double recentTenAngle[10];

// 获取方差值
float Get_AngleVariance(void)
{
	return Variance(recentTenAngle, 10);
}

// 获取平均误差值
float Get_AngleAverageError(void)
{
	return targetAngle - Average(recentTenAngle, 10);
}

// 是否已经稳定
bool Is_Stablilized(void)
{
	return (Get_AngleVariance() < Stabilized_Variance) &&
		   (abs(Get_AngleAverageError()) < Stabilized_AverageError);
}

// 设定目标角度
void Control_SetTargetAngle(double target)
{
	targetAngle = target;
}

// 设定运行状态
void Control_SetStatus(ControlStatus_t status)
{
	controlStatus = status;
}

// 获取当前角度
double Get_CurrentAngle(void)
{
	return currentAngle;
}

// 获取目标角度
float Get_TargetAngle(void)
{
	return targetAngle;
}

void TIM3_TimerInit(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE);
}

void TIM4_TimerInit(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4, ENABLE);
}

void TIM2_TimerInit(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitSturcture;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitSturcture.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitSturcture.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitSturcture.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitSturcture.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitSturcture);

	TIM_Cmd(TIM2, ENABLE);
}

// 采样周期
#define cycleTimes 2
// 采样数组长度
#define maxArrayNum 10

// 记录数据
static inline void recoderHandle(void)
{
	static uint8_t intCounter = 0;
	static uint8_t currentArrayNum = 0;

	intCounter++;
	if (intCounter >= cycleTimes)
	{
		intCounter = 0;

		recentTenAngle[currentArrayNum] = currentAngle;
		currentArrayNum++;

		if (currentArrayNum == maxArrayNum)
			currentArrayNum = 0;
	}
}

#define DEBUG_PIN LED0

void TIM3_IRQHandler(void)
{
	DEBUG_PIN = 1;

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除标志位

		currentAngle = MPU6050_GetAngle();
		recoderHandle();

		switch (controlStatus)
		{
		case Status_Run:

			Motor_SpeedControl(Calculate_PID(targetAngle - currentAngle));

			break;

		default:
			Motor_ShutDown();
			break;
		}
	}

	DEBUG_PIN = 0;
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		global_times++;
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

		GUI_RefreashInterface(componentsSet, componentsNumber);
	}
}
