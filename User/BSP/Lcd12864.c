#include "Lcd12864.h"

#define RS_ON      GPIO_SetBits(GPIOB,GPIO_Pin_12);
#define RS_OFF     GPIO_ResetBits(GPIOB,GPIO_Pin_12);
#define RW_ON      GPIO_SetBits(GPIOB,GPIO_Pin_9);
#define RW_OFF     GPIO_ResetBits(GPIOB,GPIO_Pin_9);
#define E_ON       GPIO_SetBits(GPIOB,GPIO_Pin_8);
#define E_OFF      GPIO_ResetBits(GPIOB,GPIO_Pin_8);
#define RST_ON     GPIO_SetBits(GPIOA,GPIO_Pin_9);
#define RST_OFF    GPIO_ResetBits(GPIOA,GPIO_Pin_9);

const unsigned char hohai[] =   
{  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC7,0x00,  
0x00,0x00,0x00,0x1C,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xCF,0x00,  
0x01,0xE0,0x00,0x3C,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xCF,0x00,  
0x01,0xE0,0x00,0x7C,0x00,0x00,0x06,0x00,0x00,0x00,0x60,0x00,0x00,0x1F,0xCE,0x00,  
0x01,0xE0,0x00,0xF8,0x00,0x00,0x06,0x00,0x00,0x00,0x70,0x00,0x00,0x1F,0xDC,0x00,  
0x01,0xE0,0x01,0xF0,0x00,0x00,0x04,0x00,0x00,0x00,0x70,0x00,0x00,0x0F,0x9C,0x00,  
0x01,0xC0,0x03,0xE0,0x00,0x00,0x0C,0x00,0x00,0x00,0x70,0x00,0x00,0x0F,0x9C,0x00,  
0x00,0x00,0x07,0xC0,0x00,0x00,0x08,0x00,0x00,0x00,0x70,0x00,0x00,0x0F,0xFC,0x00,  
0x00,0x00,0x0F,0x80,0x00,0xE0,0x08,0x00,0x00,0x00,0x70,0x00,0x00,0x0F,0xFE,0x00,  
0x00,0x00,0x1F,0x80,0x00,0xF0,0x18,0x00,0x00,0x00,0x60,0x00,0x00,0x0F,0xEE,0x00,  
0x00,0x00,0x3B,0xC0,0x00,0xF0,0x10,0xE0,0x00,0x00,0x60,0x00,0x00,0x0F,0xE6,0x00,  
0x00,0x00,0x73,0xE0,0x00,0xF0,0x11,0xE0,0x00,0x00,0x60,0x00,0x00,0x0F,0xEE,0x00,  
0x00,0x00,0x66,0xE0,0x00,0x70,0x27,0xC0,0x00,0x00,0x60,0x00,0x00,0x0F,0xCC,0x00,  
0x00,0x00,0xE4,0x40,0x00,0x70,0x3C,0x80,0x00,0x00,0x60,0x00,0x00,0x0E,0xF8,0x00,  
0x01,0x81,0xC4,0x40,0x00,0x00,0x38,0x80,0x00,0x00,0x60,0x00,0x00,0x0E,0xF3,0xC0,  
0x03,0xC1,0x8C,0x40,0x00,0x00,0x31,0x00,0x00,0x00,0x64,0x00,0x00,0x0E,0xFF,0xC0,  
0x03,0xC3,0x08,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x0C,0xFF,0xC0,  
0x03,0xC7,0x0A,0x40,0x00,0x20,0x02,0x60,0x00,0x00,0xFF,0x00,0x00,0x0C,0xF8,0xC0,  
0x01,0xC6,0x1E,0xC0,0x00,0x70,0x00,0xE0,0x00,0x00,0xFF,0x00,0x00,0x01,0xF0,0xC0,  
0x01,0xCC,0x1E,0xC0,0x00,0x70,0x04,0x60,0x00,0x1B,0xFE,0x00,0x00,0x03,0xE1,0x80,  
0x01,0x8C,0x3E,0xC0,0x00,0x70,0x04,0x60,0x00,0x3F,0xE0,0x00,0x00,0x0F,0x83,0x00,  
0x01,0x9C,0x3E,0x40,0x00,0x30,0x04,0x60,0x00,0x3F,0xC0,0x00,0x00,0x1F,0x00,0x00,  
0x03,0x18,0x3E,0xC0,0x00,0x20,0x09,0xE0,0x00,0x3F,0xC0,0x00,0x00,0x3C,0x38,0x00,  
0x03,0x18,0x6E,0xC0,0x00,0x60,0x0B,0xE0,0x00,0x1C,0xC0,0x00,0x06,0x70,0x38,0x00,  
0x03,0x30,0x66,0xC0,0x00,0x40,0x0B,0xFE,0x00,0x00,0xC0,0x00,0x07,0xE0,0xF0,0x00,  
0x03,0x30,0x60,0xC0,0x00,0x40,0x1B,0xFF,0x00,0x01,0xC0,0x00,0x07,0xC0,0xF0,0x00,  
0x03,0x20,0x60,0xC0,0x00,0x00,0xFF,0xE2,0x00,0x01,0xC0,0x00,0x0F,0x80,0x20,0x00,  
0x03,0x60,0x00,0xC0,0x00,0x88,0xFF,0xC0,0x00,0x01,0x80,0x00,0x06,0x00,0x20,0x00,  
0x03,0x60,0x00,0xC0,0x00,0x88,0x7E,0xC0,0x00,0x01,0x84,0x00,0x00,0x00,0x20,0x00,  
0x03,0x40,0x00,0xC0,0x00,0x90,0x30,0xC0,0x00,0x03,0x06,0x00,0x00,0x00,0x30,0x00,  
0x02,0x40,0x00,0xC0,0x00,0x90,0x19,0xC0,0x00,0x03,0x07,0x00,0x00,0x00,0x10,0x00,  
0x02,0x40,0x00,0xC0,0x00,0xB0,0x3F,0xE0,0x00,0x06,0x07,0x80,0x00,0x00,0x18,0x00,  
0x03,0xC0,0x00,0xC0,0x00,0xB0,0x1F,0xE0,0x00,0x06,0x03,0xC0,0x00,0x00,0x08,0x00,  
0x03,0xC0,0x00,0xC0,0x01,0xF0,0x1F,0xC0,0x00,0x0E,0x01,0xC0,0x00,0x00,0x1F,0x80,  
0x03,0xC0,0x00,0xC0,0x01,0xF0,0x00,0xC0,0x00,0x1C,0x01,0xE0,0x00,0x00,0x7F,0x80,  
0x03,0xC0,0x00,0xC0,0x01,0xE0,0x01,0x80,0x00,0x78,0x01,0xE0,0x00,0x00,0xFF,0x80,  
0x03,0xC0,0x00,0xC0,0x01,0xE0,0x01,0x80,0x00,0x70,0x00,0xE0,0x00,0x01,0xFF,0x00,  
0x03,0x80,0x00,0xC0,0x01,0xE0,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x01,0x84,0x00,  
0x03,0x80,0x00,0xC0,0x01,0xC0,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x03,0x8C,0x00,  
0x00,0x00,0x03,0xC0,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x08,0x00,  
0x00,0x00,0x01,0xC0,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x08,0x00,  
0x00,0x00,0x01,0xC0,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x08,0x00,  
0x00,0x00,0x01,0xC0,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x18,0x00,  
0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
};


