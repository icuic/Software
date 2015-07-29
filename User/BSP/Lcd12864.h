#ifndef _LCD12864_h_
#define _LCD12864_h_
#include "stm32f10x.h"


typedef enum
{
    E_CUSTOM_STR_READ_CARD_PLEASE = 0,
    E_CUSTOM_STR_ROOM_HAS_OPEN,
    E_CUSTOM_STR_CLOSE_THE_ROOM_PLEASE,
}eCustomString;

void DisplayInit(void);
void SetCursor(unsigned char x,unsigned char y);
void LcdWdata(unsigned char WData);
void LcdWcom(unsigned char WCom);
void ClearDisplay(void);
void DisplayPic(void);
void DisplayCustomStr(eCustomString eStr);
void LCD_Clear_GDRAM(void);


#endif
