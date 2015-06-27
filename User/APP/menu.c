#include "stm32f10x.h"
#include "menu.h"
#include "Lcd12864.h"
#include "bsp.h"
#include "bsp_timer.h"
#include <stdio.h>


extern void DisplayStr(u8 *str,u8 x,u8 y);

void draw_welcome(void);
void enter_welcome(void);
void action_welcome(uint8_t key);

void draw_open_success(void);
void enter_open_success(void);
void action_open_success(uint8_t key);


UIStruct menuTab[E_UI_MAX] = 
{
    {
    E_UI_WELCOME,
    E_UI_WELCOME,
    draw_welcome,
    enter_welcome,
    action_welcome,
    },
    
    {
    E_UI_OPEN_SUCCESS,
    E_UI_OPEN_SUCCESS,
    draw_open_success,
    enter_open_success,
    action_open_success,
    },
};

volatile eUIIndex currentMenu = E_UI_WELCOME;
eUIIndex lastMenu = E_UI_WELCOME;

u8 lockStat = 0;


u8 DockCard[2][4] = 
    {
        {0x75,0x98,0x8a,0x52},
        {0xD5,0x38,0x08,0xC0}
};

void timeout()
{
    currentMenu = E_UI_WELCOME;
}

void scanCard(void)
{
    unsigned char UID[5],Temp[4];
    u8 i,j;

    //scan card
    if(PcdRequest(0x52,Temp)==MI_OK)
    {
    #if 0
        if(Temp[0]==0x04&&Temp[1]==0x00) 
        {
            //DisplayStr("MFOne-S50",1,0);
        }
        else if(Temp[0]==0x02&&Temp[1]==0x00)
        {
            DisplayStr("MFOne-S70",1,0);
        }
        else if(Temp[0]==0x44&&Temp[1]==0x00)
        {
            DisplayStr("MF-UltraLight",1,0);
        }
        else if(Temp[0]==0x08&&Temp[1]==0x00)
        {
            DisplayStr("MF-Pro",1,0);
        }
        else if(Temp[0]==0x44&&Temp[1]==0x03)
        {
            DisplayStr("MF Desire",1,0);
        }
        else
        {
            DisplayStr("Unknown",1,0);
        }
    #endif
        if(PcdAnticoll(UID)==MI_OK)
        { 
            //DisplayStr("Card Id is:",2,0);
            //sprintf(tmps,"%02X%02X%02X%02X",UID[0],UID[1],UID[2],UID[3]);
            //tmps[8] = 0;
            //DisplayStr(tmps,3,0);
            for(i=0;i<2;i++)
            {
                for(j=0;j<4;j++)
                {
                    if(DockCard[i][j] != UID[j])
                    {
                        break;
                    }
                }
    
                if(j == 4)
                {
                
                    if(i ==0)
                    {
                        OpenLock(0X0001);
                    }
                    else
                    {
                        OpenLock(0X0002);
                    }
                    lockStat = 1;
                    IS_TIMEOUT_1MS(LockPlus,0);
                    currentMenu = E_UI_OPEN_SUCCESS;
                    
                    bsp_StartTimer(1, 5000, timeout);
                }
            }
            
        }
    }


}

void draw_welcome(void)
{
    DisplayStr("欢迎使用", 0, 2);
    DisplayStr("请读卡", 2, 2);
}

void enter_welcome(void)
{
}

void action_welcome(uint8_t key)
{
    draw_welcome();

    // scan card
    scanCard();
}


void draw_open_success(void)
{
    
    DisplayStr("箱门已打开", 0, 2);
    DisplayStr("请随手关闭箱门", 2, 0);
}

void enter_open_success(void)
{
}

void action_open_success(uint8_t key)
{
    draw_open_success();
    
}


void menu_handle(uint8_t key)
{
    uint8_t tmps[5];

    // Soft timer 1 used for menu only
    bsp_CheckTimer(1);

    sprintf(tmps,"%d",(u8)currentMenu);
    DisplayStr(tmps,0,7);

    if (lastMenu != currentMenu)
    {
        LcdWcom(0x01);
    }
    
    if (menuTab[currentMenu].menu_action != 0)
        menuTab[currentMenu].menu_action(key);

    lastMenu = currentMenu;
}

