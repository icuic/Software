#include "stm32f10x.h"
#include "bsp.h"


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
    ErrorStatus HSEStartUpStatus = SUCCESS;
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();
    
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)  
    {
    
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        
        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);
        
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 
        
        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 
        
        /* PCLK1 = HCLK */
        RCC_PCLK1Config(RCC_HCLK_Div1);

        /* PLLCLK = 4MHz * 6 = 24 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);
        /* Enable PLL */ 
        RCC_PLLCmd(ENABLE);
        
        /* Wait till PLL is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }
        
        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
}


/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                     RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                     RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,DISABLE);
    
}

/*******************************************************************************
* Function Name :void InterruptOrder(void)
* Description   :
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  11:50:05
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
    
    /* 4 bit for pre-emption priority, 0 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//优先级设置  全为抢占式优先级
}

void NVIC_SRAM(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
}


/*******************************************************************************
* Function Name :void SystemConfig(void)
* Description   :
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  13:14:59
*******************************************************************************/
void BspInit(void)
{
    RCC_Configuration();
    NVIC_Configuration();	
    GPIO_Configuration();	


    bsp_InitTimer(); 

    BspTim2Init();
#if 0    
    BspTim3Init(); 
    BspUsart1Init();
    LedInit();
    HT1632C_Init();
    CodeKey_Init();
    IRCodeInit();
    RelayVolControlInit();
    InitEeprom();
    InitKey();
#endif

    InOutControlInit();

    DisplayInit();

    InitKey();

    InitNRF522Func();

    rtcInit();

    //BspTim3Open();
}

