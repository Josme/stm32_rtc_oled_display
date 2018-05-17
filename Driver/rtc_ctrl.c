#include "rtc_ctrl.h"
#include "oled.h"
 rtc_struct pDate;



/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void RTC_NVIC_Configuartion(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(40000);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
void RTC_IRQHandler(void)
{ 
	 uint8_t dis_buf[20];
    if (RTC_GetITStatus(RTC_IT_SEC) == SET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
        RTC_GetDate(&pDate);
        //printf("%d-%2d-%2d-%d\t%02d:%02d:%02d\n", pDate.year, pDate.month, pDate.day, pDate.week, pDate.hour, pDate.minute, pDate.sec);
        sprintf(dis_buf,"%d", pDate.year);
			  OLED_ShowString(0,0,dis_buf,16,1);
			  sprintf(dis_buf,"%d-%2d-%d%",pDate.month, pDate.day, pDate.week);
			  OLED_ShowString(0,16,dis_buf,16,1);
				sprintf(dis_buf,"%2d:%02d:%02d", pDate.hour, pDate.minute, pDate.sec);
			  OLED_ShowString(0,32,dis_buf,16,1);
		}
}
/******************************************************/
/*
    函数功能：RTC 初始化并设置RTC初值
    函数参数：
    返回值：    
*/
void RTC_Init(void)
{

	      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);        //使能PWR和BKP外设时钟   
        PWR_BackupAccessCmd(ENABLE);        //使能后备寄存器访问 
//                RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                //设置RTC时钟(RTCCLK),选择LSI作为RTC时钟    
//                RCC_RTCCLKCmd(ENABLE);        //使能RTC时钟  
        RCC_LSICmd(ENABLE);
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)
    {
        RTC_Configuration();        //第一次运行 初始化设置
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
//  RTC_SetCounter(0);          //设置时间初值
        RTC_EnterConfigMode();
        RTC_SetSec(2018,3,14,17,50,59);
        RTC_WaitForLastTask();  //等待写寄存器完成
        RTC_ExitConfigMode();
        BKP_WriteBackupRegister(BKP_DR1, 0x5050);   //写入配置标志
    }else
    {
        //RTC_Configuration();
        RTC_WaitForSynchro();       //等待RTC寄存器同步
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //使能RTC 秒中断
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
    }
    RCC_ClearFlag();    //清除复位标志
    RTC_NVIC_Configuartion();

}
/***********************判断是否为闰年********/
const uint8_t monthTable[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
/*
    函数功能：判断是否闰年
    函数参数：需要判断的年份
    返回值：    返回1成功 其他失败
*/
static uint8_t isLeapYear(uint16_t mYear)   
{
    if (((mYear%4 == 0)&&((mYear%100) != 0))||(mYear%400) == 0)
    {
        return 1;
    }else 
    {
        return 0;
    }
}
/********************************************************************************/
/*
    函数功能：日期转换为秒并配置RTC
    函数参数：年、月、日、时、分、秒
    返回值：    返回0成功 其他失败
*/
uint8_t RTC_SetSec(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec)
{
    uint32_t secCount = 0;
    uint16_t i;

    if (year <1970 || year >2099)
    {
        return 1;
    }
    for (i=1970; i<year; i++)
    {
        if (isLeapYear(i))
        {
            secCount += 31622400UL;
        }
        else
        {
            secCount += 31536000UL;
        }
    }

    for (i=0; i<month-1; i++)
    {
        secCount += ((uint32_t)monthTable[i]*86400UL);
        if (isLeapYear(year) && i==1)   //润年 多加一天
        {
            secCount += 86400UL;
        }
    }
    secCount += (uint32_t)(day-1) * 86400UL;    
    secCount += (uint32_t)(hour)*3600;
    secCount += (uint32_t)(minute)*60;
    secCount += (uint32_t)(sec);
    //printf("%u\n",secCount);

    /*执行以下操作可以使能对备份寄存器和 RTC 的访问： 
    1）通过设置寄存器 RCC_APB1ENR 的 PWREN 和 BKPEN 位来打开电源和后备接口的时钟 
    2）电源控制寄存器(PWR_CR)的 DBP 位来使能对后备寄存器和 RTC 的访问
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);
    RTC_WaitForLastTask();
    return 0;
}
/********************************************************************************/
/*
    函数功能：从RTC获取秒转换为年、月、日、星期、时、分、秒
                        存入 struct thisDate 中
    函数参数：struct thisDate 
    返回值：    返回0成功 其他失败
*/
uint8_t RTC_GetDate(rtc_struct * thisDate)
{
    uint32_t timeCount = 0;
    uint32_t Count ;
    uint16_t tmp = 0;
    timeCount = RTC_GetCounter();
    Count = timeCount/86400UL;  //计算出天数
    thisDate->week = (4+Count)%7;
    if (Count !=0)                  //大于一天
    {
        tmp = 1970;                     //从1970年开始
        while(Count>=365)
        {
            if (isLeapYear(tmp))//是闰年
            {
                if (Count>= 366)    //天数大于366
                {
                    Count -= 366;
                }else
                {
                    tmp++;                  //天数小于366 最后一年
                    break;
                }
            }else
            {
                Count -= 365;       //平年
            }
                tmp++;
        }
        thisDate->year = tmp;   //获取到了年
        tmp = 0;
        while(Count>= 28)
        {
            if (isLeapYear(thisDate->year) && tmp == 1)
            {
                if (Count >= 29) 
                {
                    Count -= 29;
                }
                else    
                {
                    break;  //当年是闰年 二月 某时
                }
            }else
            {
                if (Count >= monthTable[tmp]) 
                {
                    Count -= monthTable[tmp];
                }else
                {
                    break;
                }
            }
                tmp ++;
        }
        thisDate->month = tmp+1;    //获取到月
        thisDate->day = Count+1;    //获取到日
    }
    Count = timeCount%86400UL;
    thisDate->hour = Count/3600;
    thisDate->minute = (Count%3600)/60;
    thisDate->sec = (Count%3600)%60;
    return 0;
}