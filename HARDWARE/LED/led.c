#include "led.h"
#include "delay.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_5);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

// 声光提示
void LightAndBeep(uint16_t t)
{
    for (uint16_t i = 0; i < t; i++)
    {
        LED1 = 1;
        BEEP = 1;
        delay_ms(50);
        LED1 = 0;
        BEEP = 0;
        delay_ms(50);
    }
}

void LED_Blink(uint8_t LEDn)
{
    
    switch (LEDn)
    {
        case 0:  LED0 = !LED0; break;
        case 1:  LED1 = !LED1; break;
        case 2:  LED2 = !LED2; break;
    
        default:
            break;
    }
}
