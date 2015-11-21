/*
*********************************************************************************************************
*                                     
*   模块名称 : 定时器模块
*   文件名称 : bsp_timer.h
*   版    本 : V2.0
*   说    明 : 头文件
*
*   Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

/* 目前是空操作，用户可以定义让CPU进入IDLE状态的函数和喂狗函数 */
#define CPU_IDLE()

typedef void (*pFun)(void);

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
    volatile uint32_t count;
    volatile uint8_t flag;
    pFun fTimeout;
}SOFT_TMR;

#define M_SOFT_TIMER_FOR_MENU   1
#define M_SOFT_TIMER_FOR_BEEP   2
#define M_SOFT_TIMER_FOR_OPEN   3   // Open box one by one


/* 供外部调用的函数声明 */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period,  void(*pf));
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);

#endif
