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

//1.�������üĴ���
#define CONTRAST        0x81            //2�ֽ�����Աȶ�����
#define DIS_FULL        0xA4            //1�ֽ����[:0]ȫ�Թ��ܿ�/��
#define INV_DIS         0xA6            //1�ֽ����[:0]����/����
#define DIS_ON          0xAE            //1�ֽ����[:0]��ʾ��/��

//2.�������üĴ���
#define SCRO_H          0x26            //7�ֽ����ˮƽ�������
                                        /*
                                         *  [0:0]-> 0:Right, 1:Left
                                         *  [7:0]-> 00H
                                         *  [2:0]-> ҳ��ʼ��ַ
                                         *  [2:0]-> ����ʱ������֡Ƶ�ʱ���
                                         *  [2:0]-> ҳ������ַ
                                         *  [7:0]-> 00H
                                         *  [7:0]-> 00H
                                         */

#define SCRO_V          0x28            //6�ֽ����ˮƽ��ֱ�������->0x29,ˮƽ����̶�1�о���
                                        /*
                                         *  [1:0]-> 01H:���������Ҿ�����10H:�������������
                                         *  [7:0]-> 00H
                                         *  [2:0]-> ҳ��ʼ��ַ
                                         *  [2:0]-> ����ʱ������֡Ƶ�ʱ���
                                         *  [2:0]-> ҳ������ַ
                                         *  [5:0]-> ��ֱ�����������:0-63
                                         */

#define SCRO_V_AR       0xA3            //3�ֽ�������ô�ֱ��������
                                        /*
                                         *  [5:0]-> ��ֱ����ƫ���У���ʼλ����
                                         *  [6:0]-> ���ھ���������
                                         */
#define SCRO_STOP       0x2E            //1�ֽ����ֹͣ���������þ���ǰ�����ȷ���ֹͣ���������ʾRAM�����
#define SCRO_STAR       0x2F            //1�ֽ������ʼ������������ʼ��RAM���ݶ�д�����������޸ģ�������ֹ

//3.Ѱַ���üĴ���
#define COL_ADD_L	    0x00			//1�ֽ��������ʼ��ַ�Ͱ��ֽ� [3:0]��ֻ����ҳѰַģʽ
#define COL_ADD_H	    0x10			//1�ֽ��������ʼ��ַ�߰��ֽ� [3:0]��ֻ����ҳѰַģʽ
#define MEM_ADD_MODE    0x20            //2�ֽ����GRAMѰַģʽ
                                        //  [1:0]-> 00:ˮƽѰַģʽ��01:��ֱѰַģʽ��10:ҳѰַģʽ

#define COL_ADD         0x21            //3�ֽ����GRAM����ʼ/������ַ��ֻ����ˮƽ/��ֱѰַģʽ
                                        /*
                                         *  [6:0]-> ��ʼ��ַ
                                         *  [6:0]-> ������ַ
                                         */

#define PAGE_ADD        0x22            //3�ֽ����GRAMҳ��ʼ/������ַ��ֻ����ҳѰַģʽ
                                        /*
                                         *  [2:0]-> ��ʼ��ַ
                                         *  [2:0]-> ������ַ
                                         */

#define PAGE_ADDR 	    0xB0			//1�ֽ��������ҳ��ʼ��ַ [2:0]: Page0 - Page7��ֻ����ҳѰַģʽ

//4.Ӳ�����üĴ���
#define DIS_COL_STR_ADD 0x40            //1�ֽ����������ʾ��ʼ�У�[5:0]: 0 - 63
#define REVE_H          0xA0            //1�ֽ����ˮƽ��ת�����Ҿ��� [0:0]-> 00H:������01:����
#define MUX_RATIO       0xA8            //2�ֽ��������ˮƽ����������·��, [5:0] > 14
#define COM_DIR			0xC0			//1�ֽ����COM����˳����,N ��MUX_RATIO��������
										//	[3:3]-> 0:COM0 -> COM N-1		1:COM N-1 -> COM0
										
#define SHIFT_V         0xD3            //2�ֽ�������ô�ֱƫ������ [5:0]
#define SET_COM_PIN     0xDA            //2�ֽ����COM��������
										/*	
										 *	[1:1]-> 1:����Ϊ1
                                         *  [4:4]-> 0:COM���������ض��壬1:COM���ż���ض���
                                         *  [5:5]-> 0:COM�������ҹ̶���  1:COM���������ض���
                                         */
			
//5.ʱ�Ӽ���������
#define CLK_DIV			0xD5			//2�ֽ����ʱ��Ƶ�ʼ���Ƶϵ��
										/*
										 *	[3:0]-> ��Ƶϵ��
										 *	[7:4]->	ʱ��Ƶ��
										 */
										 
#define PRE_CHG_T		0xD9			//2�ֽ����Ԥ���ʱ��
										/*
										 *	[3:0]->	��1Ԥ���ʱ�䣬1 - 15 DCLK
										 *	[7:4]->	��2Ԥ���ʱ�䣬1 - 15 DCLK
										 */
										 
#define SET_VCOM		0xDB			//2�ֽ�������� Vcomh �����ѹ
										//	[6:4]-> 00H: 0 - 0.65xVcc	20H: 0 - 0x77xVcc	30H: 0 - 0x83xVcc
										
#define OLED_NOP		0xE3			//1�ֽ�������в���ָ��
#endif
