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

/** OLED GRAM 缓存 **/
/** 引入增量刷新机制之后，即使SPI通信速率很慢的情况下，对于页面数据的刷新不会引起屏幕的闪烁感 **/
volatile u8 OLED_GRAM[128][8] = {0};		//[列][页]
u8 font_width = 0;

void OLED_CP_Demo()
{
	u16 i = 0;
	u8 j = 0;
	for(j = 0;j < 8;j++){
		OLED_WriteCmd(PAGE_ADDR + j);    				//设置页地址（0~7），只用于页寻址模式
		for(i = 0;i < 128;i++){									//要求图片数据必须按屏幕宽度128像素对齐！！！
			OLED_WriteData(~BmpDemo_CP[j*128+i]);
		}
	}
}


/**********************
*
*	OLED 总初始化
*
**********************/
void OLED_Init()
{
	
	
	OLED_GPIO_Init();

	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	delay_ms(10);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	
	OLED_WriteCmd(0xAE);			//关闭显示
	
	OLED_WriteCmd(0xD5);			//设置时钟频率及分频系数
	OLED_WriteCmd(0xF0);			//设置晶振频率？
	
	OLED_WriteCmd(0xA8);			//设置64路输出
	OLED_WriteCmd(0x3F);
	
	OLED_WriteCmd(0xD3);			//垂直偏移行数
	OLED_WriteCmd(0x00);
	
	OLED_WriteCmd(0x40);			//显示起始行
	
	OLED_WriteCmd(0x8D);			//电荷泵设置
	OLED_WriteCmd(0x14);			//开启电荷泵		无外部7.5V供电时必须开启
	
	OLED_WriteCmd(0x20);			//设置内存寻址模式
	OLED_WriteCmd(0x02);			//页寻址模式
//	OLED_WriteCmd(0x00);			//水平寻址模式
	
	OLED_WriteCmd(0xA1);			//水平左右镜像 COL127 -> SEG0
//	OLED_WriteCmd(0xA0);			//水平左右不镜像 COL0 -> SEG0
	
//	OLED_WriteCmd(0xC0);			//垂直顺序扫描，COM0 -> COM63，向上
	OLED_WriteCmd(0xC8);			//垂直顺序扫描，COM63 -> COM0，向下
	
	OLED_WriteCmd(0xDA);			//COM引脚设置，左右交换功能错误，使能无效
	OLED_WriteCmd(0x12);			//左右不交换，引脚连续分配
//	OLED_WriteCmd(0x02);			//左右不交换，引脚间隔分配，隔行扫描
	
	OLED_WriteCmd(0x81);			//对比度设置
	OLED_WriteCmd(0xFF);			//7F：默认值
	
	OLED_WriteCmd(0xD9);			//预充电时间设置
	OLED_WriteCmd(0xF1);			//相1：1 DCLK，相2：15 DCLK
	
	OLED_WriteCmd(0xDB);			//Vcomh设置
	OLED_WriteCmd(0x30);			//0 - 0.83xVcc
	
	OLED_WriteCmd(0xA4);			//全显关
	
	OLED_WriteCmd(0xA6);			//正常，非反显
	
	OLED_WriteCmd(0xAF);			//显示开

	OLED_Clear();
}

/**********************
*
*	OLED GPIO初始化
*
**********************/
void OLED_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/** 初始化OLED CS DC 引脚 **/
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
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  	//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;								//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  									//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	

}

/**********************
*
*	OLED 模拟SPI写时序
*
**********************/
void OLED_WriteByte(u8 dat)
{
	u8 retry=0;				 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>200)
			return ;
	}			  
//	SPI_I2S_SendData(SPI2, dat); //通过外设SPIx发送一个数据
	SPI2->DR = dat;
//	SPIx_ReadWriteByte(dat);
	delay_us(1);				//wait OLED to finish receive
}


/**********************
*
*	OLED 写入命令
*
**********************/
void OLED_WriteCmd(u8 cmd)
{
	OLED_DC = 0;				//选择数据类型：命令
	OLED_WriteByte(cmd);	//写入命令
}

/**********************
*
*	OLED 写入数据
*
**********************/
void OLED_WriteData(u8 data)
{
	OLED_DC = 1;				//选择数据类型：数据
	OLED_WriteByte(data);	//写入数据
}

