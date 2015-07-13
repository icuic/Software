#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f10x.h"


#define M_KEY_0         0xfa
#define M_KEY_1         0xf0
#define M_KEY_2         0xf8
#define M_KEY_3         0x6f
#define M_KEY_4         0xef
#define M_KEY_5         0xf4
#define M_KEY_6         0xfc
#define M_KEY_7         0x2f
#define M_KEY_8         0xaf
#define M_KEY_9         0xf2
#define M_KEY_OK        0x8f
#define M_KEY_ESC       0x0f   
#define M_KEY_PRE       0x4f
#define M_KEY_NEXT      0xcf
#define M_KEY_NO_KEY    0xFF

#define isNumKey(key)   ((key == M_KEY_0) || (key == M_KEY_1) || (key == M_KEY_2) || \
                         (key == M_KEY_3) || (key == M_KEY_4) || (key == M_KEY_5) || \
                         (key == M_KEY_6) || (key == M_KEY_7) || (key == M_KEY_8) || \
                         (key == M_KEY_9))


typedef struct __KEYVALUETABLE
{
    u8 KeyValueRead;
    u8 KeyString[10];
    u8 NumASCII;
    u8 CharASCII;
}KeyValueTable;

extern const KeyValueTable MyKey[];
extern uint8_t KeyCode;


void InitSPIcommon(void);
void InitKey(void);
u8 scankey(void) ; //只处理一个键按下情况

#endif
