#include "oled.h"
#include "font.h"
#include "delay.h"
#include "stdio.h"

/****************************
*
*	SCLK:	PA6
*	SDIO:	PA5
*   CS:     NC(Always Low)
*   DC:     PA7
*
****************************/

/** OLED GRAM ���� **/
/** ��������ˢ�»���֮�󣬼�ʹSPIͨ�����ʺ���������£�����ҳ�����ݵ�ˢ�²���������Ļ����˸�� **/
volatile u8 OLED_GRAM[128][8] = {0};		//[��][ҳ]
u8 font_width = 0;

void OLED_CP_Demo()
{
	u16 i = 0;
	u8 j = 0;
	for(j = 0;j < 8;j++){
		OLED_WriteCmd(PAGE_ADDR + j);    				//����ҳ��ַ��0~7����ֻ����ҳѰַģʽ
		for(i = 0;i < 128;i++){									//Ҫ��ͼƬ���ݱ��밴��Ļ���128���ض��룡����
			OLED_WriteData(~BmpDemo_CP[j*128+i]);
		}
	}
}


/**********************
*
*	OLED �ܳ�ʼ��
*
**********************/
void OLED_Init()
{
	
	
	OLED_GPIO_Init();

	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	delay_ms(10);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	
	OLED_WriteCmd(0xAE);			//�ر���ʾ
	
	OLED_WriteCmd(0xD5);			//����ʱ��Ƶ�ʼ���Ƶϵ��
	OLED_WriteCmd(0xF0);			//���þ���Ƶ�ʣ�
	
	OLED_WriteCmd(0xA8);			//����64·���
	OLED_WriteCmd(0x3F);
	
	OLED_WriteCmd(0xD3);			//��ֱƫ������
	OLED_WriteCmd(0x00);
	
	OLED_WriteCmd(0x40);			//��ʾ��ʼ��
	
	OLED_WriteCmd(0x8D);			//��ɱ�����
	OLED_WriteCmd(0x14);			//������ɱ�		���ⲿ7.5V����ʱ���뿪��
	
	OLED_WriteCmd(0x20);			//�����ڴ�Ѱַģʽ
	OLED_WriteCmd(0x02);			//ҳѰַģʽ
//	OLED_WriteCmd(0x00);			//ˮƽѰַģʽ
	
	OLED_WriteCmd(0xA1);			//ˮƽ���Ҿ��� COL127 -> SEG0
//	OLED_WriteCmd(0xA0);			//ˮƽ���Ҳ����� COL0 -> SEG0
	
//	OLED_WriteCmd(0xC0);			//��ֱ˳��ɨ�裬COM0 -> COM63������
	OLED_WriteCmd(0xC8);			//��ֱ˳��ɨ�裬COM63 -> COM0������
	
	OLED_WriteCmd(0xDA);			//COM�������ã����ҽ������ܴ���ʹ����Ч
	OLED_WriteCmd(0x12);			//���Ҳ�������������������
//	OLED_WriteCmd(0x02);			//���Ҳ����������ż�����䣬����ɨ��
	
	OLED_WriteCmd(0x81);			//�Աȶ�����
	OLED_WriteCmd(0xFF);			//7F��Ĭ��ֵ
	
	OLED_WriteCmd(0xD9);			//Ԥ���ʱ������
	OLED_WriteCmd(0xF1);			//��1��1 DCLK����2��15 DCLK
	
	OLED_WriteCmd(0xDB);			//Vcomh����
	OLED_WriteCmd(0x30);			//0 - 0.83xVcc
	
	OLED_WriteCmd(0xA4);			//ȫ�Թ�
	
	OLED_WriteCmd(0xA6);			//�������Ƿ���
	
	OLED_WriteCmd(0xAF);			//��ʾ��

	OLED_Clear();
}

/**********************
*
*	OLED GPIO��ʼ��
*
**********************/
void OLED_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/** ��ʼ��OLED CS DC ���� **/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;								//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  									//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	

}

