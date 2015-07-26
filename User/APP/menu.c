#include "stm32f10x.h"
#include "menu.h"
#include "Lcd12864.h"
#include "bsp.h"
#include "bsp_timer.h"
#include <stdio.h>
#include "roomInfoManage.h"
//#include "string.h"


#define M_SOFT_TIMER_FOR_MENU   1
#define M_1_SECOND              1000
#define M_NEVER_GO_BACK         0xFFFF

#define M_DEFAULT_VALUE         0

extern void DisplayStr(u8 *str,u8 x,u8 y);
extern void DisplayKey(u8 keyvalue, u8 x, u8 y);

static void draw_welcome(void);
static void enter_welcome(uint8_t key);
static void action_welcome(uint8_t key);


static void draw_room_number(void);
static void enter_room_number(uint8_t key);
static void action_room_number(uint8_t key);


static void draw_room_pw(void);
static void enter_room_pw(uint8_t key);
static void action_room_pw(uint8_t key);


static void draw_room_invalid(void);
static void enter_room_invalid(uint8_t key);
static void action_room_invalid(uint8_t key);


static void draw_room_pw_error(void);
static void enter_room_pw_error(uint8_t key);
static void action_room_pw_error(uint8_t key);


static void draw_set_admin_card(void);
static void enter_set_admin_card(uint8_t key);
static void action_set_admin_card(uint8_t key);


static void draw_admin(void);
static void enter_admin(uint8_t key);
static void action_admin(uint8_t key);


static void draw_user_setting(void);
static void enter_user_setting(uint8_t key);
static void action_user_setting(uint8_t key);


static void draw_admin_setting(void);
static void enter_admin_setting(uint8_t key);
static void action_admin_setting(uint8_t key);


static void draw_system_setting(void);
static void enter_system_setting(uint8_t key);
static void action_system_setting(uint8_t key);


static void draw_box_setting(void);
static void enter_box_setting(uint8_t key);
static void action_box_setting(uint8_t key);


static void draw_user_setting_box_pw(void);
static void enter_user_setting_box_pw(uint8_t key);
static void action_user_setting_box_pw(uint8_t key);


static void draw_user_setting_box_pw_success(void);
static void enter_user_setting_box_pw_success(uint8_t key);
static void action_user_setting_box_pw_success(uint8_t key);


static void draw_user_setting_box_pw_failed(void);
static void enter_user_setting_box_pw_failed(uint8_t key);
static void action_user_setting_box_pw_failed(uint8_t key);


static void draw_user_setting_card_auth(void);
static void enter_user_setting_card_auth(uint8_t key);
static void action_user_setting_card_auth(uint8_t key);


static void draw_user_setting_box_num(void);
static void enter_user_setting_box_num(uint8_t key);
static void action_user_setting_box_num(uint8_t key);


static void draw_user_setting_card_clear(void);
static void enter_user_setting_card_clear(uint8_t key);
static void action_user_setting_card_clear(uint8_t key);


static void draw_card_clear_success(void);
static void enter_card_clear_success(uint8_t key);
static void action_card_clear_success(uint8_t key);


static void draw_tips(void);
static void enter_tips(uint8_t key);
static void action_tips(uint8_t key);


