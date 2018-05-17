#ifndef _ADC_H
#define _ADC_H
#include "stm32f10x.h"



enum {
	KEY_UP = 0xF1,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_CENTER,
	KEY_NONE = 0xFF,
	ADC_ISR = 0xAD
};

typedef void (*adc_isr_t)(uint8_t cmd);

extern adc_isr_t adc_awd_isr;
extern u16 adc_RawValue;


void adc_init(void);
void adc_gpio_init(void);
void adc_dma_init(void);
void adc_watchdog_init(void);
u8 key_read(void);





#endif
