#ifndef __OLED_H__
#define __OLED_H__
#include "sys.h"

#define OLED_DC  PBout(12)
#define OLED_CLK PBout(13)
#define OLED_SDI PBout(15)

#define OLED_CMD	0
#define OLED_DATA	1

void OLED_CP_Demo(void);

void OLED_Init(void);
void OLED_GPIO_Init(void);
void OLED_WriteByte(u8 dat);
void OLED_WriteCmd(u8 cmd);
void OLED_WriteData(u8 data);
void OLED_RefreshGram(void);
void OLED_Clear(void);
void OLED_DiplayOn(void);
void OLED_DiplayOff(void);
void OLED_DrawPoint(u8 x, u8 y, u8 dat);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,char chr,u8 size,u8 dot);
void OLED_ShowString(u8 x,u8 y,char *str,u8 size,u8 dot);
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 size,u8 len,u8 dot);
void OLED_ShowHex(u8 x, u8 y, u32 hex, u8 size);

/***			Software Initialization
*
*	1.Set MUX Ratio: 							A8H -> 3FH
*	2.Set Display Offset:						D3H -> 00H
*	3.Set Display Start Line:					40H
*	4.Set Segment Re-map:						A0H/A1H
*	5.Set COM Output Scan Direction:			C0H/C8H
*	6.Set COM Pins Hardware Configuration:		DAH ->02H
*	7.Set Contrast Control:						81H -> 7FH
*	8.Disable Entire Display On:				A4H
*	9.Set Normal Display:						A6H
*	10.Set Osc Freq:							D5H -> 80H
*	11.Enable Charge Pump Regulator:			8DH -> 14H
*	12.Display On:								AFH
*
*
***/

//1.基本设置寄存器
#define CONTRAST        0x81            //2字节命令，对比度设置
#define DIS_FULL        0xA4            //1字节命令，[:0]全显功能开/关
#define INV_DIS         0xA6            //1字节命令，[:0]正常/反显
#define DIS_ON          0xAE            //1字节命令，[:0]显示开/关

//2.卷屏设置寄存器
#define SCRO_H          0x26            //7字节命令，水平方向卷屏
                                        /*
                                         *  [0:0]-> 0:Right, 1:Left
                                         *  [7:0]-> 00H
                                         *  [2:0]-> 页起始地址
                                         *  [2:0]-> 卷屏时间间隔，帧频率倍数
                                         *  [2:0]-> 页结束地址
                                         *  [7:0]-> 00H
                                         *  [7:0]-> 00H
                                         */

#define SCRO_V          0x28            //6字节命令，水平垂直方向卷屏->0x29,水平方向固定1列卷屏
                                        /*
                                         *  [1:0]-> 01H:向上且向右卷屏，10H:向上且向左卷屏
                                         *  [7:0]-> 00H
                                         *  [2:0]-> 页起始地址
                                         *  [2:0]-> 卷屏时间间隔，帧频率倍数
                                         *  [2:0]-> 页结束地址
                                         *  [5:0]-> 垂直方向卷屏行数:0-63
                                         */

#define SCRO_V_AR       0xA3            //3字节命令，设置垂直卷屏区域
                                        /*
                                         *  [5:0]-> 垂直卷屏偏移行，起始位行数
                                         *  [6:0]-> 用于卷屏的行数
                                         */
#define SCRO_STOP       0x2E            //1字节命令，停止卷屏，设置卷屏前必须先发送停止命令，并且显示RAM需更新
#define SCRO_STAR       0x2F            //1字节命令，开始卷屏，卷屏开始后，RAM数据读写，卷屏参数修改，均被禁止

//3.寻址设置寄存器
#define COL_ADD_L	    0x00			//1字节命令，列起始地址低半字节 [3:0]，只用于页寻址模式
#define COL_ADD_H	    0x10			//1字节命令，列起始地址高半字节 [3:0]，只用于页寻址模式
#define MEM_ADD_MODE    0x20            //2字节命令，GRAM寻址模式
                                        //  [1:0]-> 00:水平寻址模式，01:垂直寻址模式，10:页寻址模式

#define COL_ADD         0x21            //3字节命令，GRAM列起始/结束地址，只用于水平/垂直寻址模式
                                        /*
                                         *  [6:0]-> 起始地址
                                         *  [6:0]-> 结束地址
                                         */

#define PAGE_ADD        0x22            //3字节命令，GRAM页起始/结束地址，只用于页寻址模式
                                        /*
                                         *  [2:0]-> 起始地址
                                         *  [2:0]-> 结束地址
                                         */

#define PAGE_ADDR 	    0xB0			//1字节命令，设置页起始地址 [2:0]: Page0 - Page7，只用于页寻址模式

//4.硬件设置寄存器
#define DIS_COL_STR_ADD 0x40            //1字节命令，设置显示起始行，[5:0]: 0 - 63
#define REVE_H          0xA0            //1字节命令，水平翻转，左右镜像 [0:0]-> 00H:正常，01:镜像
#define MUX_RATIO       0xA8            //2字节命令，设置水平驱动器开启路数, [5:0] > 14
#define COM_DIR			0xC0			//1字节命令，COM引脚顺序方向,N 由MUX_RATIO命令设置
										//	[3:3]-> 0:COM0 -> COM N-1		1:COM N-1 -> COM0
										
#define SHIFT_V         0xD3            //2字节命令，设置垂直偏移行数 [5:0]
#define SET_COM_PIN     0xDA            //2字节命令，COM引脚设置
										/*	
										 *	[1:1]-> 1:必须为1
                                         *  [4:4]-> 0:COM引脚连续重定义，1:COM引脚间隔重定义
                                         *  [5:5]-> 0:COM引脚左右固定，  1:COM引脚左右重定义
                                         */
			
//5.时钟及驱动设置
#define CLK_DIV			0xD5			//2字节命令，时钟频率及分频系数
										/*
										 *	[3:0]-> 分频系数
										 *	[7:4]->	时钟频率
										 */
										 
#define PRE_CHG_T		0xD9			//2字节命令，预充电时间
										/*
										 *	[3:0]->	相1预充电时间，1 - 15 DCLK
										 *	[7:4]->	相2预充电时间，1 - 15 DCLK
										 */
										 
#define SET_VCOM		0xDB			//2字节命令，设置 Vcomh 输出电压
										//	[6:4]-> 00H: 0 - 0.65xVcc	20H: 0 - 0x77xVcc	30H: 0 - 0x83xVcc
										
#define OLED_NOP		0xE3			//1字节命令，空闲操作指令
#endif