static unsigned char ReadCardPlease[] =
{
    0x00,0x40,0x40,0x40,0x27,0xFC,0x20,0x40,0x03,0xF8,0x00,0x40,0xE7,0xFE,0x20,0x00,
    0x23,0xF8,0x22,0x08,0x23,0xF8,0x22,0x08,0x2B,0xF8,0x32,0x08,0x22,0x28,0x02,0x10,/*"请",0*/

    0x00,0x20,0x20,0x20,0x11,0xFC,0x10,0x20,0x00,0x20,0x03,0xFE,0xF0,0x02,0x10,0x94,
    0x10,0x50,0x11,0x10,0x10,0x90,0x13,0xFE,0x14,0x28,0x18,0x44,0x10,0x82,0x03,0x02,/*"读",1*/

    0x02,0x00,0x02,0x00,0x03,0xF8,0x02,0x00,0x02,0x00,0x02,0x00,0xFF,0xFE,0x02,0x00,
    0x02,0x00,0x02,0x40,0x02,0x20,0x02,0x10,0x02,0x08,0x02,0x00,0x02,0x00,0x02,0x00,/*"卡",2*/
};

static unsigned char RoomHasOpen[] = 
{
    0x10,0x40,0x10,0x40,0x3F,0x7E,0x28,0x90,0x45,0x08,0x88,0x00,0x08,0xFC,0x7E,0x84,
    0x08,0x84,0x18,0xFC,0x1C,0x84,0x2A,0x84,0x2A,0xFC,0x48,0x84,0x88,0x84,0x08,0xFC,/*"箱",0*/
    
    0x20,0x00,0x13,0xFC,0x10,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,
    0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x14,0x40,0x08,/*"门",1*/
    
    0x00,0x00,0x3F,0xF0,0x00,0x10,0x00,0x10,0x00,0x10,0x20,0x10,0x20,0x10,0x3F,0xF0,
    0x20,0x00,0x20,0x00,0x20,0x00,0x20,0x04,0x20,0x04,0x20,0x04,0x1F,0xFC,0x00,0x00,/*"已",2*/
    
    0x10,0x00,0x10,0x00,0x13,0xFE,0x10,0x20,0xFC,0x20,0x10,0x20,0x10,0x20,0x14,0x20,
    0x18,0x20,0x30,0x20,0xD0,0x20,0x10,0x20,0x10,0x20,0x10,0x20,0x50,0xA0,0x20,0x40,/*"打",3*/
    
    0x00,0x00,0x7F,0xFC,0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0xFF,0xFE,
    0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0x10,0x20,0x10,0x20,0x20,0x20,0x40,0x20,/*"开",4*/
};

