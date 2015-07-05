
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

    
    BspInit();

    
    initVirtAddVarTab();
    recoverRoomInfoFromEEPROM();
    recoverAdminCardIDFromEEPROM();


    while (1)
    {
        //scaned key
        //menu_handle(KeyCode);
        //DisplayKey(KeyCode);

        //sprintf(tmps,"%d",aaa);
        //DisplayStr(tmps,2,0);

        
        #if 1
        if(IS_TIMEOUT_1MS(Keyscan,20))
        {
            KeyCode = scankey();
            //DisplayKey(bbb);
            menu_handle(KeyCode);
            IS_TIMEOUT_1MS(Keyscan,0);
        }
        #endif
        
        

#if 0
        //scan card
        if(PcdRequest(0x52,Temp)==MI_OK)
        {
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
            
            if(PcdAnticoll(UID)==MI_OK)
            { 
                //DisplayStr("Card Id is:",2,0);
                sprintf(tmps,"%02X%02X%02X%02X",UID[0],UID[1],UID[2],UID[3]);
                tmps[8] = 0;
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
                    }
                }
                
            }
        }
#endif
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
