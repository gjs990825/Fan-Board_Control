#ifndef _LED_H_
#define _LED_H_

#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED0 PCout(13)
//#define LED1 PBout(12)
//#define LED2 PBout(13)

void LED_Init(void);
void LED_Blink(uint8_t LEDn);

#ifdef __cplusplus
}
#endif

#endif

