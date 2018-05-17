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
*	�������ţ�PA9&PA10
*
*****************************/
void Uart1_Init(u32 baud)
{
	USART_InitTypeDef USART1_InitStructure;
	USART_ClockInitTypeDef USART1_ClockInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/***  ����GPIO��USARTʱ�� ***/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //����USART1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	//����PA9��ΪUSART1��Tx
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	//����PA10��ΪUSART1��Rx
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA , &GPIO_InitStructure);

	//����USART1
	//�жϱ�������
	USART1_InitStructure.USART_BaudRate = baud;  										//���ò�����
	USART1_InitStructure.USART_WordLength = USART_WordLength_8b;  						//8λ���ݸ�ʽ
	USART1_InitStructure.USART_StopBits = USART_StopBits_1;   							//��֡��β1ֹͣλ��ʽ
	USART1_InitStructure.USART_Parity = USART_Parity_No;    							//������żУ��
	USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 	//Ӳ��������ʧ��
	USART1_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  					//���͡�����ʹ��
	//����USART1ʱ��
	USART1_ClockInitStructure.USART_Clock = USART_Clock_Disable;  						//ʱ�ӵ͵�ƽ�
	USART1_ClockInitStructure.USART_CPOL = USART_CPOL_Low;  							//SLCK������ʱ������ļ���->�͵�ƽ
	USART1_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;  							//ʱ�ӵڶ������ؽ������ݲ���
	USART1_ClockInitStructure.USART_LastBit = USART_LastBit_Disable; 					//���һλ���ݵ�ʱ�����岻��SCLK���

	USART_Init(USART1, &USART1_InitStructure);					//��ʼ������1
	USART_ClockInit(USART1, &USART1_ClockInitStructure);		//��ʼ������1ʱ��

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

	//ʹ��USART1�����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//ʹ��USART1
	USART_Cmd(USART1, ENABLE);

}

static u8 TxBuffer[256];    //���ڷ��ͻ���
static u8 TxCounter = 0;    //���ڷ��ͼ�����
static u8 count = 0;        //�����ͻ�������ָ��
u8 Rx_Buf[2][32];           //˫���ջ���
static u8 Rx_Act = 0;       //��ǰ�����
static u8 Rx_Adr = 0;       //��ǰ�����ֽ����
u8 Rx0_Ok = 0;
u8 Rx1_Ok = 0;

/*** �жϷ����� ***/
void USART1_IRQHandler(void)
{
    u8 Rcv_Data = 0;
	
    /** ORE�ж� **/
    if(USART1->SR & USART_SR_ORE)
        Rcv_Data = USART1->DR;  //ȡ������

    /** �����ж� **/
    if((USART1->SR & (1<<7)) && (USART1->CR1 & USART_CR1_TXEIE))  //��������ʽ���͵�ʵ��
    {
        USART1->DR = TxBuffer[TxCounter++];         //�������ͻ�������һ����
        if(TxCounter == count)
        {
            USART1->CR1 &= ~USART_CR1_TXEIE;        //���ݷ�����ɣ��ر�TXE�ж�
        }
    }

    /** �����ж� **/
	if(USART1->SR & (1<<5))
	{
		Rcv_Data = USART1->DR;

		if(Rx_Adr==0)		    //��ǰ���յ�һ���ֽ�
		{
			if(Rcv_Data==0xAA)  //Ѱ��֡ͷ0XAAAF
			{
				Rx_Buf[Rx_Act][0] = Rcv_Data;
				Rx_Adr = 1;
			}
		}
		else if(Rx_Adr==1)      //��ǰ���յڶ����ֽ�
		{
			if(Rcv_Data==0xAF)
			{
				Rx_Buf[Rx_Act][1] = Rcv_Data;
				Rx_Adr = 2;
			}
			else
				Rx_Adr = 0;
		}
		else if(Rx_Adr==2)		//�����ֽ�:FUN
		{
			Rx_Buf[Rx_Act][2] = Rcv_Data;
			Rx_Adr = 3;
		}
		else if(Rx_Adr==3)		//�����ֽ�:LEN
		{
			Rx_Buf[Rx_Act][3] = Rcv_Data;
			Rx_Adr = 4;
		}
		else                    //��������ʣ���ֽ�
		{
			Rx_Buf[Rx_Act][Rx_Adr] = Rcv_Data;
			Rx_Adr ++;
		}
		if(Rx_Adr>=Rx_Buf[Rx_Act][3]+5)	//���ݽ������:�����ֽڱ�ʾ���ݳ���
		{
				
			Rx_Adr = 0;
			if(Rx_Act)
			{
				Rx_Act = 0; 			//�л�����
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
    TxBuffer[count++] = DataToSend;                     //���������ݷ��뻺��

    if(!(USART1->CR1 & USART_CR1_TXEIE))                //��������ж�δʹ����ʹ��
    	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    return DataToSend;
}
uint8_t Uart1_Send_Int16(uint16_t DataToSend)
{
	uint8_t sum = 0;
	TxBuffer[count++] = BYTE1(DataToSend);              //uint16 �����ֽڣ�С��ģʽ���ֽ������뻺��
	TxBuffer[count++] = BYTE0(DataToSend);

	if(!(USART1->CR1 & USART_CR1_TXEIE))                //ȷ��ʹ�ܷ����ж�
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	sum += BYTE1(DataToSend);                           //���ط�������У���
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

	while(*Str)                 //�ж�Strָ��������Ƿ���Ч.
	{
		if(*Str=='\r')              //����ǻس���,������Ӧ�Ļس� 0x0d 0x0a
			Uart1_Send_Char(0x0d);
		else if(*Str=='\n')
			Uart1_Send_Char(0x0a);
		else
			Uart1_Send_Char(*Str);

		Str++;                      //ָ��++ ָ����һ���ֽ�.
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



