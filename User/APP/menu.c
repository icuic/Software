#include "stm32f10x.h"
#include "menu.h"
#include "Lcd12864.h"
#include "bsp.h"
#include "bsp_timer.h"
#include <stdio.h>
#include "roomInfoManage.h"
//#include "string.h"

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


void draw_set_admin_card(void);
void enter_set_admin_card(uint8_t key);
void action_set_admin_card(uint8_t key);


void draw_admin(void);
void enter_admin(uint8_t key);
void action_admin(uint8_t key);


void draw_user_setting(void);
void enter_user_setting(uint8_t key);
void action_user_setting(uint8_t key);


void draw_admin_setting(void);
void enter_admin_setting(uint8_t key);
void action_admin_setting(uint8_t key);


void draw_system_setting(void);
void enter_system_setting(uint8_t key);
void action_system_setting(uint8_t key);


void draw_box_setting(void);
void enter_box_setting(uint8_t key);
void action_box_setting(uint8_t key);


void draw_user_setting_box_pw(void);
void enter_user_setting_box_pw(uint8_t key);
void action_user_setting_box_pw(uint8_t key);


void draw_user_setting_box_pw_success(void);
void enter_user_setting_box_pw_success(uint8_t key);
void action_user_setting_box_pw_success(uint8_t key);


void draw_user_setting_box_pw_failed(void);
void enter_user_setting_box_pw_failed(uint8_t key);
void action_user_setting_box_pw_failed(uint8_t key);


void draw_user_setting_card_auth(void);
void enter_user_setting_card_auth(uint8_t key);
void action_user_setting_card_auth(uint8_t key);



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
    }, 

    {
    E_UI_SET_ADMIN_CARD,
    E_UI_WELCOME,
    draw_set_admin_card,
    enter_set_admin_card,
    action_set_admin_card,
    },

    {
    E_UI_ADMIN,
    E_UI_WELCOME,
    draw_admin,
    enter_admin,
    action_admin,
    },
    
    {
    E_UI_USER_SETTING,
    E_UI_ADMIN,
    draw_user_setting,
    enter_user_setting,
    action_user_setting,
    },
    
    {
    E_UI_ADMIN_SETTING,
    E_UI_ADMIN,
    draw_admin_setting,
    enter_admin_setting,
    action_admin_setting,
    },

    {
    E_UI_SYSTEM_SETTING,
    E_UI_ADMIN,
    draw_system_setting,
    enter_system_setting,
    action_system_setting,
    },

    {
    E_UI_BOX_SETTING,
    E_UI_ADMIN,
    draw_box_setting,
    enter_box_setting,
    action_box_setting,
    },

    {
    E_UI_USER_SETTING_BOX_PW,
    E_UI_USER_SETTING,
    draw_user_setting_box_pw,
    enter_user_setting_box_pw,
    action_user_setting_box_pw,
    },

    {
    E_UI_USER_SETTING_BOX_PW_SUCCESS,
    E_UI_USER_SETTING,
    draw_user_setting_box_pw_success,
    enter_user_setting_box_pw_success,
    action_user_setting_box_pw_success,
    },

    {
    E_UI_USER_SETTING_BOX_PW_FAILED,
    E_UI_USER_SETTING,
    draw_user_setting_box_pw_failed,
    enter_user_setting_box_pw_failed,
    action_user_setting_box_pw_failed,    
    },

    {
    E_UI_USER_SETTING_CARD_AUTH,
    E_UI_USER_SETTING,
    draw_user_setting_card_auth,
    enter_user_setting_card_auth,
    action_user_setting_card_auth,
    },
};

typedef enum
{
    E_BOX_PW_SUB_ROOM = 0,      // 输入房间号
    E_BOX_PW_SUB_PW,            // 第一次输入密码
    E_BOX_PW_SUB_PW_AGAIN,      // 再次输入密码
    E_BOX_PW_END
}eBoxPwSubState;

eBoxPwSubState boxPwSubState = E_BOX_PW_SUB_ROOM;   


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

static uint8_t lenRoomPWAgain = 0;   // length of room password
static uint8_t roomPWAgain[6];       // Room password



void timeout()
{
    currentMenu = menuTab[currentMenu].parent;

    LcdWcom(0x01);
    menuTab[currentMenu].menu_enter(0);
}

void scanCard(unsigned char* UID)
{
    unsigned char Temp[4];
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

            #if 0
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
            #endif
        }
    }


}

void isAuthCard(uint8_t* uid)
{
    uint8_t i = 0, j = 0;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(roomInfo[i].cardID[0][j] != uid[j])
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
    DisplayDataTime(RTC_GetCounter());
}

void enter_welcome(uint8_t key)
{
    LcdWcom(0x01);
}

