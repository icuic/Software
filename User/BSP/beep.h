#ifndef _BEEP_H_
#define _BEEP_H_

#include "stm32f10x.h"
#include "bsp_timer.h"

void BeepInit(void);
void Beep(uint8_t ms);

#endif
