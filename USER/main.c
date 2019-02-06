/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "usart.h"

int main(void)
{
	int a = 0, b = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
	USART1_Init(115200);
    LED_Init();
    
	printf(" ‰»Îab\r\n");
	scanf("%d %d", &a, &b);

    while (1)
    {
        LED_Blink(0);
        delay_ms(100);
		printf("a:%d, b:%d\r\n", a, b);
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

