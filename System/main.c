#include "sys.h"
#include "delay.h"
#include "led.h"
#include "tim3.h"
#include "oled.h"
#include "usart.h"
#include "adc.h"
#include "dataTrans.h"
#include "board.h"
#include <math.h>
#include <stdlib.h>

#include "dw_driver.h"
#include "deca_spi.h"
#include "deca_regs.h"
#include "deca_device_api.h"
#include "display_menu.h"
#include "task_manage.h"
#include "dw_app.h"
#include "rtc_ctrl.h"


int main()
{	
	RTC_Init();
	delay_init(72);
	NVIC_Configuration();
	Uart1_Init(115200);
	
	adc_init();

	delay_ms(100);	
	OLED_Init();
  
	
	OLED_Fill(0,0,127,63,1);
	OLED_Fill(1,1,126,62,0);


	OLED_Clear();
		display_init();
	task_init((task_app_t)disp_page_proc_ptr);
TIM3_Init();
	
	while(1)
	{
		task_loop();
	}

}