/**********************
*
*	OLED ģ��SPIдʱ��
*
**********************/
void OLED_WriteByte(u8 dat)
{
	u8 retry=0;				 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)
			return ;
	}			  
//	SPI_I2S_SendData(SPI2, dat); //ͨ������SPIx����һ������
	SPI2->DR = dat;
//	SPIx_ReadWriteByte(dat);
	delay_us(1);				//wait OLED to finish receive
}


/**********************
*
*	OLED д������
*
**********************/
void OLED_WriteCmd(u8 cmd)
{
	OLED_DC = 0;				//ѡ���������ͣ�����
	OLED_WriteByte(cmd);	//д������
}

/**********************
*
*	OLED д������
*
**********************/
void OLED_WriteData(u8 data)
{
	OLED_DC = 1;				//ѡ���������ͣ�����
	OLED_WriteByte(data);	//д������
}

/**********************
*
*	OLED ˢ����ʾ
*
**********************/
void OLED_RefreshGram(void)			//ȫ��Ļˢ�£�����
{
	u8 i,n;
	
//	OLED_WriteCmd(COL_ADD);									//������ʼ/�����У�ֻ����ˮƽ/��ֱѰַģʽ
//	OLED_WriteCmd(0x00);
//	OLED_WriteCmd(0x7F);
	
	for(i = 0;i < 8;i++)
	{
		OLED_WriteCmd(PAGE_ADDR + i);    				//����ҳ��ַ��0~7����ֻ����ҳѰַģʽ
//		OLED_WriteCmd(COL_ADD_L + 0);     		//������ʾλ�á��е�ַ�Ͱ��ֽ�
//		OLED_WriteCmd(COL_ADD_H + 0);      		//������ʾλ�á��е�ַ�߰��ֽ�
		for(n = 0;n < 128;n++){
			OLED_WriteData(OLED_GRAM[n][i]);
		}
	}
}

/**********************
*
*	OLED �����ʾ��ȫ��
*
**********************/
void OLED_Clear(void)
{
	u8 i,n;

	for(i = 0;i < 8;i++)				//GRAMҳ
		for(n = 0;n < 128;n++){			//GRAM��
			OLED_GRAM[n][i] = 0X00;  	//����GRAM�����0
		}
	OLED_RefreshGram();					//������ʾ
}

/**********************
*
*	OLED ������ʾ��DCDC
*
**********************/
void OLED_DiplayOn(void)
{
	OLED_WriteCmd(0x8D);		//SET DCDC
	OLED_WriteCmd(0x14);		//DCDC ON
	OLED_WriteCmd(0xAF);		//Display ON
}

/**********************
*
*	OLED �ر���ʾ��DCDC
*
**********************/
void OLED_DiplayOff(void)
{
	OLED_WriteCmd(0x8D);		//SET DCDC
	OLED_WriteCmd(0x10);		//DCDC ON
	OLED_WriteCmd(0xAE);		//Display OFF
}

/**********************
*
*	OLED �趨λ�ã��У�ҳ
*
**********************/
void OLED_SetPos(u8 x, u8 y)
{
	OLED_WriteCmd(PAGE_ADDR + y / 8);
	OLED_WriteCmd(COL_ADD_L + ( x & 0x0F));
	OLED_WriteCmd(COL_ADD_H + ((x & 0xF0) >> 4));
}

/**********************
*
*	OLED ����
*	u8 dat:����ɫ
*
**********************/
void OLED_DrawPoint(u8 x, u8 y, u8 dat)
{
	u8 page  = 0x00;
	u8 dot   = 0x00;
	
	if(x > 127 || y > 63)		//������Χ
		return;

	page = y / 8;				//�õ�ҳ��
	dot  = y % 8;				//�õ���ֱλ��
	dot  = 0x01 << (dot);		//���ɵ�ǰҳ��һ����ʾ����
	
	if((OLED_GRAM[x][page] & dot) == dat)	//����͵�ǰ��ʾһ�£��������д��ֱ���˳����˲�����������ʾ��˸
		return;

	if(dat)
		OLED_GRAM[x][page] |= dot;
	else
		OLED_GRAM[x][page] &= (~dot);

	OLED_SetPos(x,y);						//��λ�У�X����ҳ��Y��
	OLED_WriteData(OLED_GRAM[x][page]);		//ֻ��д������λ��
}

