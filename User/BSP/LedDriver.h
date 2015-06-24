#ifndef _LEDDRIVER_h_
#define _LEDDRIVER_h_

#include "stm32f10x.h"

typedef enum
{
    E_LED1,
    E_LED2,
    E_LED3,
    E_LED4,
    E_LED5,
    NUM_LED
}LEDTYPE;

#define E_ON  Bit_RESET
#define E_OFF   Bit_SET

#define JTAGDisable         1

#define RCC_APB2Periph      2
#define RCC_APB1Periph      1

#define LED1_IOPORT         GPIOB
#define LED1_CLOCK_PERIPH   RCC_APB2Periph_GPIOB             
#define LED1_GPIOPIN        GPIO_Pin_3

#define LED2_IOPORT         GPIOB
#define LED2_CLOCK_PERIPH   RCC_APB2Periph_GPIOB             
#define LED2_GPIOPIN        GPIO_Pin_4


#define LED3_IOPORT         GPIOB
#define LED3_CLOCK_PERIPH   RCC_APB2Periph_GPIOB             
#define LED3_GPIOPIN        GPIO_Pin_6


#define LED4_IOPORT         GPIOB
#define LED4_CLOCK_PERIPH   RCC_APB2Periph_GPIOB            
#define LED4_GPIOPIN        GPIO_Pin_7

#define LED5_IOPORT         GPIOB
#define LED5_CLOCK_PERIPH   RCC_APB2Periph_GPIOB             
#define LED5_GPIOPIN        GPIO_Pin_8

#define LED_TYPE_LIST       LED1_IOPORT##,##LED2_IOPORT##,##LED3_IOPORT##,##LED4_IOPORT##,##LED5_IOPORT
#define LED_PIN_LIST        LED1_GPIOPIN##,##LED2_GPIOPIN##,##LED3_GPIOPIN##,##LED4_GPIOPIN##,##LED5_GPIOPIN

#define LED_CLK_APB1_LIST   0
#define LED_CLK_APB2_LIST   LED1_CLOCK_PERIPH##|##LED2_CLOCK_PERIPH##|##LED3_CLOCK_PERIPH##|##LED4_CLOCK_PERIPH##|##LED5_CLOCK_PERIPH


void LedInit(void);
void LedSetStatus(LEDTYPE ledid,BitAction ledstatus);

#endif

