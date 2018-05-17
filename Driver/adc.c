#include "adc.h"
#include "usart.h"

/*******************
*
*	GPIO		PA0
*	UP			0.5
*	DOWN		0.825
*	LEFT		0.75
*	RIGHT		0.872
*	CENTER		0.667
*
*******************/

#define AD_KEY_CH	0
#define AD_KEY_OFFSET	50
#define AD_OFFSET	4096

#define AD_UP			(0.5	*	AD_OFFSET)
#define AD_DOWN			(0.825	*	AD_OFFSET)
#define AD_LEFT			(0.75	*	AD_OFFSET)
#define AD_RIGHT		(0.872	*	AD_OFFSET)
#define AD_CENTER		(0.667	*	AD_OFFSET)

#define KEY_CHK(x,y)	(((x) - (y) < AD_KEY_OFFSET) && ((y) - (x) < AD_KEY_OFFSET))

#define ADC1_DR_Address    ((u32)ADC1_BASE+0x4c)

u16 adc_RawValue = 0;
adc_isr_t adc_awd_isr;				//


/*** check the key value for once ***/
u8 key_read(void)
{
	if(KEY_CHK(adc_RawValue,AD_UP))
		return KEY_UP;
	else if(KEY_CHK(adc_RawValue,AD_DOWN))
		return KEY_DOWN;
	else if(KEY_CHK(adc_RawValue,AD_LEFT))
		return KEY_LEFT;
	else if(KEY_CHK(adc_RawValue,AD_RIGHT))
		return KEY_RIGHT;
	else if(KEY_CHK(adc_RawValue,AD_CENTER))
		return KEY_CENTER;
	else
		return 0xFF;
}

/*** ADC1 ISR ***/
extern u8 key_val;
void ADC1_2_IRQHandler()
{
	if(ADC_GetITStatus(ADC1, ADC_IT_AWD)){				//anolog watch dog interrupt
		if(adc_awd_isr){
			key_val = key_read();
		}
		ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
	}
}


/*** initial adc for all ***/
void adc_init(void)
{
	adc_gpio_init();
	adc_dma_init();
	adc_watchdog_init();
}


/*** initial adc gpio ***/
void adc_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /** enable gpio clock  **/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    /* init gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*** adc analog watchdog function initial ***/
void adc_watchdog_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
	
	ADC_AnalogWatchdogSingleChannelConfig(ADC1,ADC_Channel_0);				//set single channel number
	ADC_AnalogWatchdogThresholdsConfig(ADC1, AD_OFFSET-1, 0.9*AD_OFFSET);		//set thresholds
	
    /*** 设置中断优先级 ***/
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	ADC_ITConfig(ADC1,ADC_IT_AWD, ENABLE);

	ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_SingleRegEnable);			//enable single regular mode
}

/*** initial adc dma,auto transfer ***/
void adc_dma_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

    /** 使能DMA时钟 **/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /** 使能ADC & GPIO 时钟  **/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	                //ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_RawValue;                  //数组内存首地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;                                       //6个数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		                        //循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	                        //独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	                            //禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	                        //开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	        //不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	                    //采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 	                                //要转换的通道数目
	ADC_Init(ADC1, &ADC_InitStructure);

	/*配置ADC时钟，为PCLK2的6分频，即12MHz,ADC频率最高不能超过14MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	/*配置ADC1的通道1为55.	5个采样周期，将待转换通道按顺序分配序号 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);

	/* 使能 ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);

	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));

	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