static UIStruct menuTab[E_UI_MAX] = 
{
    {
    E_UI_WELCOME,
    E_UI_WELCOME,
    M_NEVER_GO_BACK,
    draw_welcome,
    enter_welcome,
    action_welcome,
    },

    {
    E_UI_USER_ROOM_NUM,
    E_UI_WELCOME,
    10 * M_1_SECOND,
    draw_room_number,
    enter_room_number,
    action_room_number,
    },

    {
    E_UI_USER_PASSWORD,
    E_UI_WELCOME,    
    10 * M_1_SECOND,
    draw_room_pw,
    enter_room_pw,
    action_room_pw,
    },

    {
    E_UI_ROOM_INVALID,
    E_UI_WELCOME,
    2 * M_1_SECOND,
    draw_room_invalid,
    enter_room_invalid,
    action_room_invalid,
    },

    {
    E_UI_ROOM_PW_ERROR,
    E_UI_USER_PASSWORD,
    2 * M_1_SECOND,
    draw_room_pw_error,
    enter_room_pw_error,
    action_room_pw_error,
    }, 

    {
    E_UI_SET_ADMIN_CARD,
    E_UI_WELCOME,
    10 * M_1_SECOND,
    draw_set_admin_card,
    enter_set_admin_card,
    action_set_admin_card,
    },

    {
    E_UI_ADMIN,
    E_UI_WELCOME,
    10 * M_1_SECOND,
    draw_admin,
    enter_admin,
    action_admin,
    },
    
    {
    E_UI_USER_SETTING,
    E_UI_ADMIN,
    10 * M_1_SECOND,
    draw_user_setting,
    enter_user_setting,
    action_user_setting,
    },
    
    {
    E_UI_ADMIN_SETTING,
    E_UI_ADMIN,
    10 * M_1_SECOND,
    draw_admin_setting,
    enter_admin_setting,
    action_admin_setting,
    },

    {
    E_UI_SYSTEM_SETTING,
    E_UI_ADMIN,
    10 * M_1_SECOND,
    draw_system_setting,
    enter_system_setting,
    action_system_setting,
    },

    {
    E_UI_BOX_SETTING,
    E_UI_ADMIN,
    10 * M_1_SECOND,
    draw_box_setting,
    enter_box_setting,
    action_box_setting,
    },

    {
    E_UI_USER_SETTING_BOX_PW,
    E_UI_USER_SETTING,
    10 * M_1_SECOND,
    draw_user_setting_box_pw,
    enter_user_setting_box_pw,
    action_user_setting_box_pw,
    },

    {
    E_UI_USER_SETTING_BOX_PW_SUCCESS,
    E_UI_USER_SETTING,
    2 * M_1_SECOND,
    draw_user_setting_box_pw_success,
    enter_user_setting_box_pw_success,
    action_user_setting_box_pw_success,
    },

    {
    E_UI_USER_SETTING_BOX_PW_FAILED,
    E_UI_USER_SETTING,
    2 * M_1_SECOND,
    draw_user_setting_box_pw_failed,
    enter_user_setting_box_pw_failed,
    action_user_setting_box_pw_failed,    
    },

    {
    E_UI_USER_SETTING_CARD_AUTH,
    E_UI_USER_SETTING,
    10 * M_1_SECOND,
    draw_user_setting_card_auth,
    enter_user_setting_card_auth,
    action_user_setting_card_auth,
    },

    {
    E_UI_USER_SETTING_BOX_NUM,
    E_UI_USER_SETTING,
    20 * M_1_SECOND,
    draw_user_setting_box_num,
    enter_user_setting_box_num,
    action_user_setting_box_num,
    },

    {
    E_UI_USER_SETTING_CARD_CLEAR,
    E_UI_USER_SETTING,
    10 * M_1_SECOND,    
    draw_user_setting_card_clear,
    enter_user_setting_card_clear,
    action_user_setting_card_clear,
    },

    {
    E_UI_CARD_CLEAR_SUCCESS,
    E_UI_USER_SETTING,
    2 * M_1_SECOND,    
    draw_card_clear_success,
    enter_card_clear_success,
    action_card_clear_success,
    },

    {
    E_UI_TIPS,
    E_UI_WELCOME,
    2 * M_1_SECOND,    
    draw_tips,
    enter_tips,
    action_tips,
    },
};

typedef enum
{
    E_TIPS_TYPE_BOX_OPEN_SUCCESS = 0,       // 箱门已打开
    E_TIPS_TYPE_ROOM_NUM_INVALID,           // 无效房间号
    E_TIPS_TYPE_ROOM_PW_ERROR,              // 密码错误
    E_TIPS_TYPE_ROOM_PW_SETTING_SUCCESS,    // 房间密码设置成功
    E_TIPS_TYPE_CARD_DISAUTH_SUCCESS,       // 退卡成功
    E_TIPS_TYPE_CARD_INVALID,               // 无效卡
    E_TIPS_TYPE_END
}eTipsType;

typedef enum
{
    E_BOX_PW_SUB_ROOM = 0,      // 输入房间号
    E_BOX_PW_SUB_PW,            // 第一次输入密码
    E_BOX_PW_SUB_PW_AGAIN,      // 再次输入密码
    E_BOX_PW_END
}eBoxPwSubState;

typedef enum
{
    E_FOR_OPEN_BOX = 0,
    E_FOR_AUTH_CARD,
    E_FOR_CLEAR_CARD,
    E_FOR_END
}eMenuEnterFor;

static eBoxPwSubState subState = E_BOX_PW_SUB_ROOM;   


static volatile eUIIndex currentMenu = E_UI_WELCOME;
static eMenuEnterFor roomNumFor = E_FOR_OPEN_BOX;

