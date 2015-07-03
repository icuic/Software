#include "stm32f10x.h"
#include "menu.h"
#include "Lcd12864.h"
#include "bsp.h"
#include "bsp_timer.h"
#include <stdio.h>


extern void DisplayStr(u8 *str,u8 x,u8 y);

void draw_welcome(void);
void enter_welcome(uint8_t key);
void action_welcome(uint8_t key);

void draw_open_success(void);
void enter_open_success(uint8_t key);
void action_open_success(uint8_t key);

void draw_room_number(void);
void enter_room_number(uint8_t key);
void action_room_number(uint8_t key);


void draw_room_pw(void);
void enter_room_pw(uint8_t key);
void action_room_pw(uint8_t key);


void draw_room_invalid(void);
void enter_room_invalid(uint8_t key);
void action_room_invalid(uint8_t key);

void draw_room_pw_error(void);
void enter_room_pw_error(uint8_t key);
void action_room_pw_error(uint8_t key);



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
    E_UI_USER_ROOM_NUM,
    E_UI_WELCOME,
    draw_room_number,
    enter_room_number,
    action_room_number,
    },

    {
    E_UI_USER_PASSWORD,
    E_UI_WELCOME,
    draw_room_pw,
    enter_room_pw,
    action_room_pw,
    },
    
    {
    E_UI_OPEN_SUCCESS,
    E_UI_WELCOME,
    draw_open_success,
    enter_open_success,
    action_open_success,
    },

    {
    E_UI_ROOM_INVALID,
    E_UI_WELCOME,
    draw_room_invalid,
    enter_room_invalid,
    action_room_invalid,
    },

    {
    E_UI_ROOM_PW_ERROR,
    E_UI_USER_PASSWORD,
    draw_room_pw_error,
    enter_room_pw_error,
    action_room_pw_error,
    }
};

volatile eUIIndex currentMenu = E_UI_WELCOME;
eUIIndex lastMenu = E_UI_WELCOME;

u8 lockStat = 0;


u8 DockCard[2][4] = 
    {
        {0x75,0x98,0x8a,0x52},
        {0xD5,0x38,0x08,0xC0}
};

static uint8_t lenRoomNum = 0;           // length of room number
static uint8_t roomNum[6];      // Room number

static uint8_t lenRoomPW = 0;   // length of room password
static uint8_t roomPW[6];       // Room password


void timeout()
{
    currentMenu = menuTab[currentMenu].parent;

    LcdWcom(0x01);
    menuTab[currentMenu].menu_enter(0);
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

                    if(menuTab[currentMenu].menu_enter != 0)
                        menuTab[currentMenu].menu_enter(0);
                    
                    //bsp_StartTimer(1, 3000, timeout);
                }
            }
            
        }
    }


}

void DisplayDataTime(uint32_t TimeVar)
{
    uint32_t THH = 0, TMM = 0, TSS = 0;
    uint8_t tmp[16];
    
    /* Reset RTC Counter when Time is 23:59:59 */
    if (RTC_GetCounter() == 0x0001517F)
    {
       RTC_SetCounter(0x0);
       /* Wait until last write operation on RTC registers has finished */
       RTC_WaitForLastTask();
    }
    
    /* Compute  hours */
    THH = TimeVar / 3600;
    /* Compute minutes */
    TMM = (TimeVar % 3600) / 60;
    /* Compute seconds */
    TSS = (TimeVar % 3600) % 60;
    
    sprintf(tmp, "%02d:%02d:%02d", THH, TMM, TSS);
    DisplayStr(tmp, 3, 2);
}


void draw_welcome(void)
{
    DisplayStr("欢迎使用", 0, 2);
    DisplayStr("请读卡", 2, 2);
    //SetCursor(2, 2);
    //LcdWdata(' ');
    //LcdWdata(0xC7);
    //LcdWdata(0xEB);
    //DisplayDataTime(RTC_GetCounter());
}

void enter_welcome(uint8_t key)
{
    LcdWcom(0x01);
}

void action_welcome(uint8_t key)
{
    draw_welcome();

    // scan card
    scanCard();

    if(key != 0xff)
    {
        currentMenu = E_UI_USER_ROOM_NUM;
        enter_room_number(key);
    }
}


void draw_open_success(void)
{
    
    DisplayStr("箱门已打开", 0, 2);
    DisplayStr("请随手关闭箱门", 2, 0);
}

