#ifndef _rtc_h_
#define _rtc_h_

//ʱ��ṹ��
typedef struct 
{
    vu8 hour;
    vu8 min;
    vu8 sec;            
    //������������
    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;       
}calendar_obj;          

extern calendar_obj calendar;   //�����ṹ��


void rtcInit(void);
u8 RTC_Get();

#endif
