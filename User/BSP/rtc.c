#include "stm32f10x.h"
#include "rtc.h"

calendar_obj calendar;//ʱ�ӽṹ��

//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�                                             
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�     

const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31}; //ƽ����·����ڱ�


//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//year:���
//����ֵ:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{             
    if(year%4==0) //�����ܱ�4����
    { 
        if(year%100==0) 
        { 
            if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400����     
            else return 0;   
        }
        else return 1;   
    }
    else return 0;  
} 

//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//year,month,day������������ 
//����ֵ�����ں�                                                                                         
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{   
    u16 temp2;
    u8 yearH,yearL;
    
    yearH=year/100; yearL=year%100; 
    // ���Ϊ21����,�������100  
    if (yearH>19)yearL+=100;
    // ����������ֻ��1900��֮���  
    temp2=yearL+yearL/4;
    temp2=temp2%7; 
    temp2=temp2+day+table_week[month-1];
    if (yearL%4==0&&month<3)temp2--;
    return(temp2%7);
}


//�õ���ǰ��ʱ�䣬���������calendar�ṹ������
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get()
{
    static u16 daycnt=0;
    u32 timecount=0; 
    u32 temp=0;
    u16 temp1=0;      
    timecount=RTC->CNTH;//�õ��������е�ֵ(������)
    timecount<<=16;
    timecount+=RTC->CNTL;            

    temp=timecount/86400;   //�õ�����(��������Ӧ��)
    if(daycnt!=temp)//����һ����
    {     
        daycnt=temp;
        temp1=1970; //��1970�꿪ʼ
        while(temp>=365)
        {                
            if(Is_Leap_Year(temp1))//������
            {
                if(temp>=366)temp-=366;//�����������
                else {temp1++;break;}  
            }
            else temp-=365;   //ƽ�� 
            temp1++;  
        }   
        calendar.w_year=temp1;//�õ����
        temp1=0;
        while(temp>=28)//������һ����
        {
            if(Is_Leap_Year(calendar.w_year)&&temp1==1)//�����ǲ�������/2�·�
            {
                if(temp>=29)temp-=29;//�����������
                else break; 
            }
            else 
            {
                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
                else break;
            }
            temp1++;  
        }
        calendar.w_month=temp1+1;   //�õ��·�
        calendar.w_date=temp+1;     //�õ����� 
    }
    temp=timecount%86400;           //�õ�������       
    calendar.hour=temp/3600;        //Сʱ
    calendar.min=(temp%3600)/60;    //����  
    calendar.sec=(temp%3600)%60;    //����
    calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//��ȡ����   
    return 0;
}


//syear,smon,sday,hour,min,sec��������ʱ����
//����ֵ�����ý����0���ɹ���1��ʧ�ܡ�
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
    if(syear<1970||syear>2099)return 1;    
    for(t=1970;t<syear;t++) //��������ݵ��������
    {
        if(Is_Leap_Year(t))seccount+=31622400;//�����������
        else seccount+=31536000;              //ƽ���������
    }
    smon-=1;
    for(t=0;t<smon;t++)    //��ǰ���·ݵ����������
    {
        seccount+=(u32)mon_table[t]*86400;//�·����������
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������       
    }
    seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
    seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;   //����������
    seccount+=sec;//�������Ӽ���ȥ
                                                        
    //����ʱ��
    RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
    RCC->APB1ENR|=1<<27;//ʹ�ܱ���ʱ��
    PWR->CR|=1<<8;    //ȡ��������д����
    //���������Ǳ����!
    RTC->CRL|=1<<4;   //�������� 
    RTC->CNTL=seccount&0xffff;
    RTC->CNTH=seccount>>16;
    RTC->CRL&=~(1<<4);//���ø���
    while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ���������� 
    RTC_Get();//������֮�����һ������  
    return 0;       
}


/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void rtc_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

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
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(0);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}



void rtcInit(void)
{
    //rtc_NVIC_Configuration();

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

    //printf("\r\n\n RTC not yet configured....");

    /* RTC Configuration */
    RTC_Configuration();

    //printf("\r\n RTC configured....");

    /* Adjust time by values entred by the user on the hyperterminal */
    //Time_Adjust();

    RTC_Set(2015,8,5,21,3,55);  //����ʱ�� 

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);

    //DisplayStr("Not init", 0, 0);
  }
  else
  {
    //DisplayStr("Alread init", 0, 0);
  
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      //printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      //printf("\r\n\n External Reset occurred....");
    }

    //printf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, DISABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    RTC_Get();//����ʱ�� 
  }
}
