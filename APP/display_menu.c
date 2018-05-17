#include "display_menu.h"
#include "dw_driver.h"
#include "dw_app.h"
#include "adc.h"
#include "delay.h"
#include "adc.h"
#include "usart.h"
#include "rtc_ctrl.h"
#include "task_manage.h"


display_param_t display_param = {
	0x00,		//oled
	0x01,		//mono
	128,
	64,
	8,			//1206
	12,
	0x01		//mono
};

disp_ui_t main_ui = {
	NULL,
	NULL,
};

uint8 flag_fresh = 1;						//flag,display need refresh
disp_proc_t disp_page_proc_ptr;
disp_proc_t disp_page_key_ptr;

uint8 page_state = 0;

#define LINE(x)	((x)*display_param.font_width)
#define COL(x)	((x)*display_param.font_height)

#define ui_disp_string(line, col, str)		OLED_ShowString(LINE(line), COL(col), str, display_param.font_height,display_param.font_color)
#define ui_disp_num(line, col, num, len)	OLED_ShowNum(LINE(line), COL(col), num, display_param.font_height, len,display_param.font_color)
#define ui_disp_hex(line, col, hex)			OLED_ShowHex(LINE(line), COL(col), hex, display_param.font_height)
#define ui_clr()							OLED_Clear()

/**************** page1 ******************/
menu_idx_t page1_idx[] = {

	{1,"diaplay_time",display_time},
	{2,"set sec",set_sec},
	{3,"set minute",set_minute},
	{4,"set hou",set_hour},
	{5,"set day",set_day},
	{6,"set month",set_month},
	{7,"set year",set_year},
};
uint8 top_idx = 0;
uint8 max_idx = 0;
uint8 sel_idx = 0;

/****************************************/
void display_init(void)
{
	top_idx = 0;		//1-4
	max_idx = 7;
	sel_idx = 0;
	page_state = 1;
	adc_awd_isr = disp_callback;
	disp_page_proc_ptr = disp_page1_proc;
	disp_page_key_ptr = disp_page1_key;
	
}
void display_param_init(void);

/*** to be do, will not execute ***/
void disp_main(void)
{
	disp_page_proc_ptr = disp_page1_proc;
	disp_page_key_ptr = disp_page1_key;
	
	while(1){
		disp_page_proc_ptr();	
	}
}

/*** call back for this module,including display & key process ***/
void disp_callback(uint8 cmd)
{
	switch(cmd){
		case ADC_ISR: disp_page_key_ptr();
		break;
		default:break;
	}
}
uint8 key_val = KEY_UP;
/*** key process for page2 ***/
void disp_page2_key(void)
{
	switch(key_val){
		case KEY_UP:break;
		case KEY_DOWN:break;
		case KEY_LEFT:{
			ui_clr();
					if(!task_set_app((task_app_t)disp_page1_proc)){				//return to page1
						disp_page_key_ptr = disp_page1_key;						//set the key process ptr back to page1
						page_state = 1;
						flag_fresh = 1;
					}
				}break;
		case KEY_RIGHT:break;
		case KEY_CENTER:break;
		default:break;
	}
	key_val = 0;
}

/*** key process for page1 ***/

void disp_page1_key(void)
{
	uint8 sel_loc = 0;
	

	sel_loc = sel_idx - top_idx;
			
	
	switch(key_val){
		case KEY_UP:{
				if(sel_loc == 0){					//selected idx on the top
					if(top_idx > 0){				//still can page up
						sel_idx = --top_idx;
					}
				}else {
					sel_idx--;
				}
				flag_fresh = 1;
				break;
			}
		case KEY_DOWN:{
				if(sel_loc == 3){					//selected idx on the bottom
					if(top_idx < (max_idx - 4)){				//still can page down
						top_idx++;
						sel_idx++;
					}
				}else {
					sel_idx++;
				}
				flag_fresh = 1;
				break;
			}
		case KEY_LEFT:break;
		case KEY_RIGHT:{
				flag_fresh = 0;												//no need to fresh the display
				ui_clr();
				if(!task_set_app((task_app_t)page1_idx[sel_idx].app))		//run selected app & change page
					disp_page_key_ptr = disp_page2_key;						//set the key process ptr to page2
					page_state = 2;
				break;			
			}
		case KEY_CENTER:{													//run selected app & change page
				flag_fresh = 0;
				ui_clr();
				if(!task_set_app((task_app_t)page1_idx[sel_idx].app))
					disp_page_key_ptr = disp_page2_key;						//set the key process ptr to page2
					page_state = 2;
				break;
			}
		default:flag_fresh = 0;break;										//flag  --> do not refresh the display
	}
	key_val = 0;
	
}

/*** ui process for page 1 ***/
void disp_page1_proc(void)
{
	uint8 i = 0;
		
	if(flag_fresh){
		/*** Refresh the display ***/
		display_param.font_color = 1;
		//ui_disp_string(3,0,"APP Choose");
		for(i = 0;i < 4;i++){
			display_param.font_color = 1;								//default dot on
			ui_disp_string(0,i+1,"                ");
			ui_disp_num(0,i+1,page1_idx[top_idx+i].num,1);				//display_param num 
			ui_disp_string(1,i+1,".");
			if((sel_idx - top_idx) == i)								//select line reverse dot
				display_param.font_color = 0;
			ui_disp_string(2,i+1,page1_idx[top_idx+i].name);			//display name
		}
		flag_fresh = 0; 
	}
}

void add_menu(menu_t menu);
void add_menuidx(menu_t menu, menu_idx_t dix);
