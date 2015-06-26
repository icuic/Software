#include "stm32f10x.h"
#include "menu.h"
#include "Lcd12864.h"

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
}


void draw_open_success(void)
{
    //LcdWcom(0x01);
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
    if (menuTab[currentMenu].menu_action != 0)
        menuTab[currentMenu].menu_action(key);
}