static eTipsType tipsType = E_TIPS_TYPE_BOX_OPEN_SUCCESS;


u8 lockStat = 0;



//u8 DockCard[2][4] = 
//    {
//        {0x75,0x98,0x8a,0x52},
//        {0xD5,0x38,0x08,0xC0}
//};



static uint8_t lenRoomNum = 0;                          // length of room number
static uint8_t roomNum[M_ROOM_NUM_MAX_LENGTH];          // Room number

static uint8_t lenRoomPW = 0;                           // length of room password
static uint8_t roomPW[M_ROOM_PASSWORD_MAX_LENGTH];      // Room password

static uint8_t lenRoomPWAgain = 0;                      // length of room password
static uint8_t roomPWAgain[M_ROOM_PASSWORD_MAX_LENGTH]; // Room password

static uint8_t tmpIndex = 0;                            // 

static uint8_t matchedIndex = 0xFF;                     // 

static void timeout()
{
    /* Go back to parent menu once timeout */
    currentMenu = menuTab[currentMenu].parent;

    /* Clear LCD display */
    ClearDisplay();

    /* Init parameters for next menu's display */
    menuTab[currentMenu].menu_enter(M_KEY_NO_KEY);
}

static void scanCard(unsigned char* UID)
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

static void isAuthCard(uint8_t* uid)
{
#if 0
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

        if(j == 4)  /* Find */
        {
        
            //if(i ==0)
            //{
            //    OpenLock(0X0001);
            //}
            //else
            //{
            //    OpenLock(0X0002);
            //}
            OpenLock(1<<i);
            
            lockStat = 1;
            IS_TIMEOUT_1MS(LockPlus,0);
            currentMenu = E_UI_OPEN_SUCCESS;

            break;
        }
        else    /* Not find */
        {
            currentMenu = E_UI_CARD_INVALID;
        }
    }
#endif


    matchedIndex= matchCardID(uid);

    if (matchedIndex < M_MAX_BOX)
    {
        OpenLock(1<<matchedIndex);
        
        lockStat = 1;
        IS_TIMEOUT_1MS(LockPlus,0);

        currentMenu = E_UI_TIPS;
        tipsType = E_TIPS_TYPE_BOX_OPEN_SUCCESS;
    }
    else
    {
        tipsType = E_TIPS_TYPE_CARD_INVALID;
        currentMenu = E_UI_TIPS;
        enter_tips(M_KEY_NO_KEY);
    }
    

    if(menuTab[currentMenu].menu_enter != 0)
    {
        menuTab[currentMenu].menu_enter(0);
    }
}

static void DisplayDataTime(uint32_t TimeVar)
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


static void draw_welcome(void)
{   
#if 0
    uint8_t i = 0;
    uint8_t tmp[(M_MAX_BOX + 8) / 8 + 1];
    
    DisplayStr("欢迎使用", 0, 2);
    DisplayStr("请读卡", 2, 2);

    if (bitMapFlashDateError != 0)
    {
        sprintf(tmp, "%x", bitMapFlashDateError);

        //DisplayStr(tmp, 1, 0);
    }
#endif

    void DisplayPic();
    
    DisplayDataTime(RTC_GetCounter());
}

static void enter_welcome(uint8_t key)
{
    ClearDisplay();

    LcdWcom(0x34);  // 扩充指令
}

static void action_welcome(uint8_t key)
{
    uint8_t UID[M_CARD_ID_MAX_LENGTH];
    memset(UID, M_DEFAULT_VALUE, M_CARD_ID_MAX_LENGTH);

    draw_welcome();

    /* scan card */
    scanCard(UID);

    if (memcmp(UID, "\x00\x00\x00\x00\x00\x00", M_CARD_ID_MAX_LENGTH))
    {
        isAuthCard(UID);
    }

    // admin
    if (!memcmp(adminCardID, UID, M_CARD_ID_MAX_LENGTH))
    {
        Beep(500);
    
        currentMenu = E_UI_ADMIN;
        enter_admin(key);
    }

    if(isNumKey(key))
    {
        currentMenu = E_UI_USER_ROOM_NUM;
        roomNumFor = E_FOR_OPEN_BOX;
        enter_room_number(key);
    }
}


/* --------------------------------------------------------------- */
static void DisplayRoomNumber(uint8_t keyvalue)
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


