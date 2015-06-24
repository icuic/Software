#ifndef _DrvTime2_h_
#define _DrvTime2_h_

#ifndef NULL
#define NULL        ((void *)0)
#endif





enum {
    LockPlus,
    Keyscan,
    eTimUpdata,
    eTimYModem,
    eTimMax,
};



//如果index对应的时间超时，清零并返回1，未超时返回0
#define IS_TIMEOUT_1MS(index, count)    ((g_Tim2Array[(char)(index)] >= (count))?  \
                                        ((g_Tim2Array[(char)(index)] = 0) == 0): 0)


extern volatile int g_Tim2Array[(char)eTimMax];

void BspTim2Init(void);
void BspTim2Close(void);




#endif
/*********************************** END **************************************/


