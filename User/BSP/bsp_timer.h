/*
*********************************************************************************************************
*                                     
*   ģ������ : ��ʱ��ģ��
*   �ļ����� : bsp_timer.h
*   ��    �� : V2.0
*   ˵    �� : ͷ�ļ�
*
*   Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

/* Ŀǰ�ǿղ������û����Զ�����CPU����IDLE״̬�ĺ�����ι������ */
#define CPU_IDLE()

typedef void (*pFun)(void);

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
    volatile uint32_t count;
    volatile uint8_t flag;
    pFun fTimeout;
}SOFT_TMR;

#define M_SOFT_TIMER_FOR_MENU   1
#define M_SOFT_TIMER_FOR_BEEP   2
#define M_SOFT_TIMER_FOR_OPEN   3   // Open box one by one


/* ���ⲿ���õĺ������� */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period,  void(*pf));
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);

#endif
