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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);					//������ʱ��3ʱ��
	
    /*** ��ʼ��TIM3��Ϊ����״̬�������� ***/
    TIM3_BaseInitStructure.TIM_Prescaler = 720 - 1;						//��ʱ��ʱ��Ԥ��Ƶϵ��	72M / 0.72k = 100kHz
    TIM3_BaseInitStructure.TIM_Period = 30000 - 1;;						//���ü�������	  	  100kHz / 0.1k = 1kHz
    TIM3_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM3_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//ʱ�ӷ�Ƶϵ��
    TIM_TimeBaseInit(TIM3, &TIM3_BaseInitStructure);
	
    /*** �����ж����ȼ� ***/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*** ʹ��TIM3�����ж� ***/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM3, ENABLE);                                              //ʹ��TIM3
	
}

/** 1kHz ����1ms **/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		count++;
		adc_awd_isr(ADC_ISR);
	}	
	/*** ����жϱ�־λ ***/
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
