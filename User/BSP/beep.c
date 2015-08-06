#include "beep.h"

void BeepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    bsp_DelayMS(200);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);    
}

void StopBeep(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void Beep(uint8_t ms)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    bsp_StartTimer(2, ms, StopBeep);
}
