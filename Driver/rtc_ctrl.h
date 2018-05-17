#ifndef __RTC_CTRL_H
#define __RTC_CTRL_H
#include "stm32f10x.h"
typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t sec;
}rtc_struct;
extern rtc_struct pDate;
uint8_t RTC_SetSec(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec);
uint8_t RTC_GetDate(rtc_struct * thisDate);
void RTC_Init(void);
#endif