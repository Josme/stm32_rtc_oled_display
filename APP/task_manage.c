#include "task_manage.h"
#include "usart.h"
#include "stdio.h"

task_app_t task_main_ptr;
task_param_t* task_param_ptr;

/*** set the default app ***/
void task_init(task_app_t task_app)
{
	if(task_app != NULL)
		task_main_ptr = task_app;
	else{
		printf("no defult app has set.\r\n");
	}
}

/*** set the main loop task ptr ***/
int8 task_set_app(task_app_t task_app)
{
	int8 sta = 0;
	
	if(task_app != NULL)
		task_main_ptr = task_app;
	else
		sta = -1;
	return sta;
}

/*** the main task loop ***/
void task_loop(void)
{
	while(1){
		if(task_main_ptr != NULL)
			task_main_ptr(task_param_ptr);
		else{
			printf("no app to run, stack here and waite for app to insert.\r\n");
		}
	}
}
