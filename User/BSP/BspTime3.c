#include "stm32f10x.h"
#include "BspTime3.h"

#define IRQHANDLENUM    1
static void (*IRQHandler[IRQHANDLENUM])(void) = {NULL};

/*******************************************************************************
* Function Name :void BspTim3Init(void)
* Description   :通用定时器 初始化，
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
void BspTim3Init(void)
{
    NVIC_InitTypeDef NvicInitdef;
    TIM_TimeBaseInitTypeDef timbase;	
    u16 cnt = 1000;     //170us 触发

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	

    TIM_DeInit(TIM3); 
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Period = cnt - 1;
    timbase.TIM_Prescaler = 2400 - 1; 	        // 1分频
    TIM_TimeBaseInit(TIM3, &timbase);

    NvicInitdef.NVIC_IRQChannel = TIM3_IRQn;
    NvicInitdef.NVIC_IRQChannelPreemptionPriority = 9;
    NvicInitdef.NVIC_IRQChannelSubPriority = 0;
    NvicInitdef.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NvicInitdef);

    TIM_SetCounter(TIM3, 0);
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_ClearITPendingBit( TIM3, TIM_IT_Update);

    /* TIMX enable counter */
    TIM_Cmd(TIM3, DISABLE);
}

/*******************************************************************************
* Function Name :void BspTim3Open()
* Description   :使能能定时器
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3Open(void)
{
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

/*******************************************************************************
* Function Name :void BspTim3Close()
* Description   :关闭定时器
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3Close(void)
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_SetCounter(TIM3, 0);
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
}

/*******************************************************************************
* Function Name :void BspTim3SetCounter(u16 count)
* Description   :设置计数值
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3SetCounter(u16 count)
{
    TIM_SetCounter(TIM3, count);
}

/*******************************************************************************
* Function Name :void BspTim3SetIRQFun(void *fun)
* Description   :定义中断函数
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3SetIRQCallBack(void *fun)
{
    u8 i;
    for(i=0;i<IRQHANDLENUM;i++)
    {
        if(IRQHandler[i] == NULL)
        {
            IRQHandler[i] = (void (*)(void))fun;
            return;
        }
    }
}

/*******************************************************************************
* Function Name :void TIM3_IRQHandler(void)
* Description   :timx中断函数
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
extern volatile uint8_t aaa;

void TIM3_IRQHandler(void)
{
    u8 i;
    
    if( SET == TIM_GetITStatus(TIM3,TIM_IT_Update) )
    {
        TIM_ClearITPendingBit( TIM3, TIM_IT_Update);

#if 1
        for(i=0;i<IRQHANDLENUM;i++)
        {
            if (IRQHandler[i] != NULL)
                (*IRQHandler[i])();
        }
#endif    
    }

        //aaa++;
        //aaa %= 255;
}


/********************** END ***************************************************/

