#ifndef _menu_h_
#define _menu_h_
#include "stm32f10x.h"

typedef enum
{
    E_UI_WELCOME = 0,
    //E_UI_ADMIN_PASSWORD,
    E_UI_USER_ROOM_NUM,
    E_UI_USER_PASSWORD,

    E_UI_ROOM_INVALID,
    E_UI_ROOM_PW_ERROR, // password error

    E_UI_SET_ADMIN_CARD,
    E_UI_ADMIN,

    E_UI_USER_SETTING,
    E_UI_ADMIN_SETTING,
    E_UI_SYSTEM_SETTING,
    E_UI_BOX_SETTING,      // 开箱选择

    E_UI_USER_SETTING_BOX_PW,   // 设置箱门密码

    E_UI_USER_SETTING_BOX_PW_SUCCESS,
    E_UI_USER_SETTING_BOX_PW_FAILED,

    E_UI_USER_SETTING_CARD_AUTH,    // 授卡

    E_UI_USER_SETTING_BOX_NUM,      // 设置房间号与锁的对应关系

    E_UI_USER_SETTING_CARD_CLEAR,   //  退卡

    E_UI_CARD_CLEAR_SUCCESS,

    E_UI_TIPS,                      // 操作结果提示

    E_UI_SURE_TO_AUTH_CARD,

    E_UI_ADMIN_SETTING_CLOCK,       // 设置时钟

    E_UI_ADMIN_SEETING_PW,          // 设置管理员密码

    E_UI_BOX_SETTING_OPEN_ONE_BY_ONE,      // 单独开某一箱门，输入箱号

    E_UI_MAX
}eUIIndex;

typedef struct
{
    eUIIndex number;
    eUIIndex parent;
    uint32_t timeout;
    void (*menu_draw)(void);
    void (*menu_enter)(uint8_t triggerdKey);
    void (*menu_action)(uint8_t triggeredKey);
}UIStruct;

void menu_handle(uint8_t key);


#endif
