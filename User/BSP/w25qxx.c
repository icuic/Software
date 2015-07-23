/**********************************rights cpoy (c)********************************               
** file name : flash.c
** author    : snail
** date      : 2013-11-2
** version   :          v1.0
** description:  W25Q80 SPI FLASH driving program          
***********************************************************************************/
#include "w25qxx.h"
 
#define Flash_CS_Low    GPIO_ResetBits(GPIOA, GPIO_Pin_11)
#define Flash_CS_High   GPIO_SetBits(GPIOA, GPIO_Pin_11)
 
/*---------------------------------------------------------------------------------
* Function Name  : Flash_Init
* Description    : Config the flash GPIO and flash 
* Input          : None
* Output         : None
* Return         : None
* Attention  : None
*-------------------------------------------------------------------------------*/
void Flash_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //CS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#if 1      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //SCLK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15; //MISO
    GPIO_Init(GPIOB,&GPIO_InitStructure);
   
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
     
    SPI_Cmd(SPI2,ENABLE);
#endif
}
 
/*---------------------------------------------------------------------------------
* Function Name  : Write_Enable
* Description    : Flash write enable  
* Input          : None
* Output         : None
* Return         : None
* Attention  : None
*---------------------------------------------------------------------------------*/
void Flash_Write_Enable(void)
{
    Flash_CS_Low;
    
    Flash_SendByte(0x06);
    
    Flash_CS_High;
}
 
/*---------------------------------------------------------------------------------
* Function Name  : Flash_WaitWriteEnd()
* Description    : waiting for writing flash end
* Input          : None
* Output         : None
* Return         : None
* Attention  : None
*---------------------------------------------------------------------------------*/
void Flash_WaitWriteEnd(void)
{
    u8 Flash_Status = 0;
    
    Flash_CS_Low;
    
    Flash_Status = Flash_SendByte(0x05);
     
    while((Flash_Status & 0x01) == SET)
    {
        Flash_Status = Flash_SendByte(0xFF); //send the OxFF just for offering the clock to the slave 
    }
 
    Flash_CS_High;
}
/*---------------------------------------------------------------------------------
* Function Name  : Flash_SendByte()
* Description    : send to flash a byte 
* Input          : None
* Output         : None
* Return         : one byte
* Attention  : None
*---------------------------------------------------------------------------------*/
u8 Flash_SendByte(u8 byte)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); //Loop while DR register in not emplty 

    SPI_I2S_SendData(SPI2, byte);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); //wait for receiving data

    return SPI_I2S_ReceiveData(SPI2);
}
 
/*---------------------------------------------------------------------------------
* Function Name  : Flash_EraseSector
* Description    : Erase a sector
* Input          : u32int_t SectorAddress
* Output         : None
* Return         : one byte
* Attention  : None
*---------------------------------------------------------------------------------*/
void Flash_EraseSector(u32 SectorAddress)
{
    Flash_Write_Enable();
    
    Flash_CS_Low;
    
    Flash_SendByte(0x20);
    Flash_SendByte(SectorAddress>>16);
    Flash_SendByte(SectorAddress>>8);
    Flash_SendByte(SectorAddress);

    Flash_CS_High;
     
    Flash_WaitWriteEnd();
}
 
/*---------------------------------------------------------------------------------
* Function Name  : Flash_EraseSector
* Description    : Erase a sector
* Input          : u32int_t SectorAddress
* Output         : None
* Return         : None
* Attention  : None
*---------------------------------------------------------------------------------*/
void Flash_PageWrite(u8 *pBuffer,u32 PageAddress,u16 WriteByteNum)
{
    Flash_Write_Enable();
    
    Flash_CS_Low;
    
    Flash_SendByte(0x02);
    Flash_SendByte(PageAddress >> 16);
    Flash_SendByte(PageAddress >> 8);
    Flash_SendByte(PageAddress);
      
    if(WriteByteNum > 256)  //the biggest num is 256
    {
        WriteByteNum = 256;
    }
     
    while(WriteByteNum--)
    {
        Flash_SendByte(*pBuffer);
        pBuffer++;
    }
     
    Flash_CS_High;

    Flash_WaitWriteEnd();
}
 
 
/*---------------------------------------------------------------------------------
* Function Name  : Flash_EraseBlock
* Description    : Erase a Block
* Input          : u32int_t BlockAddress
* Output         : None
* Return         : None
* Attention  : None
*---------------------------------------------------------------------------------*/
void Flash_EraseBlock(u32 BlockAddress)
{
    Flash_Write_Enable();
    
    Flash_CS_Low;

    Flash_SendByte(0xD8);
    Flash_SendByte(BlockAddress >> 16);
    Flash_SendByte(BlockAddress >> 8);
    Flash_SendByte(BlockAddress);
     
    Flash_CS_High;

    Flash_WaitWriteEnd();
}
 
/*---------------------------------------------------------------------------------
* Function Name  : Flash_ReadBuffer
* Description    : read certain number bytes from the flash
* Input          : u8 *pBuffer ,u32 ReadBuffer,u16 ReadByteNum
* Output         : None
* Return         : None
* Attention  : None
*---------------------------------------------------------------------------------*/
void Flash_Read(u8 *pBuffer,u32 ReadAddress,u16 ReadByteNum)
{
    Flash_CS_Low;
    
    Flash_SendByte(0x03);
    Flash_SendByte(ReadAddress >> 16);
    Flash_SendByte(ReadAddress >> 8);
    Flash_SendByte(ReadAddress);
     
    while(ReadByteNum--)
    {
        *pBuffer = Flash_SendByte(0xFF);
        pBuffer++;
    }
    
    Flash_CS_High;
}
 
//Power-down mode 
void Flash_PowerDown(void)   
{ 
  Flash_CS_Low;
 
  Flash_SendByte(0xb9);
 
  Flash_CS_High;
}   
 
//wake up the flash
void Flash_WakeUp(void)   
{
  Flash_CS_Low;
 
  Flash_SendByte(0xab);
 
  Flash_CS_High;            
}   
 
/*---------------------------------------------------------------------------------
* Function Name  : FlashID_Read
* Description    : read flash ID
* Input          : None
* Output         : u32 IDnumber
* Return         : None
* Attention  : None
*---------------------------------------------------------------------------------*/
u32 Flash_ReadID(void)
{
    u32 ID_Temp,temp_h,temp_m,temp_l;

    Flash_CS_Low;
    Flash_SendByte(0x9F);

    temp_h = Flash_SendByte(0xFF);
    temp_m = Flash_SendByte(0xFF);
    temp_l = Flash_SendByte(0xFF);
    Flash_CS_High;

    ID_Temp = (temp_h << 16)|(temp_m << 8)|temp_l;
    return ID_Temp;
}

