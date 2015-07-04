#ifndef _menu_h_
#define _menu_h_


typedef enum
{
    E_UI_WELCOME = 0,
    //E_UI_ADMIN_PASSWORD,
    E_UI_USER_ROOM_NUM,
    E_UI_USER_PASSWORD,
    E_UI_OPEN_SUCCESS,
    E_UI_ROOM_INVALID,
    E_UI_ROOM_PW_ERROR, // password error

    E_UI_SET_ADMIN_CARD,
    E_UI_ADMIN,

    E_UI_USER_SETTING,
    E_UI_ADMIN_SETTING,
    E_UI_SYSTEM_SETTING,
    E_UI_BOX_SETTING,      // ¿ªÏäÑ¡Ôñ
    E_UI_MAX
}eUIIndex;

typedef struct
{
    eUIIndex number;
    eUIIndex parent;
    void (*menu_draw)(void);
    void (*menu_enter)(uint8_t triggerdKey);
    void (*menu_action)(uint8_t triggeredKey);
}UIStruct;

void menu_handle(uint8_t key);


#endif
