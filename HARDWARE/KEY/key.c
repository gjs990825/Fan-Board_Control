#include "stm32f10x.h"
#include "key.h"
#include "sys.h"
#include "delay.h"

void KEY_Init(void) //IO��ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

#define KEY1_PRES 1
#define KEY2_PRES 2
#define KEY3_PRES 3
#define KEY4_PRES 4
#define KEY5_PRES 5

#define IfKey(n)     \
	if (KEY##n == 0) \
		return KEY##n##_PRES;

#define ElifKey(n)   \
	if (KEY##n == 0) \
		return KEY##n##_PRES;

// 0Ϊ��֧������
uint8_t KEY_Scan(uint8_t mode)
{
	static uint8_t key_up = 1; //�������ɿ���־
	if (mode)
	{
		key_up = 1; //֧������
	}

	if (key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0 || KEY5 == 0))
	{
		delay_ms(10); //ȥ����
		key_up = 0;

		IfKey(1);
		ElifKey(2);
		ElifKey(3);
		ElifKey(4);
		ElifKey(5);
	}
	else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1 && KEY5 == 1)
	{
		key_up = 1;
	}
	return 0; // �ް�������
}
