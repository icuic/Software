
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "FlashPrimitives.h"

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#elif defined STM32F10X_HD
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#else 
 #error "Please select first the STM32 device"    
#endif

#define FLASH_PAGE_SIZE   PAGE_SIZE
#define FLASH_PAGE_NUM    2

#define StartAddrNum 0x0800f800
#define StartAddr  (u32)##StartAddrNum
#define EndAddr    0x0800ffff


#if (StartAddrNum + FLASH_PAGE_SIZE*FLASH_PAGE_NUM - 1>EndAddr)
#error "StartAddr ERROR"
#endif


#define MaxEepromNum 5

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
u8 CurrentPageInde = 0;
//-----------------------------------------------------------------------------
// FLASH Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// This routine reads a <byte> from the linear FLASH address <addr>.
//
#define FLASH_HalfWordRead_t(addr) (*(u16 *)(addr))


u8 InitEeprom(void)
{
    u16 flag,i;
    volatile FLASH_Status FLASHStatus;

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);

    for(i=0;i<FLASH_PAGE_NUM;i++)
    {
        flag = FLASH_HalfWordRead_t(StartAddr + i*PAGE_SIZE);
        if(flag == 0x55aa)
        {
            break;
        }
    }

    if(i == FLASH_PAGE_NUM)
    {
        CurrentPageInde = 0;
        
        for(i=0;i<FLASH_PAGE_NUM;i++)
        {
            FLASH_ErasePage(StartAddr + (FLASH_PAGE_SIZE * i));
        }

        FLASHStatus = FLASH_ProgramHalfWord(StartAddr,0x55aa);
        if(FLASHStatus != FLASH_COMPLETE)
        {
        	return 1;  
        }
        else
        {
            return 0;
        }
    }
    else
    {
        
		CurrentPageInde = i;
		return 0;  
    }
}

u8 EEPROM_Write(u8 addr,u16 Data)
{
	u16 i,temp;
	u32 addrt;
    u8 tempIndex;

	for(i=0;i<(PAGE_SIZE/4);i++)
	{
		addrt = StartAddr + CurrentPageInde*(u32)PAGE_SIZE + i*4;
		temp = (*(u16 *)(addrt));
        
		if(temp == (u16)0xffff)	
		{
            //find valid address
			FLASH_ProgramHalfWord(addrt,addr);	
			FLASH_ProgramHalfWord(addrt+2,Data);
			return 0;
		}
	}
    
	tempIndex = CurrentPageInde;
        
    if(tempIndex == (FLASH_PAGE_NUM - 1))
    {
        tempIndex = 0;
    }
    else
    {
        tempIndex++;
    }

    addrt = StartAddr + (FLASH_PAGE_SIZE * tempIndex);
    FLASH_ErasePage(addrt);
    //write valid flag
    FLASH_ProgramHalfWord(addrt,0x55aa);
	addrt += 4;

    FLASH_ProgramHalfWord(StartAddr + (u32)(CurrentPageInde * FLASH_PAGE_SIZE),0x00);
    
    for(i=0;i<addr;i++)
    {
        if(EEPROM_Read(i,&temp) == 0)
		{
	        FLASH_ProgramHalfWord(addrt,i);	
			FLASH_ProgramHalfWord(addrt+2,temp);
	        addrt += 4;
		}
    }

    FLASH_ProgramHalfWord(addrt,addr);	
	FLASH_ProgramHalfWord(addrt+2,Data);

    addrt += 4;

    for(i=addr+1;i<MaxEepromNum;i++)
    {
        if(EEPROM_Read(i,&temp) == 0)
		{
	        FLASH_ProgramHalfWord(addrt,i);	
			FLASH_ProgramHalfWord(addrt+2,temp);
	        addrt += 4;
		}
    }

    CurrentPageInde = tempIndex;
    return 0;
}

u8  EEPROM_Read(u8 addr,u16 *data)
{
	u16 i,datatemp;
	u32 addrt;

	for(i=0;i<(PAGE_SIZE/4);i++)
	{
		addrt = StartAddr + (CurrentPageInde + 1)*PAGE_SIZE - (i+1)*4;
        
        datatemp = FLASH_HalfWordRead_t(addrt);
      
		if(addr==datatemp)	//ÕÒµ½Êý¾Ý
		{	
			*data = FLASH_HalfWordRead_t(addrt + 2);
            return 0;
		}
	}

	return 1;
}