static unsigned char CloseTheBoxPls[] =
{
    0x00,0x40,0x40,0x40,0x27,0xFC,0x20,0x40,0x03,0xF8,0x00,0x40,0xE7,0xFE,0x20,0x00,
    0x23,0xF8,0x22,0x08,0x23,0xF8,0x22,0x08,0x2B,0xF8,0x32,0x08,0x22,0x28,0x02,0x10,/*"请",0*/
    
    0x00,0x10,0x78,0x10,0x49,0xFE,0x54,0x20,0x52,0x40,0x60,0xFC,0x51,0x44,0x48,0x44,
    0x4E,0x7C,0x4A,0x44,0x6A,0x7C,0x52,0x44,0x42,0x54,0x42,0x48,0x45,0x00,0x48,0xFE,/*"随",1*/
    
    0x00,0x10,0x00,0xF8,0x3F,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00,
    0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00,/*"手",2*/
    
    0x10,0x10,0x08,0x10,0x08,0x20,0x00,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00,0x01,0x00,
    0xFF,0xFE,0x01,0x00,0x02,0x80,0x02,0x80,0x04,0x40,0x08,0x20,0x30,0x18,0xC0,0x06,/*"关",3*/
    
    0x20,0x00,0x17,0xFC,0x00,0x04,0x40,0x84,0x40,0x84,0x5F,0xF4,0x40,0x84,0x41,0x84,
    0x42,0x84,0x44,0x84,0x48,0x84,0x50,0x84,0x42,0x84,0x41,0x04,0x40,0x14,0x40,0x08,/*"闭",4*/
    
    0x10,0x40,0x10,0x40,0x3F,0x7E,0x28,0x90,0x45,0x08,0x88,0x00,0x08,0xFC,0x7E,0x84,
    0x08,0x84,0x18,0xFC,0x1C,0x84,0x2A,0x84,0x2A,0xFC,0x48,0x84,0x88,0x84,0x08,0xFC,/*"箱",5*/
    
    0x20,0x00,0x13,0xFC,0x10,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,
    0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x14,0x40,0x08,/*"门",6*/

};

void delay(unsigned int ms)
{
    unsigned int y;
    for(y=0;y<4000;y++);   // 24000
}

