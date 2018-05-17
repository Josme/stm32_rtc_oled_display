#include "tim3.h"
#include "led.h"
#include "dataTrans.h"
#include "adc.h"
u16 count = 0;

u8 count_1ms = 1;
u8 count_2ms = 2;
u8 count_5ms = 5;
u8 count_10ms = 10;
u8 count_20ms = 20;
u8 count_50ms = 50;
u8 count_100ms = 100;
u8 count_200ms = 200;
u16 count_500ms = 500;
u16 count_1000ms = 1000;

u8 flag_oled = 0;
u8 flag_rc_get = 0;
u8 flag_rc_cal = 0;
u8 flag_rc_send = 0;
u8 flag_rc_key = 0;
u8 flag_nrf_check = 0;
u8 flag_lock = 1;

extern adc_isr_t adc_awd_isr;
void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM3_BaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);					//开启定时器3时钟
	
    /*** 初始化TIM3作为任务状态机控制器 ***/
    TIM3_BaseInitStructure.TIM_Prescaler = 720 - 1;						//定时器时钟预分频系数	72M / 0.72k = 100kHz
    TIM3_BaseInitStructure.TIM_Period = 30000 - 1;;						//设置计数周期	  	  100kHz / 0.1k = 1kHz
    TIM3_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM3_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//时钟分频系数
    TIM_TimeBaseInit(TIM3, &TIM3_BaseInitStructure);
	
    /*** 设置中断优先级 ***/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*** 使能TIM3更新中断 ***/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM3, ENABLE);                                              //使能TIM3
	
}

/** 1kHz 周期1ms **/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		count++;
		adc_awd_isr(ADC_ISR);
	}	
	/*** 清除中断标志位 ***/
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
