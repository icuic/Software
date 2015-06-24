#ifndef _LCD12864_h_
#define _LCD12864_h_
#include "stm32f10x.h"

void DisplayInit(void);
void SetCursor(unsigned char x,unsigned char y);
void LcdWdata(unsigned char WData);

#endif
