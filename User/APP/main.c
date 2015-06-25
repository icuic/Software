
/* Includes ------------------------------------------------------------------*/

#include "bsp.h"
#include <stdio.h>

unsigned char UID[5],Temp[4]                                       ;
unsigned char RF_Buffer[18]                                        ;
unsigned char Password_Buffer[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}   ; // Mifare One ????

char          MBRX[30]                                             ;
char          MBKeyTP[30]                                          ;
char          Event   ;               
    
u8 DockCard[2][4] = 
    {
        {0x75,0x98,0x8a,0x52},
        {0xD5,0x38,0x08,0xC0}
};

/*typedef struct
{
    eUIStatus windowIndex;              //当前窗口状态索引
    void (*menu_draw)(void);            // 界面显示
    void (*menu_enter)(void);           // 进入界面处理
    void (*menu_action)(uint8_t triggeredKey);  // 界面对应按键操作
        
}UIStruct;*/


void DisplayKey(u8 keyvalue)
{
    u8 i = 0 ,j = 0;

    SetCursor(0,0);

    SetCursor(0,0);
    for(i = 0;i<17;i++)
    {
        if(MyKey[i].KeyValueRead == keyvalue)
        {
            break;
        }
    }

    if(i<17)
    {
        j = 0;
        while(MyKey[i].KeyString[j] != 0)
        {
            LcdWdata(MyKey[i].KeyString[j]);
            j++;
        }

    for(;j<10;j++)
    {
      LcdWdata(' ');
    }
    }
}

void DisplayStr(u8 *str,u8 x,u8 y)
{
    SetCursor(x,y);
    while(*str != 0)
    {
        LcdWdata(*str);
        str++;
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
    u8 lockStat = 0,tmps[20],i,j;
    
    BspInit();

    while (1)
    {
        //scaned key
        if(IS_TIMEOUT_1MS(Keyscan,20))
        {
            DisplayKey(scankey());
            IS_TIMEOUT_1MS(Keyscan,0);
        }

        //scan card
        if(PcdRequest(0x52,Temp)==MI_OK)
        {
            if(Temp[0]==0x04&&Temp[1]==0x00) 
            {
                DisplayStr("MFOne-S50",1,0);
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
            
            if(PcdAnticoll(UID)==MI_OK)
            { 
                DisplayStr("Card Id is:",2,0);
                sprintf(tmps,"%02X%02X%02X%02X",UID[0],UID[1],UID[2],UID[3]);
                tmps[8] = 0;
                DisplayStr(tmps,3,0);
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
                    }
                }
                
            }
        }

        if(lockStat== 1)
        {
            if(IS_TIMEOUT_1MS(LockPlus,50))
            {
                OpenLock(0x0000);
        lockStat = 0;
            }

      
        }
        
    }
}


/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
