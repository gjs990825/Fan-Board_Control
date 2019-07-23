/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "usart.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
	USART1_Init(115200);
    LED_Init();

    while (1)
    {
        LED_Blink(0);
        delay_ms(100);
    }
    return 0;
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while(1)
    {
        printf("File: %s, line: %d\r\n", file, line);
        delay_ms(500);
    }
}