/* --------------------------------------------------------------- */
static void draw_room_number(void)
{
    uint8_t j = 0;

    DisplayStr("请输入房间号", 0, 1);

    SetCursor(2,0);
    for(j = 0; j < lenRoomNum; j++)
        DisplayRoomNumber(roomNum[j]);

}

static void enter_room_number(uint8_t key)
{
    lenRoomNum = 0;
    memset(roomNum, 0, sizeof(roomNum));
    matchedIndex = 0xFF;

    if (isNumKey(key) && roomNumFor == E_FOR_OPEN_BOX)
    {   
        roomNum[lenRoomNum++] = key;
    }

    
    ClearDisplay();
    draw_room_number();

    bsp_StartTimer(M_SOFT_TIMER_FOR_MENU, menuTab[currentMenu].timeout, timeout);
}



static void translateKey(uint8_t *key, uint8_t len)
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


static void action_room_number(uint8_t key)
{
    if (key == M_KEY_NO_KEY)
        return;

    switch(key)
    {
        case M_KEY_ESC:     /* Delete a number */
        {
            if (lenRoomNum > 0)
            {
                lenRoomNum--;
                roomNum[lenRoomNum] = 0;
            }

            ClearDisplay();
            draw_room_number();
        }
            break;

        case M_KEY_0:
        case M_KEY_1:
        case M_KEY_2:
        case M_KEY_3:
        case M_KEY_4:
        case M_KEY_5:
        case M_KEY_6:
        case M_KEY_7:
        case M_KEY_8:
        case M_KEY_9:
        {
            roomNum[lenRoomNum++] = key;
            
            draw_room_number();
        }
            break;

        case M_KEY_OK:
        {
            translateKey(roomNum, lenRoomNum);

            if (strcmp(roomNum, "888", lenRoomNum) == 0)            /* Enter administer card setting menu */
            {
                currentMenu = E_UI_SET_ADMIN_CARD;
            
                ClearDisplay();
                enter_set_admin_card(key);
            }
            else                                                    /* It's a valid room number */
            {
                matchedIndex = matchRoomNum(roomNum, lenRoomNum);

                if (matchedIndex < M_MAX_BOX)
                {
                    ClearDisplay();
                
                    if (roomNumFor == E_FOR_OPEN_BOX)
                    {
                        currentMenu = E_UI_USER_PASSWORD;
                        enter_room_pw(key);
                    }
                    else if (roomNumFor == E_FOR_AUTH_CARD) 
                    {
                        roomNumFor = E_FOR_OPEN_BOX;
                        currentMenu = E_UI_USER_SETTING_CARD_AUTH;
                        enter_user_setting_card_auth(key);
                    }
                    else if (roomNumFor == E_FOR_CLEAR_CARD)
                    {
                        roomNumFor = E_FOR_OPEN_BOX;
                        currentMenu = E_UI_USER_SETTING_CARD_CLEAR;
                        enter_user_setting_card_clear(key);
                    }
                    
                }
                else                                                    /* It's a invalid room number */
                {
                    currentMenu = E_UI_ROOM_INVALID;
                
                    ClearDisplay();
                    enter_room_invalid(key);
                }
            }
        }
            break;

        default:
            break;   
    }
}

/* --------------------------------------------------------------- */
static void draw_room_pw(void)
{
    uint8_t j = 0;

    DisplayStr("请输入密码", 0, 1);

    SetCursor(2,0);
    for(j = 0; j < lenRoomPW; j++)
        LcdWdata('*');

}

static void enter_room_pw(uint8_t key)
{
    //if (key == 0xFF)
    //    return;

    lenRoomPW= 0;
    memset(roomPW, 0, sizeof(roomPW));
    
    LcdWcom(0x01);
    draw_room_pw();

    bsp_StartTimer(1, 15000, timeout);
}

