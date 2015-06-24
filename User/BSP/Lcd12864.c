#include "Lcd12864.h"

#define RS_ON      GPIO_SetBits(GPIOA,GPIO_Pin_0);
#define RS_OFF     GPIO_ResetBits(GPIOA,GPIO_Pin_0);
#define RW_ON      GPIO_SetBits(GPIOA,GPIO_Pin_1);
#define RW_OFF     GPIO_ResetBits(GPIOA,GPIO_Pin_1);
#define E_ON       GPIO_SetBits(GPIOA,GPIO_Pin_2);
#define E_OFF      GPIO_ResetBits(GPIOA,GPIO_Pin_2);
#define RST_ON     GPIO_SetBits(GPIOB,GPIO_Pin_11);
#define RST_OFF    GPIO_ResetBits(GPIOB,GPIO_Pin_11);

void delay(unsigned int ms)
{
    unsigned int y;
    for(y=0;y<24000;y++);
}

void InitLcd(void)
{
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    //RS,RW,E
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    //RST    
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    //DATA
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5
                                    |GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
                                    |GPIO_Pin_9 | GPIO_Pin_10;
    
	GPIO_Init(GPIOB,&GPIO_InitStructure);  

    //bk
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_3);
}

void BuildData(u8 dataVal)
{
    //d0
    if(dataVal & 0x01)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_0);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    }

    //d1
    if(dataVal & 0x02)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_1);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_1);
    }

    //d2
    if(dataVal & 0x04)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_5);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_5);
    }

    //d3
    if(dataVal & 0x08)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_6);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_6);
    }

    //d4
    if(dataVal & 0x10)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_7);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_7);
    }

    //d5
    if(dataVal & 0x20)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_8);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_8);
    }

    //d6
    if(dataVal & 0x40)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_9);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_9);
    }

    //d7
    if(dataVal & 0x80)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_10);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_10);
    }

}

void LcdWcom(unsigned char WCom)
{
    delay(10);
    RS_OFF;
    RW_OFF;
    BuildData(WCom);
    E_ON;
    delay(10);
    E_OFF;
}

void LcdWdata(unsigned char WData)
{
    delay(10);
    RS_ON;
    RW_OFF;
    BuildData(WData);
    E_ON;
    delay(10);
    E_OFF;
}

void DisplayInit(void)
{
    InitLcd();
    delay(10);

    RST_OFF;
    delay(10);
    RST_ON;

    LcdWcom(0x30); 
    LcdWcom(0x0c);
    LcdWcom(0x01);
}
void SetCursor(unsigned char x,unsigned char y)
{
    unsigned char i;
    switch(x)
    {
        case 0x00: i=0x80; 
        break;
        case 0x01: i=0x90; 
        break;
        case 0x02: i=0x88; 
        break;
        case 0x03: i=0x98; 
        break;
        default: break;
    }
    i=y+i;
    LcdWcom(i);
}

