#ifndef __LED_H__
#define __LED_H__
#include "sys.h"

#define LED0 PAout(3)
#define LED1 PAout(4)

#define LED_FAST 5
#define LED_MID  20
#define LED_LOW  50

#define LED_ON      1
#define LED_BLINK   2
#define LED_OFF     0

void LED_Init(void);
void LED_TIM4_Init(void);
void LED_On(u8 num);
void LED_Off(u8 num);
void LED_Blink(u8 num,u8 freq);







#endif

