#include "dataTrans.h"
#include "usart.h"

/** С��ģʽ���ֽ����ݵ�ַӳ�� **/
#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))


u8 data_to_send[50];


/** �����ݽ�����ɵ���Ӧ��������ͨ��NRF���͵��ɿ� **/
void DT_CheckEvent(void)
{
//	if(Rx0_Ok)
//	{
//		Rx0_Ok = 0;
//		NRF_TxPacket_ACKPayload(Rx_Buf[0],Rx_Buf[0][3]+5);
//	}
//	if(Rx1_Ok)
//	{
//		Rx1_Ok = 0;
//		NRF_TxPacket_ACKPayload(Rx_Buf[1],Rx_Buf[1][3]+5);
//	}

//    /** �������ݰ�֮����2401�ж�״̬ **/
//    NRF_Check_Event();
}

/** ����ң������ **/
void DT_SendRCData(void)
{
//	u8 _cnt=0;
//	u8 sum = 0;
//	u8 i = 0;

//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0xAF;
//	data_to_send[_cnt++]=0x03;
//	data_to_send[_cnt++]=0;
//	data_to_send[_cnt++]=BYTE1(rc_Data[THROTTLE]);
//	data_to_send[_cnt++]=BYTE0(rc_Data[THROTTLE]);
//	data_to_send[_cnt++]=BYTE1(rc_Data[YAW]);
//	data_to_send[_cnt++]=BYTE0(rc_Data[YAW]);
//	data_to_send[_cnt++]=BYTE1(rc_Data[ROLL]);
//	data_to_send[_cnt++]=BYTE0(rc_Data[ROLL]);
//	data_to_send[_cnt++]=BYTE1(rc_Data[PITCH]);
//	data_to_send[_cnt++]=BYTE0(rc_Data[PITCH]);

//	data_to_send[_cnt++]=(aux_Data[0] >> 8) & 0xFF;
//	data_to_send[_cnt++]=aux_Data[0] & 0xFF;

//	data_to_send[3] = _cnt-4;

//	for(i = 0;i<_cnt;i++)
//		sum += data_to_send[i];

//	data_to_send[_cnt++]=sum;

//	DT_SendData(data_to_send, _cnt);
}
void DT_SendDFData(void)
{
//	data_to_send[0] = (rc_Data[THROTTLE]/10-100);
//	data_to_send[1] = (rc_Data[PITCH]/10-100);
//	data_to_send[2] = (rc_Data[ROLL]/10-100);
//	data_to_send[3] = (rc_Data[YAW]/10-100);
//	
//	data_to_send[31] = 0x02;
//	
//	DT_SendData(data_to_send,32);
}
/** ң�����ݷ��͵ײ�ӿ�:NRF **/
void DT_SendData(u8 *dataToSend, u8 len)
{
//    NRF_TxPacket(dataToSend, len);
}