static void action_room_pw(uint8_t key)
{
    if (key == M_KEY_NO_KEY)
        return;


    switch(key)
    {
        case M_KEY_ESC:     /* Delete a number */
        {
            if (lenRoomPW > 0)
            {
                lenRoomPW--;
                roomPW[lenRoomPW] = 0;
            }

            ClearDisplay();
            draw_room_pw();
        }
            break;


        case M_KEY_0:
        case M_KEY_1:
        case M_KEY_2:
        case M_KEY_3:
        case M_KEY_4:
        case M_KEY_5:
        case M_KEY_6:
        case M_KEY_7:
        case M_KEY_8:
        case M_KEY_9:
        {
            roomPW[lenRoomPW++] = key;
            
            draw_room_pw();

        }
            break;
            
        case M_KEY_OK:
        {
            translateKey(roomPW, lenRoomPW);
        
            if (matchRoomPassword(matchedIndex, roomPW, lenRoomPW))
            {
                //OpenLock(0X0002);
                OpenLock(1<<matchedIndex);

                lockStat = 1;
                IS_TIMEOUT_1MS(LockPlus,0);

            
                currentMenu = E_UI_TIPS;
                tipsType = E_TIPS_TYPE_BOX_OPEN_SUCCESS;

                enter_tips(key);
            }
            else
            {
                currentMenu = E_UI_ROOM_PW_ERROR;

                enter_room_pw_error(key);
            }
        }
            break;
        
        default:
            break;   
    }
    
}


static void draw_room_invalid(void)
{
    DisplayStr("无效房间号", 2, 0);
}

static void enter_room_invalid(uint8_t key)
{
    ClearDisplay();

    switch(roomNumFor)
    {
        case E_FOR_OPEN_BOX:
        {
            menuTab[E_UI_ROOM_INVALID].parent = E_UI_WELCOME;
        }
        break;

        case E_FOR_AUTH_CARD:
        case E_FOR_CLEAR_CARD:
        {
            menuTab[E_UI_ROOM_INVALID].parent = E_UI_USER_SETTING;
        }
        break;

        default:
        break;
    }

    bsp_StartTimer(1, menuTab[currentMenu].timeout, timeout);
}

static void action_room_invalid(uint8_t key)
{
    draw_room_invalid();
}


static void draw_room_pw_error(void)
{
    DisplayStr("密码错误", 2, 0);
}

static void enter_room_pw_error(uint8_t key)
{
    ClearDisplay();

    bsp_StartTimer(1, 2000, timeout);
}

static void action_room_pw_error(uint8_t key)
{
    draw_room_pw_error();
}


static void draw_set_admin_card(void)
{
    uint8_t tmps[6];

    DisplayStr("请刷管理员卡", 1, 1);
    
    sprintf(tmps,"%02X%02X%02X%02X",adminCardID[0],adminCardID[1],adminCardID[2],adminCardID[3]);
    DisplayStr(tmps,3,0);

}

static void enter_set_admin_card(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 5000, timeout);
}

static void action_set_admin_card(uint8_t key)
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
        writeAdminCardIDToFlash(UID);
    }
}


static void draw_admin(void)
{
    DisplayStr("设置", 0, 3);
    DisplayStr("1 用户", 1, 0);
    DisplayStr("2 管理", 1, 4);
    DisplayStr("3 系统", 2, 0);
    DisplayStr("4 箱门", 2, 4);    
    DisplayStr("0 退出", 3, 0);
}

static void enter_admin(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

static void action_admin(uint8_t key)
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


static void draw_user_setting(void)
{
    DisplayStr("用户设置", 0, 2);
    DisplayStr("1 箱密", 1, 0);
    DisplayStr("2 房号", 1, 4);
    DisplayStr("3 授卡", 2, 0);
    DisplayStr("4 退卡", 2, 4);    
    DisplayStr("0 退出", 3, 0);
}

static void enter_user_setting(uint8_t key)
{
    lenRoomNum = 0;
    memset(roomNum, 0, sizeof(roomNum));
    matchedIndex = 0xFF;

    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

static void action_user_setting(uint8_t key)
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

        case '2':
            currentMenu = E_UI_USER_SETTING_BOX_NUM;
            enter_user_setting_box_num(key);
            break;

        case '3':
            roomNumFor = E_FOR_AUTH_CARD;
            currentMenu = E_UI_USER_ROOM_NUM;
            enter_room_number(key);
            break;

        case '4':
            roomNumFor = E_FOR_CLEAR_CARD;
            currentMenu = E_UI_USER_ROOM_NUM;
            enter_room_number(key);
            break;

        case '0':
            currentMenu = E_UI_ADMIN;
            enter_admin(key);
            break;

        default:
            break;
    }
}


static void draw_admin_setting(void)
{
    DisplayStr("管理设置", 0, 2);
    DisplayStr("1 密码", 1, 0);
    DisplayStr("2 时钟", 1, 4);
    DisplayStr("3 锁定", 2, 0);
    DisplayStr("4 查询", 2, 4);    
    DisplayStr("5 字母", 3, 0);
    DisplayStr("0 退出", 3, 4);
}

