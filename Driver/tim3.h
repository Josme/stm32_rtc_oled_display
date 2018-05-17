#ifndef __TIM3_H__
#define __TIM3_H__
#include "sys.h"

extern u8 flag_oled;
extern u8 flag_rc_get;
extern u8 flag_rc_cal;
extern u8 flag_rc_send;
extern u8 flag_rc_key;
extern u8 flag_nrf_check;
extern u8 flag_lock;

void TIM3_Init(void);







#endif
