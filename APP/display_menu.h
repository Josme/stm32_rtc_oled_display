#ifndef DIS_MENU_H
#define DIS_MENU_H
#include "oled.h"
#include "stdint.h"
#include "dw_driver.h"
#include "dw_app.h"

#define NULL 0


typedef void (*disp_proc_t)(void);

typedef struct {

	uint8 	type;			//Default 0x00 --> OLED
	uint8 	color;			//Default 0x00 --> mono
	uint16 	line;
	uint16 	col;
	uint8 	font_width;
	uint8 	font_height;
	uint16 	font_color;
	
}display_param_t;

typedef struct {
	
	uint8 num;
	char* name;
	disp_proc_t app; 
	
}menu_idx_t;

typedef struct{

	uint8 		menu_level;
	char* 		name;
	uint8 		total_idx;
	menu_idx_t*	idx_list;
	uint8 		top_idx;
	uint8 		bot_idx;
	uint8		sel_idx;
	
}menu_t;

typedef struct{

	menu_t* upper_menu;
	menu_t* curnt_menu;
	
	
}disp_ui_t;



extern disp_proc_t disp_page_proc_ptr;
extern disp_proc_t disp_page_key_ptr;

void test(char ,...);


void display_init(void);
void display_param_init(void);
void add_menu(menu_t menu);
void add_menuidx(menu_t menu, menu_idx_t dix);
void disp_main(void);
void disp_callback(uint8 cmd);
void disp_page1_proc(void);
void disp_page2_proc(void);
void disp_page1_key(void);
void disp_page2_key(void);


#endif

