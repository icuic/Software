#ifndef _W25QXX_H_
#define _W25QXX_H_

#include "stm32f10x.h"

#define FLASH_PAGE_SIZE     (256)           // 256 bytes
#define FLASH_SECTOR_SIZE   (4 * 1024)      // 4K bytes
#define FLASH_BLOCK_SIZE    (64 * 1024)     // 64K bytes

void Flash_Init(void);
void Flash_Write_Enable(void);
void Flash_WaitWriteEnd(void);
u8   Flash_SendByte(u8 byte);
void Flash_EraseSector(u32 SectorAddress);
void Flash_PageWrite(u8 *pBuffer,u32 PageAddress,u16 WriteByteNum);
void Flash_EraseBlock(u32 BlockAddress);
void Flash_Read(u8 *pBuffer,u32 ReadAddress,u16 ReadByteNum);
void Flash_PowerDown(void);
void Flash_WakeUp(void);
u32  Flash_ReadID(void);
void Flash_EraseChip(void);





#endif
