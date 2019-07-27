#include "timer.h"
#include "led.h"
#include "mpu6050.h"
#include "motor.h"
#include "my_lib.h"
#include <stdlib.h>

#define Stabilized_Variance 0.5f
#define Stabilized_AverageError 3.0f

// Ŀ��Ƕ�ֵ
double targetAngle = 90.0;

// ��ǰ�Ƕ�ֵ
double currentAngle = 0;

// ��������״̬
ControlStatus_t controlStatus = Status_Stop;

// ���ʮ�β����ĽǶ�ֵ
double recentTenAngle[10];

// ��ȡ����ֵ
float Get_AngleVariance(void)
{
	return Variance(recentTenAngle, 10);
}

// ��ȡƽ�����ֵ
float Get_AngleAverageError(void)
{
	return targetAngle - Average(recentTenAngle, 10);
}

// �Ƿ��Ѿ��ȶ�
bool Is_Stablilized(void)
{
	return (Get_AngleVariance() < Stabilized_Variance) &&
		   (abs(Get_AngleAverageError()) < Stabilized_AverageError);
}

// �趨Ŀ��Ƕ�
void Control_SetTargetAngle(double target)
{
	targetAngle = target;
}

// �趨����״̬
void Control_SetStatus(ControlStatus_t status)
{
	controlStatus = status;
}

// ��ȡ��ǰ�Ƕ�
double Get_CurrentAngle(void)
{
	return currentAngle;
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE);
}

#define DEBUG_PIN LED0

#define cycleTimes 2
uint8_t intCounter = 0;
#define maxArrayNum 10
uint8_t currentArrayNum = 0;

static inline void recoderHandle(void)
{
	intCounter++;
	if (intCounter >= cycleTimes)
	{
		intCounter = 0;

		recentTenAngle[currentArrayNum] = currentAngle;
		currentArrayNum++;

		if (currentArrayNum == maxArrayNum)
		{
			currentArrayNum = 0;
		}
	}
}

void TIM3_IRQHandler(void)
{
	DEBUG_PIN = 1;

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //�����־λ

		switch (controlStatus)
		{
		case Status_Run:

			currentAngle = MPU6050_GetAngle();
			Calculate_pid(targetAngle - currentAngle);
			Motor_SpeedControl(PID_value);

			recoderHandle();

			break;

		default:
			Motor_ShutDown();
			break;
		}
	}

	DEBUG_PIN = 0;
}
