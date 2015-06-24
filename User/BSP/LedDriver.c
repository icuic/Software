#include "LedDriver.h"


void LedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    u32 led_RCC_APB1Periph = LED_CLK_APB1_LIST,led_RCC_APB2Periph = LED_CLK_APB2_LIST;
    u8 i;
    GPIO_TypeDef* led_type[NUM_LED] = {LED_TYPE_LIST};
    u16  led_pin[NUM_LED] = {LED_PIN_LIST};
    
    if(led_RCC_APB1Periph != 0)
    {
        RCC_APB1PeriphClockCmd(led_RCC_APB1Periph, ENABLE);
    }

    if(led_RCC_APB2Periph != 0)
    {
        RCC_APB2PeriphClockCmd(led_RCC_APB2Periph, ENABLE);
    }


    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    for(i = 0;i < NUM_LED; i++)
    {
        GPIO_InitStructure.GPIO_Pin = led_pin[i];
        GPIO_Init(led_type[i], &GPIO_InitStructure);
        GPIO_WriteBit(led_type[i],led_pin[i],E_OFF);
    }

#if (JTAGDisable == 1)	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
#endif

}

void LedSetStatus(LEDTYPE ledid,BitAction ledstatus)
{

    GPIO_TypeDef* led_type[NUM_LED] = {LED_TYPE_LIST};
    u16  led_pin[NUM_LED] = {LED_PIN_LIST};

    GPIO_WriteBit(led_type[ledid],led_pin[ledid],ledstatus);
}
