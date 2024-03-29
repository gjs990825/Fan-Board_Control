#ifndef _USART_H_
#define _USART_H_

#include "sys.h"

#define USART_REC_LEN 200         //定义最大接收字节数 200
#define USART1_ENABLE_INTERRUPT 1 //中断使能(使用scanf函数请勿使能，可能会丢失数据)

#define USART_RECEIVED (USART_RX_STA & 0x8000)
#define USART_CLR_REC() USART_RX_STA = 0

extern u8 USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART_RX_STA;               //接收状态标记

extern char UART_read(void);
extern void UART_write(char);

void USART1_Init(uint32_t baudrate);

#endif //_USART_H_