static void enter_admin_setting(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

static void action_admin_setting(uint8_t key)
{    
    uint8_t tmp = key; 
    
    draw_admin_setting();

    translateKey(&tmp, 1);

    switch(tmp)
    {
        case '1':
            break;

        case '2':
            break;

        case '3':
            break;

        case '4':
            break;

        case '5':
            break;

        case '0':
            currentMenu = E_UI_ADMIN;
            enter_admin(key);
            break;


        default:
            break;
    }
}


static void draw_system_setting(void)
{
    DisplayStr("系统设置", 0, 2);
    DisplayStr("1 期限", 1, 0);
    DisplayStr("2 时段", 1, 4);
    DisplayStr("3 方式", 2, 0);
    DisplayStr("4 权限", 2, 4);    
    DisplayStr("5 箱数", 3, 0);
    DisplayStr("0 退出", 3, 4);
}

static void enter_system_setting(uint8_t key)
{
    LcdWcom(0x01);

    bsp_StartTimer(1, 10000, timeout);
}

static void action_system_setting(uint8_t key)
{    
    draw_system_setting();

    switch(key)
    {
        case M_KEY_1:
            break;

        case M_KEY_2:
            break;

        case M_KEY_3:
            break;

        case M_KEY_4:
            break;

        case M_KEY_5:
            break;

        case M_KEY_0:
            currentMenu = E_UI_ADMIN;
            enter_admin(key);
            break;

        default:
            break;
    }
}


static void draw_box_setting(void)
{
    DisplayStr("开箱设置", 0, 2);
    DisplayStr("1 单开", 1, 0);
    DisplayStr("2 全开", 1, 4);
    DisplayStr("3 单清", 2, 0);
    DisplayStr("4 全清", 2, 4);    
    DisplayStr("5 语音", 3, 0);
    DisplayStr("0 退出", 3, 4);
}

static void enter_box_setting(uint8_t key)
{
    LcdWcom(0x01);

    draw_box_setting();

    bsp_StartTimer(1, 10000, timeout);

    
}

static void action_box_setting(uint8_t key)
{    
    

    switch(key)
    {
        case M_KEY_1:
            break;

        case M_KEY_2:
            break;

        case M_KEY_3:
            break;

        case M_KEY_4:
            break;

        case M_KEY_5:
            break;

        case M_KEY_0:
            currentMenu = E_UI_ADMIN;
            enter_admin(key);
            break;

        default:
            break;
    }
}



static void draw_user_setting_box_pw(void)
{
    uint8_t j = 0;

    if (subState == E_BOX_PW_SUB_ROOM)
    {
        DisplayStr("请输入房间号", 0, 1);

        SetCursor(1,0);
        for(j = 0; j < lenRoomNum; j++)
            DisplayRoomNumber(roomNum[j]);
    }
    else if (subState == E_BOX_PW_SUB_PW)
    {
        DisplayStr("请输入密码", 2, 1);
        
        SetCursor(3,0);
        for(j = 0; j < lenRoomPW; j++)
            LcdWdata('*');
    }
    else if (subState == E_BOX_PW_SUB_PW_AGAIN)
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

static void enter_user_setting_box_pw(uint8_t key)
{
    if (key != M_KEY_1)
        return;

    subState = E_BOX_PW_SUB_ROOM; 

    matchedIndex = 0xFF;

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

static void action_user_setting_box_pw(uint8_t key)
{    

    if (key == M_KEY_NO_KEY)
        return;

    if (subState == E_BOX_PW_SUB_ROOM)
    {
        if (key == M_KEY_OK)
        {
            translateKey(roomNum, lenRoomNum);

            matchedIndex = matchRoomNum(roomNum, lenRoomNum);

            if(matchedIndex < M_MAX_BOX)
            {
                SetRoomNum(matchedIndex, roomNum);
                
                subState = E_BOX_PW_SUB_PW;
            }
            else
            {
                currentMenu = E_UI_ROOM_INVALID;

                enter_room_invalid(key);
            }

        }
        else
        {
            roomNum[lenRoomNum++] = key;
        }
    }
    else if (subState == E_BOX_PW_SUB_PW)
    {
        
        if (key == M_KEY_OK)
        {
            translateKey(roomPW, lenRoomPW);
        
            subState = E_BOX_PW_SUB_PW_AGAIN;
        }
        else
        {
            roomPW[lenRoomPW++] = key;
        }
    }
    else if (subState == E_BOX_PW_SUB_PW_AGAIN)
    {
        if (key == M_KEY_OK)
        {
            translateKey(roomPWAgain, lenRoomPWAgain);
        
            if (lenRoomPW == lenRoomPWAgain && !memcmp(roomPW, roomPWAgain, lenRoomPW))
            {
                SetRoomPassword(matchedIndex, roomPW);

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


static void draw_user_setting_box_pw_success(void)
{
    DisplayStr("设置成功", 1, 2);
}

static void enter_user_setting_box_pw_success(uint8_t key)
{    
    LcdWcom(0x01);
    bsp_StartTimer(1, 3000, timeout);
}

static void action_user_setting_box_pw_success(uint8_t key)
{
    draw_user_setting_box_pw_success();
}


static void draw_user_setting_box_pw_failed(void)
{
    DisplayStr("两次密码不一致", 1, 1);
    DisplayStr("设置失败", 2, 2);
}

static void enter_user_setting_box_pw_failed(uint8_t key)
{    
    LcdWcom(0x01);
    bsp_StartTimer(1, 3000, timeout);
}

static void action_user_setting_box_pw_failed(uint8_t key)
{
    draw_user_setting_box_pw_failed();
}


static void draw_user_setting_card_auth(void)
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

static void enter_user_setting_card_auth(uint8_t key)
{    
    LcdWcom(0x01);
    bsp_StartTimer(1, 20000, timeout);
}

static void action_user_setting_card_auth(uint8_t key)
{
    uint8_t UID[5], tmps[5];
    memset(UID, 0xFF, 5);

    draw_user_setting_card_auth();

    // scan card
    scanCard(UID);

    if (memcmp(UID, "\xFF\xFF\xFF\xFF\xFF", 5))
    {
        SetRoomCardID(matchedIndex, UID);
    }
}


static void draw_user_setting_box_num(void)
{
    uint8_t j = 0;
    uint8_t tmps[3] = {'\0', '\0', '\0'};

    DisplayStr("请输入对应房间号", 0, 0);

    /* Control line */
    if (tmpIndex < M_MAX_BOX)
    {
        sprintf(tmps,"%02d",tmpIndex);
        DisplayStr(tmps, 2, 0);
    }


    SetCursor(2,6);


    switch(lenRoomNum)
    {
        case 0:         /* Display stored room number */
        {
            for(j = 0; j < 3; j++)
            {
                tmps[0] = *(((uint8_t *)(roomInfo[tmpIndex].number)) + j);
                if (tmps[0] != 0)
                {
                    LcdWdata(tmps[0]);
                }
                else if (lenRoomNum == 0)
                {
                    LcdWdata(' ');
                }
            }

        }        
            break;

        case 1:         /* Clear, no break */
        {
            for(j = 0; j < 3; j++)
            {
                LcdWdata(' ');
            }

            SetCursor(2,6);
        }
        
        default:
        {
            for(j = 0; j < lenRoomNum; j++)
            {
                DisplayRoomNumber(roomNum[j]);
            }

        }
            break;
    }
    

    DisplayStr("<- [*]    [#] ->", 3, 0);
}

static void enter_user_setting_box_num(uint8_t key)
{   
    if (key == M_KEY_2) /* Enter this menu form the upper */
        tmpIndex = 0;

    lenRoomNum = 0;
    memset(roomNum, 0, sizeof(roomNum));

    ClearDisplay();
    draw_user_setting_box_num();
    
    bsp_StartTimer(1, 20000, timeout);
}

static void action_user_setting_box_num(uint8_t key)
{

    if (key == M_KEY_NO_KEY)
        return;

    switch(key)
    {
        case M_KEY_PRE:      /* 上一页 */
        {
            if (tmpIndex == 0)
                tmpIndex = M_MAX_BOX - 1;
            else
                tmpIndex--;

            enter_user_setting_box_num(key);
        }
            break;

        case M_KEY_NEXT:      /* 下一页 */
        {
            if (tmpIndex == M_MAX_BOX - 1)
                tmpIndex = 0;
            else
                tmpIndex++;

            enter_user_setting_box_num(key);
        }
            break;

        case M_KEY_OK:      /* 确定 */
        {
            translateKey(roomNum, lenRoomNum);

            SetRoomNum(tmpIndex, roomNum);
            
            tmpIndex++;

            enter_user_setting_box_num(key);

        }
            break;

        case M_KEY_0:       /* 数字 */
        case M_KEY_1:
        case M_KEY_2:
        case M_KEY_3:
        case M_KEY_4:
        case M_KEY_5:
        case M_KEY_6:
        case M_KEY_7:
        case M_KEY_8:
        case M_KEY_9:
        {
            roomNum[lenRoomNum++] = key;
            
            draw_user_setting_box_num();
        }
            break;

        default:        /* 其它 */
            break;
    }

}


static void draw_user_setting_card_clear(void)
{

    DisplayStr("退卡", 0, 3);

    DisplayStr("1.确定", 3, 0);
    DisplayStr("0.取消", 3, 5);
    
}

static void enter_user_setting_card_clear(uint8_t key)
{    
    ClearDisplay();
    
    bsp_StartTimer(1, menuTab[currentMenu].timeout, timeout);
}

static void action_user_setting_card_clear(uint8_t key)
{

    draw_user_setting_card_clear();

    switch(key)
    {
        case M_KEY_0:
        {
            currentMenu = E_UI_USER_SETTING;
            enter_user_setting(key);
        }
        break;

        case M_KEY_1:
        {
            clearCard(matchedIndex);

            tipsType = E_TIPS_TYPE_CARD_DISAUTH_SUCCESS;
            currentMenu = E_UI_TIPS;
            enter_tips(key);
        }
        break;

        default:
        break;
    }
}


static void draw_card_clear_success(void)
{
    DisplayStr("退卡成功", 1, 2);
}

static void enter_card_clear_success(uint8_t key)
{
    ClearDisplay();

    bsp_StartTimer(M_SOFT_TIMER_FOR_MENU, menuTab[currentMenu].timeout, timeout);
}

static void action_card_clear_success(uint8_t key)
{
    draw_card_clear_success();
}


static void draw_tips(void)
{
    switch(tipsType)
    {
        case E_TIPS_TYPE_BOX_OPEN_SUCCESS:
        {
            uint8_t tmp[M_ROOM_NUM_MAX_LENGTH] = 0;
            sprintf(tmp,"%s",roomInfo[matchedIndex].number);
            DisplayStr(tmp,0,0);
            
            DisplayStr("箱门已打开", 0, 2);
            DisplayStr("请随手关闭箱门", 2, 0);
        }
        break;

        case E_TIPS_TYPE_ROOM_NUM_INVALID:
        break;

        case E_TIPS_TYPE_ROOM_PW_ERROR:
        break;

        case E_TIPS_TYPE_ROOM_PW_SETTING_SUCCESS:
        break;

        case E_TIPS_TYPE_CARD_DISAUTH_SUCCESS:
            DisplayStr("退卡成功", 1, 2);
        break;

        case E_TIPS_TYPE_CARD_INVALID:
            DisplayStr("无效卡", 1, 2);
        break;

        default:
        break;        
    }
}

static void enter_tips(uint8_t key)
{
    switch(tipsType)
    {
        case E_TIPS_TYPE_BOX_OPEN_SUCCESS:
            menuTab[currentMenu].parent = E_UI_WELCOME;
        break;

        case E_TIPS_TYPE_ROOM_NUM_INVALID:
        break;

        case E_TIPS_TYPE_ROOM_PW_ERROR:
        break;

        case E_TIPS_TYPE_ROOM_PW_SETTING_SUCCESS:
        break;

        case E_TIPS_TYPE_CARD_DISAUTH_SUCCESS:
            menuTab[currentMenu].parent = E_UI_USER_SETTING;
        break;

        case E_TIPS_TYPE_CARD_INVALID:
            menuTab[currentMenu].parent = E_UI_WELCOME;
        break;

        default:
        break;        
    }


    ClearDisplay();

    bsp_StartTimer(M_SOFT_TIMER_FOR_MENU, menuTab[currentMenu].timeout, timeout);
}

static void action_tips(uint8_t key)
{
    draw_tips();
}



void menu_handle(uint8_t key)
{
    /* Soft timer 1 used for menu only */
    bsp_CheckTimer(M_SOFT_TIMER_FOR_MENU);

    /* Menu display and react to the key pressed */
    if (menuTab[currentMenu].menu_action != 0)
    {
        menuTab[currentMenu].menu_action(key);
    }

    /* Reset Soft timer 1 once there is a key pressed */
    if (key != M_KEY_NO_KEY)
    {
        bsp_StartTimer(M_SOFT_TIMER_FOR_MENU, menuTab[currentMenu].timeout, timeout);
    }    
}