void enter_open_success(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 3000, timeout);
}

void action_open_success(uint8_t key)
{
    draw_open_success();
    
}

extern void DisplayKey(u8 keyvalue, u8 x, u8 y);




void DisplayRoomNumber(uint8_t keyvalue)
{
    uint8_t i, j;
    extern const KeyValueTable MyKey[17]; 

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
    }
}



void draw_room_number(void)
{
    uint8_t j = 0;

    DisplayStr("请输入房间号", 0, 1);

    SetCursor(2,0);
    for(j = 0; j < lenRoomNum; j++)
        DisplayRoomNumber(roomNum[j]);

}

void enter_room_number(uint8_t key)
{
    if (key == 0xFF)
        return;

    lenRoomNum = 0;
    memset(roomNum, 0, sizeof(roomNum));    
    roomNum[lenRoomNum++] = key;

    LcdWcom(0x01);
    draw_room_number();

    bsp_StartTimer(1, 15000, timeout);
}



void translateKey(uint8_t *key, uint8_t len)
{
    uint8_t i, j;
    for(i = 0; i < len; i++)
    {
        for(j = 0; j < 17; j++)
        {
            if (*(key + i) == MyKey[j].KeyValueRead)
                break;
        }

        if (j < 17)
            *(key + i) = MyKey[j].NumASCII;
    }
}


void action_room_number(uint8_t key)
{

    if (key == 0xFF)
        return;

    if (key == 0x8F)
    {
        translateKey(roomNum, lenRoomNum);
    
        if (strcmp(roomNum, "606", lenRoomNum) == 0)
        {
            currentMenu = E_UI_USER_PASSWORD;

            LcdWcom(0x01);
            enter_room_pw(key);
        }
        else
        {
            currentMenu = E_UI_ROOM_INVALID;

            LcdWcom(0x01);
            enter_room_invalid(key);
        }
    }
    else
    {
        roomNum[lenRoomNum++] = key;
        
        draw_room_number();
    }
}


void draw_room_pw(void)
{
    uint8_t j = 0;

    DisplayStr("请输入密码", 0, 1);

    SetCursor(2,0);
    for(j = 0; j < lenRoomPW; j++)
        LcdWdata('*');

}

void enter_room_pw(uint8_t key)
{
    //if (key == 0xFF)
    //    return;

    lenRoomPW= 0;
    memset(roomPW, 0, sizeof(roomPW));
    
    LcdWcom(0x01);
    draw_room_pw();

    bsp_StartTimer(1, 15000, timeout);
}

void action_room_pw(uint8_t key)
{


    if (key == 0xFF)
        return;

    if (key == 0x8F)
    {

        translateKey(roomPW, lenRoomPW);
    
        if (strcmp(roomPW, "888", lenRoomPW) == 0)
        {
            OpenLock(0X0002);

            lockStat = 1;
            IS_TIMEOUT_1MS(LockPlus,0);

        
            currentMenu = E_UI_OPEN_SUCCESS;

            LcdWcom(0x01);
            enter_open_success(key);
        }
        else
        {
            currentMenu = E_UI_ROOM_PW_ERROR;

            LcdWcom(0x01);
            enter_room_pw_error(key);
        }
    }
    else
    {
        roomPW[lenRoomPW++] = key;
        
        draw_room_pw();
    }
    
}


void draw_room_invalid(void)
{
    DisplayStr("无效房间号", 2, 0);
}

void enter_room_invalid(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 2000, timeout);
}

void action_room_invalid(uint8_t key)
{
    draw_room_invalid();
}


void draw_room_pw_error(void)
{
    DisplayStr("密码错误", 2, 0);
}

void enter_room_pw_error(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 2000, timeout);
}

void action_room_pw_error(uint8_t key)
{
    draw_room_pw_error();
}


void menu_handle(uint8_t key)
{
    uint8_t tmps[5];

    // Soft timer 1 used for menu only
    bsp_CheckTimer(1);

    // Clear
    if (lastMenu != currentMenu)
    {
        //LcdWcom(0x01);
    }

    // Update variable lastMenu
    lastMenu = currentMenu;

    // Menu display
    if (menuTab[currentMenu].menu_action != 0)
        menuTab[currentMenu].menu_action(key);
}