/**********************
*
*	OLED �������
*	u8 dot:�����ɫ
*
**********************/
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u8 x = 0,y = 0;

	for(x = x1;x <= x2;x++)				
		for(y = y1;y <= y2;y++)			//�Ȼ���ֱ����
			OLED_DrawPoint(x,y,dot);
}

/**********************
*
*	OLED ��ʾһ���ַ�
*	size:	�ַ���С��12/16
*	 dot:	�ַ���ɫ
*
**********************/
void OLED_ShowChar(u8 x,u8 y,char chr,u8 size,u8 dot)
{
	u8 i,j,temp = 0;

	chr = chr - ' ';					//��������ƫ����
	for(i = 0;i < size;i++)				//�����Ǻ���ɨ�裬�������󣬴��ϵ���
	{
		if(size == 7){
			temp = asc2_0507[chr][i];
			font_width = FONT_WID;
		}
		else if(size == 16){
			temp = asc2_1608[chr][i];
			font_width = FONT_WID;
		}
		else{
			temp = asc2_1206[chr][i];
			font_width = FONT_WID;
		}

		x += font_width;						//���������Ǵ�������ɨ��
		
		for(j = 0;j < font_width;j++)			//
		{
			if(temp & 0x80)
				OLED_DrawPoint( x, y, dot);
			else
				OLED_DrawPoint( x, y, !dot);

			temp <<= 1;
			x--;		//�����У������Ǵ����������ɵ�
		}
		y++;			//���»���
	}
}

/**********************
*
*	OLED ��ʾ�ַ���
*	size:	�����С
*	 dot:	�ַ���ɫ
*
**********************/
void OLED_ShowString(u8 x,u8 y,char *str,u8 size,u8 dot)
{
	while(*str)
	{
		OLED_ShowChar(x,y,*str,size,dot);
		
		x += FONT_WID;						//��һ�ַ�λ��
		str++;								
		
		if(x > 127 - 8)						//��ʾԽ�����˳�
			return;
	}
}

/**********************
*
*	OLED ��ʾ��ֵ��u16
*	 num:	����ʾ��ֵ
*	size:	�����С
*	 len:	��ʾλ��������ʹ�ÿո���䣬������ֵ����
*	 dot:	�ַ���ɫ
*
**********************/
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 size,u8 len,u8 dot)
{
    u16 temp = 0;

//	OLED_Fill(x,y,x + 3*8,y+size-1,0);									//ʹ������ˢ����ʾ֮�󣬲�����ձ�����

	temp = num % 10;
	OLED_ShowChar(x,y,'0' + temp,size,dot);				//��λ
	
	if((num > 9 || len > 1) && (x >= FONT_WID)){
		temp = num % 100 / 10;
		OLED_ShowChar(x - FONT_WID,y,'0' + temp,size,dot);			//ʮλ
		if((num > 99 || len > 2) && (x >= 2*FONT_WID)){
			temp = num % 1000 / 100;
			OLED_ShowChar(x - 2*FONT_WID,y,'0' + temp,size,dot);			//��λ
			
			if((num > 999 || len > 3) && (x >= 3*FONT_WID)){
				temp = num % 10000 / 1000;
				OLED_ShowChar(x - 3*FONT_WID, y, '0' + temp, size, dot);			//ǧλ
			}
		
		}
	}
}

void OLED_ShowHex(u8 x, u8 y, u32 hex, u8 size)
{
	char buffer[100];
	
	sprintf(buffer,"%X",hex);
	
	OLED_ShowString(x, y, buffer, size, 1);
}
