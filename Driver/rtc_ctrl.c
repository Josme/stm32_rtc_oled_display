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
    �������ܣ�RTC ��ʼ��������RTC��ֵ
    ����������
    ����ֵ��    
*/
void RTC_Init(void)
{

	      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);        //ʹ��PWR��BKP����ʱ��   
        PWR_BackupAccessCmd(ENABLE);        //ʹ�ܺ󱸼Ĵ������� 
//                RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                //����RTCʱ��(RTCCLK),ѡ��LSI��ΪRTCʱ��    
//                RCC_RTCCLKCmd(ENABLE);        //ʹ��RTCʱ��  
        RCC_LSICmd(ENABLE);
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)
    {
        RTC_Configuration();        //��һ������ ��ʼ������
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
//  RTC_SetCounter(0);          //����ʱ���ֵ
        RTC_EnterConfigMode();
        RTC_SetSec(2018,3,14,17,50,59);
        RTC_WaitForLastTask();  //�ȴ�д�Ĵ������
        RTC_ExitConfigMode();
        BKP_WriteBackupRegister(BKP_DR1, 0x5050);   //д�����ñ�־
    }else
    {
        //RTC_Configuration();
        RTC_WaitForSynchro();       //�ȴ�RTC�Ĵ���ͬ��
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //ʹ��RTC ���ж�
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
    }
    RCC_ClearFlag();    //�����λ��־
    RTC_NVIC_Configuartion();

}
/***********************�ж��Ƿ�Ϊ����********/
const uint8_t monthTable[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
/*
    �������ܣ��ж��Ƿ�����
    ������������Ҫ�жϵ����
    ����ֵ��    ����1�ɹ� ����ʧ��
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
    �������ܣ�����ת��Ϊ�벢����RTC
    �����������ꡢ�¡��ա�ʱ���֡���
    ����ֵ��    ����0�ɹ� ����ʧ��
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
        if (isLeapYear(year) && i==1)   //���� ���һ��
        {
            secCount += 86400UL;
        }
    }
    secCount += (uint32_t)(day-1) * 86400UL;    
    secCount += (uint32_t)(hour)*3600;
    secCount += (uint32_t)(minute)*60;
    secCount += (uint32_t)(sec);
    //printf("%u\n",secCount);

    /*ִ�����²�������ʹ�ܶԱ��ݼĴ����� RTC �ķ��ʣ� 
    1��ͨ�����üĴ��� RCC_APB1ENR �� PWREN �� BKPEN λ���򿪵�Դ�ͺ󱸽ӿڵ�ʱ�� 
    2����Դ���ƼĴ���(PWR_CR)�� DBP λ��ʹ�ܶԺ󱸼Ĵ����� RTC �ķ���
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);
    RTC_WaitForLastTask();
    return 0;
}
/********************************************************************************/
/*
    �������ܣ���RTC��ȡ��ת��Ϊ�ꡢ�¡��ա����ڡ�ʱ���֡���
                        ���� struct thisDate ��
    ����������struct thisDate 
    ����ֵ��    ����0�ɹ� ����ʧ��
*/
uint8_t RTC_GetDate(rtc_struct * thisDate)
{
    uint32_t timeCount = 0;
    uint32_t Count ;
    uint16_t tmp = 0;
    timeCount = RTC_GetCounter();
    Count = timeCount/86400UL;  //���������
    thisDate->week = (4+Count)%7;
    if (Count !=0)                  //����һ��
    {
        tmp = 1970;                     //��1970�꿪ʼ
        while(Count>=365)
        {
            if (isLeapYear(tmp))//������
            {
                if (Count>= 366)    //��������366
                {
                    Count -= 366;
                }else
                {
                    tmp++;                  //����С��366 ���һ��
                    break;
                }
            }else
            {
                Count -= 365;       //ƽ��
            }
                tmp++;
        }
        thisDate->year = tmp;   //��ȡ������
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
                    break;  //���������� ���� ĳʱ
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
        thisDate->month = tmp+1;    //��ȡ����
        thisDate->day = Count+1;    //��ȡ����
    }
    Count = timeCount%86400UL;
    thisDate->hour = Count/3600;
    thisDate->minute = (Count%3600)/60;
    thisDate->sec = (Count%3600)%60;
    return 0;
}