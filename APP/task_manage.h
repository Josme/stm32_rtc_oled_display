#ifndef TASK_MANAGE_H
#define TASK_MANAGE_H
#include "adc.h"
#include "display_menu.h"
#include "oled.h"

typedef struct{
	uint8 *msg_typ;
	uint16 *msg_dat;
}task_param_t;

typedef void (*task_app_t)(task_param_t *task_param);



void task_init(task_app_t);
void task_loop(void);
int8 task_set_app(task_app_t task_app);



#endif