void action_welcome(uint8_t key)
{
    uint8_t UID[5];
    memset(UID, 0xFF, 5);

    draw_welcome();

    // scan card
    scanCard(UID);

    if (memcmp(UID, "\xFF\xFF\xFF\xFF\xFF", 5))
    {
        isAuthCard(UID);
    }

    // admin
    if (!memcmp(adminCardID, UID, 4))
    {
        Beep(500);
    
        currentMenu = E_UI_ADMIN;
        enter_admin(key);
    }

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

        if (strcmp(roomNum, "888", lenRoomNum) == 0)
        {
            currentMenu = E_UI_SET_ADMIN_CARD;

            LcdWcom(0x01);
            enter_set_admin_card(key);
        }
        else if (strcmp(roomNum, "606", lenRoomNum) == 0)
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


void draw_set_admin_card(void)
{
    uint8_t tmps[6];

    DisplayStr("请刷管理员卡", 1, 1);
    
    sprintf(tmps,"%02X%02X%02X%02X",adminCardID[0],adminCardID[1],adminCardID[2],adminCardID[3]);
    DisplayStr(tmps,3,0);

}

void enter_set_admin_card(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 5000, timeout);
}

void action_set_admin_card(uint8_t key)
{    
    uint8_t UID[5];
    memset(UID, 0xFF, 5);

    draw_set_admin_card();

    // scan card
    scanCard(UID);

    if (memcmp(UID, "\xFF\xFF\xFF\xFF\xFF", 5))
    {
        // update RAM
        memcpy(adminCardID, UID, 4);
        // update EEPROM
        writeAdminCardIDToEEPROM(UID);
    }
}


void draw_admin(void)
{
    DisplayStr("设置", 0, 3);
    DisplayStr("1 用户", 1, 0);
    DisplayStr("2 管理", 1, 4);
    DisplayStr("3 系统", 2, 0);
    DisplayStr("4 箱门", 2, 4);    
    DisplayStr("0 退出", 3, 0);
}

