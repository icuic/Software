#include "beep.h"

void BeepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    bsp_DelayMS(200);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);    
}

void StopBeep(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

void Beep(uint8_t ms)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_0);

    bsp_StartTimer(M_SOFT_TIMER_FOR_BEEP, ms, StopBeep);
}
