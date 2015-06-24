#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f10x.h"

typedef struct __KEYVALUETABLE
{
    u8 KeyValueRead;
    u8 KeyString[10];
}KeyValueTable;

extern const KeyValueTable MyKey[];

void InitSPIcommon(void);
void InitKey(void);
u8 scankey(void) ; //只处理一个键按下情况

#endif
