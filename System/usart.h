#ifndef __USART_H
#define __USART_H
#include "sys.h"

//float分解为字节用的共同体
typedef union
{
    u8 byte[4];
    float num;
}t_floattobyte;

extern u8 Rx_Buf[2][32];	//两个32字节的串口接收缓存
extern u8 Rx0_Ok;		    //接收完毕标志
extern u8 Rx1_Ok;

void Uart1_Init(u32 br_num);
void Uart1_IRQ(void);

void Uart1_Send_String(unsigned char *Str);
uint8_t Uart1_Send_Char(unsigned char DataToSend);
uint8_t Uart1_Send_Int16(uint16_t DataToSend);
uint8_t Uart1_Send_Float(float DataToSend);
void Uart1_Send_Buf(unsigned char *DataToSend , u8 data_num);

#endif
