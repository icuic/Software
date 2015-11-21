#ifndef _INOUTCONTROL_h_
#define _INOUTCONTROL_h_

#include "stm32f10x.h"



void InOutControlInit(void);
//u8 SPIByte(u8 byte);
//void OpenLock(u8 * LockId,u8 Numer);
void OpenLock(u8 LockId, u8 Numer);

//void ReadKeyValue(void);



#endif