/**********************
*
*	OLED 刷新显示
*
**********************/
void OLED_RefreshGram(void)			//全屏幕刷新，较慢
{
	u8 i,n;
	
//	OLED_WriteCmd(COL_ADD);									//设置起始/结束列，只用于水平/垂直寻址模式
//	OLED_WriteCmd(0x00);
//	OLED_WriteCmd(0x7F);
	
	for(i = 0;i < 8;i++)
	{
		OLED_WriteCmd(PAGE_ADDR + i);    				//设置页地址（0~7），只用于页寻址模式
//		OLED_WriteCmd(COL_ADD_L + 0);     		//设置显示位置—列地址低半字节
//		OLED_WriteCmd(COL_ADD_H + 0);      		//设置显示位置—列地址高半字节
		for(n = 0;n < 128;n++){
			OLED_WriteData(OLED_GRAM[n][i]);
		}
	}
}

/**********************
*
*	OLED 清除显示，全黑
*
**********************/
void OLED_Clear(void)
{
	u8 i,n;

	for(i = 0;i < 8;i++)				//GRAM页
		for(n = 0;n < 128;n++){			//GRAM列
			OLED_GRAM[n][i] = 0X00;  	//更新GRAM，填充0
		}
	OLED_RefreshGram();					//更新显示
}

/**********************
*
*	OLED 开启显示及DCDC
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
*	OLED 关闭显示及DCDC
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
*	OLED 设定位置，列，页
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
*	OLED 画点
*	u8 dat:点颜色
*
**********************/
void OLED_DrawPoint(u8 x, u8 y, u8 dat)
{
	u8 page  = 0x00;
	u8 dot   = 0x00;
	
	if(x > 127 || y > 63)		//超出范围
		return;

	page = y / 8;				//得到页数
	dot  = y % 8;				//得到垂直位置
	dot  = 0x01 << (dot);		//生成当前页内一列显示数据
	
	if((OLED_GRAM[x][page] & dot) == dat)	//如果和当前显示一致，则无需改写，直接退出当此操作，消除显示闪烁
		return;

	if(dat)
		OLED_GRAM[x][page] |= dot;
	else
		OLED_GRAM[x][page] &= (~dot);

	OLED_SetPos(x,y);						//定位列（X），页（Y）
	OLED_WriteData(OLED_GRAM[x][page]);		//只改写点所在位置
}

/**********************
*
*	OLED 矩形填充
*	u8 dot:填充颜色
*
**********************/
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u8 x = 0,y = 0;

	for(x = x1;x <= x2;x++)				
		for(y = y1;y <= y2;y++)			//先画垂直方向
			OLED_DrawPoint(x,y,dot);
}

/**********************
*
*	OLED 显示一个字符
*	size:	字符大小：12/16
*	 dot:	字符颜色
*
**********************/
void OLED_ShowChar(u8 x,u8 y,char chr,u8 size,u8 dot)
{
	u8 i,j,temp = 0;

	chr = chr - ' ';					//计算字体偏移量
	for(i = 0;i < size;i++)				//字体是横向扫描，从右往左，从上到下
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

		x += font_width;						//所用字体是从右往左扫描
		
		for(j = 0;j < font_width;j++)			//
		{
			if(temp & 0x80)
				OLED_DrawPoint( x, y, dot);
			else
				OLED_DrawPoint( x, y, !dot);

			temp <<= 1;
			x--;		//向左换列，字体是从右往左生成的
		}
		y++;			//向下换行
	}
}

/**********************
*
*	OLED 显示字符串
*	size:	字体大小
*	 dot:	字符颜色
*
**********************/
void OLED_ShowString(u8 x,u8 y,char *str,u8 size,u8 dot)
{
	while(*str)
	{
		OLED_ShowChar(x,y,*str,size,dot);
		
		x += FONT_WID;						//下一字符位置
		str++;								
		
		if(x > 127 - 8)						//显示越界则退出
			return;
	}
}

/**********************
*
*	OLED 显示数值：u16
*	 num:	待显示数值
*	size:	字体大小
*	 len:	显示位数，不足使用空格填充，方便数值对齐
*	 dot:	字符颜色
*
**********************/
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 size,u8 len,u8 dot)
{
    u16 temp = 0;

//	OLED_Fill(x,y,x + 3*8,y+size-1,0);									//使用增量刷新显示之后，不必清空背景了

	temp = num % 10;
	OLED_ShowChar(x,y,'0' + temp,size,dot);				//个位
	
	if((num > 9 || len > 1) && (x >= FONT_WID)){
		temp = num % 100 / 10;
		OLED_ShowChar(x - FONT_WID,y,'0' + temp,size,dot);			//十位
		if((num > 99 || len > 2) && (x >= 2*FONT_WID)){
			temp = num % 1000 / 100;
			OLED_ShowChar(x - 2*FONT_WID,y,'0' + temp,size,dot);			//百位
			
			if((num > 999 || len > 3) && (x >= 3*FONT_WID)){
				temp = num % 10000 / 1000;
				OLED_ShowChar(x - 3*FONT_WID, y, '0' + temp, size, dot);			//千位
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
