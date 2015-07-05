
#ifndef _bsp_h_
#define _bsp_h_

#include "BspTime2.h"
//#include "BspTime3.h"
//#include "BspUsart1.h"
#include "InOutcontrol.h"
#include "Lcd12864.h"
#include "key.h"
#include "NRF522.h"
#include "rtc.h"
#include "eeprom.h"
#include "beep.h"

#ifndef NULL
#define NULL        ((void *)0)
#endif


void BspInit(void);
void NVIC_SRAM(void);




#endif

/*********************************** END **************************************/