void enter_admin(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

void action_admin(uint8_t key)
{    
    uint8_t tmp = key; 
    
    draw_admin();

    translateKey(&tmp, 1);


    switch(tmp)
    {
        case '1':
            currentMenu = E_UI_USER_SETTING;
            enter_user_setting(key);
            break;

        case '2':
            currentMenu = E_UI_ADMIN_SETTING;
            enter_admin_setting(key);
            break;

        case '3':
            currentMenu = E_UI_SYSTEM_SETTING;
            enter_system_setting(key);
            break;

        case '4':
            currentMenu = E_UI_BOX_SETTING;
            enter_box_setting(key);
            break;

        case '0':
            currentMenu = E_UI_WELCOME;
            enter_welcome(key);
            break;

        default:
            break;
    }
}


void draw_user_setting(void)
{
    DisplayStr("用户设置", 0, 2);
    DisplayStr("1 箱密", 1, 0);
    DisplayStr("2 房号", 1, 4);
    DisplayStr("3 授卡", 2, 0);
    DisplayStr("4 退卡", 2, 4);    
    DisplayStr("5 退出", 3, 0);
}

void enter_user_setting(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

void action_user_setting(uint8_t key)
{    
    uint8_t tmp = key; 
    
    draw_user_setting();

    translateKey(&tmp, 1);

    switch(tmp)
    {
        case '1':
            currentMenu = E_UI_USER_SETTING_BOX_PW;
            enter_user_setting_box_pw(key);
            break;

        case '3':
            currentMenu = E_UI_USER_SETTING_CARD_AUTH;
            enter_user_setting_card_auth(key);
            break;

        default:
            break;
    }
}


void draw_admin_setting(void)
{
    DisplayStr("管理设置", 0, 2);
    DisplayStr("1 密码", 1, 0);
    DisplayStr("2 时钟", 1, 4);
    DisplayStr("3 锁定", 2, 0);
    DisplayStr("4 查询", 2, 4);    
    DisplayStr("5 字母", 3, 0);
    DisplayStr("0 退出", 3, 4);
}

void enter_admin_setting(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

void action_admin_setting(uint8_t key)
{    
    draw_admin_setting();

    switch(key)
    {
        case 1:
            break;

        default:
            break;
    }
}


void draw_system_setting(void)
{
    DisplayStr("系统设置", 0, 2);
    DisplayStr("1 期限", 1, 0);
    DisplayStr("2 时段", 1, 4);
    DisplayStr("3 方式", 2, 0);
    DisplayStr("4 权限", 2, 4);    
    DisplayStr("5 箱数", 3, 0);
    DisplayStr("0 退出", 3, 4);
}

void enter_system_setting(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

void action_system_setting(uint8_t key)
{    
    draw_system_setting();

    switch(key)
    {
        case 1:
            break;

        default:
            break;
    }
}


void draw_box_setting(void)
{
    DisplayStr("开箱设置", 0, 2);
    DisplayStr("1 单开", 1, 0);
    DisplayStr("2 全开", 1, 4);
    DisplayStr("3 单清", 2, 0);
    DisplayStr("4 全清", 2, 4);    
    DisplayStr("5 语音", 3, 0);
    DisplayStr("0 退出", 3, 4);
}

void enter_box_setting(uint8_t key)
{
    LcdWcom(0x01);

    draw_box_setting();

    bsp_StartTimer(1, 10000, timeout);

    
}

void action_box_setting(uint8_t key)
{    
    

    switch(key)
    {
        case 1:
            break;

        default:
            break;
    }
}



void draw_user_setting_box_pw(void)
{
    uint8_t j = 0;

    if (boxPwSubState == E_BOX_PW_SUB_ROOM)
    {
        DisplayStr("请输入房间号", 0, 1);

        SetCursor(1,0);
        for(j = 0; j < lenRoomNum; j++)
            DisplayRoomNumber(roomNum[j]);
    }
    else if (boxPwSubState == E_BOX_PW_SUB_PW)
    {
        DisplayStr("请输入密码", 2, 1);
        
        SetCursor(3,0);
        for(j = 0; j < lenRoomPW; j++)
            LcdWdata('*');
    }
    else if (boxPwSubState == E_BOX_PW_SUB_PW_AGAIN)
    {
        DisplayStr("请再次输入密码", 2, 1);

        SetCursor(3,0);
        if (lenRoomPWAgain == 0)
        {
            for(j = 0; j < lenRoomPW; j++)
                LcdWdata(' ');
        }

        for(j = 0; j < lenRoomPWAgain; j++)
            LcdWdata('*');

    }

}

void enter_user_setting_box_pw(uint8_t key)
{
    if (key == 0xFF)
        return;

    boxPwSubState = E_BOX_PW_SUB_ROOM;        

    lenRoomNum = 0;
    memset(roomNum, 0, sizeof(roomNum));    


    lenRoomPW= 0;
    memset(roomPW, 0, sizeof(roomPW));


    lenRoomPWAgain= 0;
    memset(roomPWAgain, 0, sizeof(roomPWAgain));

    LcdWcom(0x01);
    draw_user_setting_box_pw();

    bsp_StartTimer(1, 15000, timeout);

}

void action_user_setting_box_pw(uint8_t key)
{    

    if (key == 0xFF)
        return;

    if (boxPwSubState == E_BOX_PW_SUB_ROOM)
    {
        if (key == 0x8F)
        {
            translateKey(roomNum, lenRoomNum);
        
            SetRoomNum(roomNum);

            boxPwSubState = E_BOX_PW_SUB_PW;
        }
        else
        {
            roomNum[lenRoomNum++] = key;
        }
    }
    else if (boxPwSubState == E_BOX_PW_SUB_PW)
    {
        
        if (key == 0x8F)
        {
            translateKey(roomPW, lenRoomPW);
        
            boxPwSubState = E_BOX_PW_SUB_PW_AGAIN;
        }
        else
        {
            roomPW[lenRoomPW++] = key;
        }
    }
    else if (boxPwSubState == E_BOX_PW_SUB_PW_AGAIN)
    {
        if (key == 0x8F)
        {
            translateKey(roomPWAgain, lenRoomPWAgain);
        
            if (lenRoomPW == lenRoomPWAgain && !memcmp(roomPW, roomPWAgain, lenRoomPW))
            {
                SetRoomPassword(0, roomPW);

                currentMenu = E_UI_USER_SETTING_BOX_PW_SUCCESS;
                enter_user_setting_box_pw_success(key);

                return;
            }
            else
            {
                // 两次密码输入不一致，设置失败
                currentMenu = E_UI_USER_SETTING_BOX_PW_FAILED;
                enter_user_setting_box_pw_failed(key);
            }
        }
        else
        {
            roomPWAgain[lenRoomPWAgain++] = key;
        }
    }
    
    draw_user_setting_box_pw();

}


void draw_user_setting_box_pw_success(void)
{
    DisplayStr("设置成功", 1, 2);
}

void enter_user_setting_box_pw_success(uint8_t key)
{    
    LcdWcom(0x01);
    bsp_StartTimer(1, 3000, timeout);
}

void action_user_setting_box_pw_success(uint8_t key)
{
    draw_user_setting_box_pw_success();
}


void draw_user_setting_box_pw_failed(void)
{
    DisplayStr("两次密码不一致", 1, 1);
    DisplayStr("设置失败", 2, 2);
}

void enter_user_setting_box_pw_failed(uint8_t key)
{    
    LcdWcom(0x01);
    bsp_StartTimer(1, 3000, timeout);
}

void action_user_setting_box_pw_failed(uint8_t key)
{
    draw_user_setting_box_pw_failed();
}


void draw_user_setting_card_auth(void)
{
    static uint8_t cnt = 0;

    cnt++;
    cnt %= 40;
    
    if (!(cnt / 30) )
    {
        DisplayStr("请读卡", 1, 2);
    }
    else
    {
        DisplayStr("      ", 1, 2);
    }
}

void enter_user_setting_card_auth(uint8_t key)
{    
    LcdWcom(0x01);
    bsp_StartTimer(1, 20000, timeout);
}

void action_user_setting_card_auth(uint8_t key)
{
    uint8_t UID[5];
    memset(UID, 0xFF, 5);

    draw_user_setting_card_auth();

    // scan card
    scanCard(UID);

    if (memcmp(UID, "\xFF\xFF\xFF\xFF\xFF", 5))
    {
        SetRoomCardID(0, UID);
    }
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

