
/* Includes ------------------------------------------------------------------*/

#include "bsp.h"
#include <stdio.h>
#include "menu.h"
#include "roomInfoManage.h"



unsigned char UID[5],Temp[4]                                       ;
unsigned char RF_Buffer[18]                                        ;
unsigned char Password_Buffer[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}   ; // Mifare One ????

char          MBRX[30]                                             ;
char          MBKeyTP[30]                                          ;
char          Event   ;               
    
volatile uint8_t aaa = 0;




extern eUIIndex currentMenu;

void DisplayKey(u8 keyvalue, u8 x, u8 y)
{


    u8 i = 0 ,j = 0;

    if(keyvalue == 0xFF)
        return;
    
    SetCursor(x,y);

    SetCursor(x,y);
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
      //LcdWdata(' ');
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
    extern u8 lockStat;

    uint8_t u08Key;
    
    BspInit();

    recoverRoomInfoFromFlash();
    recoverAdminCardIDFromFlash();

    while (1)
    {
        // scan key
        #if 0
        if(IS_TIMEOUT_1MS(Keyscan,100))
        {
            KeyCode = scankey();
            //DisplayKey(bbb);
            
            IS_TIMEOUT_1MS(Keyscan,0);
        }
        #endif

        key_detect();

        // menu display
        menu_handle(KeyCode);
        KeyCode = M_KEY_NO_KEY;


        // scan lock state
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
