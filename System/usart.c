#include "sys.h"
#include "usart.h"
#include <stdio.h>
#include "dw_driver.h"

#if 1
#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;

};

FILE __stdout;
int _sys_exit(int x)
{
	x = x;
	return 0;
}
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0)
		;
    USART1->DR = (u8) ch;
	return ch;
}
#endif

t_floattobyte floattobyte;


/*****************************
*
*	串口引脚：PA9&PA10
*
*****************************/
void Uart1_Init(u32 baud)
{
	USART_InitTypeDef USART1_InitStructure;
	USART_ClockInitTypeDef USART1_ClockInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/***  开启GPIO和USART时钟 ***/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //开启USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	//配置PA9作为USART1　Tx
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	//配置PA10作为USART1　Rx
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA , &GPIO_InitStructure);

	//配置USART1
	//中断被屏蔽了
	USART1_InitStructure.USART_BaudRate = baud;  										//设置波特率
	USART1_InitStructure.USART_WordLength = USART_WordLength_8b;  						//8位数据格式
	USART1_InitStructure.USART_StopBits = USART_StopBits_1;   							//在帧结尾1停止位格式
	USART1_InitStructure.USART_Parity = USART_Parity_No;    							//禁用奇偶校验
	USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 	//硬件流控制失能
	USART1_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  					//发送、接收使能
	//配置USART1时钟
	USART1_ClockInitStructure.USART_Clock = USART_Clock_Disable;  						//时钟低电平活动
	USART1_ClockInitStructure.USART_CPOL = USART_CPOL_Low;  							//SLCK引脚上时钟输出的极性->低电平
	USART1_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;  							//时钟第二个边沿进行数据捕获
	USART1_ClockInitStructure.USART_LastBit = USART_LastBit_Disable; 					//最后一位数据的时钟脉冲不从SCLK输出

	USART_Init(USART1, &USART1_InitStructure);					//初始化串口1
	USART_ClockInit(USART1, &USART1_ClockInitStructure);		//初始化串口1时钟

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

	//使能USART1接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(USART1, ENABLE);

}

static u8 TxBuffer[256];    //串口发送缓冲
static u8 TxCounter = 0;    //串口发送计数器
static u8 count = 0;        //做发送缓冲数组指针
u8 Rx_Buf[2][32];           //双接收缓冲
static u8 Rx_Act = 0;       //当前活动缓冲
static u8 Rx_Adr = 0;       //当前接收字节序号
u8 Rx0_Ok = 0;
u8 Rx1_Ok = 0;

/*** 中断服务函数 ***/
void USART1_IRQHandler(void)
{
    u8 Rcv_Data = 0;
	
    /** ORE中断 **/
    if(USART1->SR & USART_SR_ORE)
        Rcv_Data = USART1->DR;  //取出数据

    /** 发送中断 **/
    if((USART1->SR & (1<<7)) && (USART1->CR1 & USART_CR1_TXEIE))  //非阻塞方式发送的实现
    {
        USART1->DR = TxBuffer[TxCounter++];         //持续发送缓冲中下一数据
        if(TxCounter == count)
        {
            USART1->CR1 &= ~USART_CR1_TXEIE;        //数据发送完成，关闭TXE中断
        }
    }

    /** 接收中断 **/
	if(USART1->SR & (1<<5))
	{
		Rcv_Data = USART1->DR;

		if(Rx_Adr==0)		    //当前接收第一个字节
		{
			if(Rcv_Data==0xAA)  //寻找帧头0XAAAF
			{
				Rx_Buf[Rx_Act][0] = Rcv_Data;
				Rx_Adr = 1;
			}
		}
		else if(Rx_Adr==1)      //当前接收第二个字节
		{
			if(Rcv_Data==0xAF)
			{
				Rx_Buf[Rx_Act][1] = Rcv_Data;
				Rx_Adr = 2;
			}
			else
				Rx_Adr = 0;
		}
		else if(Rx_Adr==2)		//第三字节:FUN
		{
			Rx_Buf[Rx_Act][2] = Rcv_Data;
			Rx_Adr = 3;
		}
		else if(Rx_Adr==3)		//第四字节:LEN
		{
			Rx_Buf[Rx_Act][3] = Rcv_Data;
			Rx_Adr = 4;
		}
		else                    //持续接收剩余字节
		{
			Rx_Buf[Rx_Act][Rx_Adr] = Rcv_Data;
			Rx_Adr ++;
		}
		if(Rx_Adr>=Rx_Buf[Rx_Act][3]+5)	//数据接收完毕:第四字节表示数据长度
		{
				
			Rx_Adr = 0;
			if(Rx_Act)
			{
				Rx_Act = 0; 			//切换缓存
				Rx1_Ok = 1;
			}
			else
			{
				Rx_Act = 1;
				Rx0_Ok = 1;
			}			
		}
	}
}

#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

uint8_t Uart1_Send_Char(unsigned char DataToSend)
{
    TxBuffer[count++] = DataToSend;                     //待发送数据放入缓冲

    if(!(USART1->CR1 & USART_CR1_TXEIE))                //如果发送中断未使能则使能
    	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    return DataToSend;
}
uint8_t Uart1_Send_Int16(uint16_t DataToSend)
{
	uint8_t sum = 0;
	TxBuffer[count++] = BYTE1(DataToSend);              //uint16 两个字节，小端模式高字节先送入缓冲
	TxBuffer[count++] = BYTE0(DataToSend);

	if(!(USART1->CR1 & USART_CR1_TXEIE))                //确保使能发送中断
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	sum += BYTE1(DataToSend);                           //返回发送数据校验和
	sum += BYTE0(DataToSend);
	return sum;
}
uint8_t Uart1_Send_Float(float DataToSend)
{
	uint8_t sum = 0;
	floattobyte.num = DataToSend;
	TxBuffer[count++] = floattobyte.byte[3];
	TxBuffer[count++] = floattobyte.byte[2];
	TxBuffer[count++] = floattobyte.byte[1];
	TxBuffer[count++] = floattobyte.byte[0];
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	sum += BYTE3(DataToSend);
	sum += BYTE2(DataToSend);
	sum += BYTE1(DataToSend);
	sum += BYTE0(DataToSend);
	return sum;
}
void Uart1_Send_String(unsigned char *Str)
{

	while(*Str)                 //判断Str指向的数据是否有效.
	{
		if(*Str=='\r')              //如果是回车符,则发送相应的回车 0x0d 0x0a
			Uart1_Send_Char(0x0d);
		else if(*Str=='\n')
			Uart1_Send_Char(0x0a);
		else
			Uart1_Send_Char(*Str);

		Str++;                      //指针++ 指向下一个字节.
	}
}
void Uart1_Send_Buf(u8 *DataToSend , u8 data_num)
{
	u8 i = 0;
	for(i = 0;i<data_num;i++)
		TxBuffer[count++] = *(DataToSend+i);
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}