void InitLcd(void)
{
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    //RS,RW,E
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    //RST    
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    //DATA
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6| GPIO_Pin_7;
    
    GPIO_Init(GPIOB,&GPIO_InitStructure);  

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |GPIO_Pin_15;
    
    GPIO_Init(GPIOA,&GPIO_InitStructure); 

    //bk
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void BuildData(u8 dataVal)
{
    //d0
    if(dataVal & 0x01)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_7);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_7);
    }

    //d1
    if(dataVal & 0x02)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_6);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_6);
    }

    //d2
    if(dataVal & 0x04)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_4);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_4);
    }

    //d3
    if(dataVal & 0x08)
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_3);
    }
    else
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_3);
    }

    //d4
    if(dataVal & 0x10)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_15);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    }

    //d5
    if(dataVal & 0x20)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_12);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_12);
    }

    //d6
    if(dataVal & 0x40)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_11);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_11);
    }

    //d7
    if(dataVal & 0x80)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_10);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_10);
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

    LCD_Clear_GDRAM();

    //LcdWcom(0x30);  // 基本指令
    LcdWcom(0x0c);  // 不显示游标
    LcdWcom(0x01);  // 清屏

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

void ClearDisplay(void)
{
    delay(10);

    LcdWcom(0x01);
}


void tupian(unsigned char *p) //图片数据提取函数 
{
    unsigned char x,y,a,b,c;

    LcdWcom(0x34); //关闭绘图功能

    x=0x80;
    y=0x80;   

    for(c=0;c<2;c++)  
    {   
        for(a=0;a<32;a++)   
        {    
            LcdWcom(y+a);    
            LcdWcom(x);    

            for(b=0;b<16;b++)      
                LcdWdata(*p++);    
        }   

        x=0x88;   

    }   

    LcdWcom(0x36); //打开绘图功能  

    LcdWcom(0x30);   
}

//一种算法，测试OK xianhuitu test OK
// 测试发现整个执行完大概需要2~3S
 void LCD_Clear_GDRAM(void)  //test OK
{    
  u8 j, i;
  LcdWcom(0x34);    //关闭绘图功能
  
  for(j=0;j<32;j++) //上半屏32点行
  {
    LcdWcom(0x80+j);
    LcdWcom(0x80);//X坐标
    
    for(i=0;i<32;i++)//32表示如1行写完,自动写第3行
    {
      LcdWdata(0x00);  
    }
  }
  
  LcdWcom(0x30);     //基本指令 
}


void DisplayCustomChar(unsigned char row, unsigned char column, unsigned char *pData, unsigned char num)
{
    unsigned char x_start = 0x80, y_start = 0x80;
    unsigned char i = 0, j = 0;

    LcdWcom(0x34); //关闭绘图功能
    
    switch(row)
    {
        case 0x00: 
        {
            y_start = 0x80;
            x_start = 0x80 + column;
        }
        break;

        case 0x01: 
        {
            y_start = 0x90;
            x_start = 0x80 + column;
        }
        break;

        case 0x02: 
        {
            y_start = 0x80;
            x_start = 0x88 + column;
        }
        break;

        case 0x03: 
        {
            y_start = 0x90;
            x_start = 0x88 + column;
        }
        break;

        default: 
        break;
    }


    for(i=0; i<16; i++)   
    {    
        LcdWcom(y_start + i);    
        LcdWcom(x_start);

        LcdWdata(0);

        for(j = 0; j < num; j++)
        {
            LcdWdata(*(pData + j * 32 + i * 2));
            LcdWdata(*(pData + j * 32 + i * 2 + 1));
        }   
    } 

    LcdWcom(0x36); //打开绘图功能  

    LcdWcom(0x30); 
}
void DisplayPic(void)
{
    tupian((unsigned char*)hohai);
}

void DisplayCustomStr(eCustomString eStr)
{
    switch(eStr)
    {
        case E_CUSTOM_STR_READ_CARD_PLEASE:
            DisplayCustomChar(2, 2, ReadCardPlease, 3);
            break;

        case E_CUSTOM_STR_ROOM_HAS_OPEN:
            DisplayCustomChar(1, 1, RoomHasOpen, 5);
            DisplayCustomChar(2, 0, CloseTheBoxPls, 7);
            break;

        default:
            break;
    }
}
