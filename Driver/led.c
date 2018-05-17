#include "led.h"
#include "dwm_control.h"
#include "deca_spi.h"
#include "deca_regs.h"
#include "stdio.h"




/**************************
*
*		LED0:	PB7
*		LED1:	PB5
*		LED2:	PB6
*
**************************/

void LED_Init()
{
	
	LED_TIM4_Init();						//初始化LED闪烁定时器TIM4
}

u8 LED_status[4] = {LED_OFF,LED_OFF};  		//LED状态:开启，关闭，闪烁(快、中、慢)
u8 LED_count[4] = {0};			   			//计数阀值
u8 LED_temp[4] = {0};
u8 counter = 0;					   			//闪烁计数器

void LED_TIM4_Init()
{
	TIM_TimeBaseInitTypeDef TIM4_BaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);        		//使能TIM4时钟
	
    /*** 初始化TIM4作为LED闪烁频率控制器 ***/
    TIM4_BaseInitStructure.TIM_Prescaler = 7200 - 1;					//定时器时钟预分频系数	72M / 7.2k = 10kHz
    TIM4_BaseInitStructure.TIM_Period = 100 - 1;;						//设置计数周期			10kHz / 0.1k = 100Hz
    TIM4_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM4_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//时钟分频系数
    TIM_TimeBaseInit(TIM4, &TIM4_BaseInitStructure);

    /*** 设置中断优先级 ***/
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*** 使能TIM4更新中断 ***/
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM4, ENABLE);                                              //使能TIM4
}

void LED_Blink(u8 num,u8 freq)
{
	if(num < 4)
	{
		LED_count[num] = freq;
		if(LED_status[num] != LED_OFF)
			LED_status[num] = LED_BLINK;
	}

}
void LED_On(u8 num)
{
    if(num < 4)
    {
        LED_status[num] = LED_ON;
    }
}

void LED_Off(u8 num)
{
    if(num < 4)
    {
        LED_status[num] = LED_OFF;
    }
}
void TIM4_IRQHandler(void)

{
	/*** 查询中断类型 ***/
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == SET)
	{
		if(counter >= 50)
			counter = 0;
		counter++;
		
		/** LED0 **/
		if(LED_status[0] == LED_BLINK)
		{
			if(!(counter % LED_count[0])){
				LED_temp[0] = !LED_temp[0];
				dwt_setgpiovalue(GDM0,GDP0&LED_temp[0]);
			}
		}
		else if(LED_status[0] == LED_ON)
			dwt_setgpiovalue(GDM0,GDP0);
		else 
			dwt_setgpiovalue(GDM0,0);
		
		/** LED1 **/
		if(LED_status[1] == LED_BLINK)
		{
			if(!(counter % LED_count[1])){
				LED_temp[1] = !LED_temp[1];
				dwt_setgpiovalue(GDM1,GDP1*LED_temp[1]);
			}
		}
		else if(LED_status[1] == LED_ON)
			dwt_setgpiovalue(GDM1,GDP1);
		else
			dwt_setgpiovalue(GDM1,0);
		/** LED2 **/
		if(LED_status[2] == LED_BLINK)
		{
			if(!(counter % LED_count[2])){
				LED_temp[2] = !LED_temp[2];
				dwt_setgpiovalue(GDM2,GDP2*LED_temp[2]);
			}
		}
		else if(LED_status[2] == LED_ON)
			dwt_setgpiovalue(GDM2,GDP2);
		else
			dwt_setgpiovalue(GDM2,0);
		/** LED1 **/
		if(LED_status[3] == LED_BLINK)
		{
			if(!(counter % LED_count[3])){
				LED_temp[3] = !LED_temp[3];
				dwt_setgpiovalue(GDM3,GDP3*LED_temp[3]);
			}
		}
		else if(LED_status[3] == LED_ON)
			dwt_setgpiovalue(GDM3,GDP3);
		else
			dwt_setgpiovalue(GDM3,0);
		
	}
	/*** 清除中断标志位 ***/
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}
