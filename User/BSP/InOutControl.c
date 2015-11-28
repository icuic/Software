#include "InOutcontrol.h"
#include "core_cmInstr.h"

#define SPI_ID  SPI2


void InOutControlInit(void)
{
    u32 i;
    
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;
    //init spi1 port 
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    //SCK,MOSI
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    //LSCK,RST
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //OUTEN
    GPIO_ResetBits(GPIOB,GPIO_Pin_11);    

    SPI_Cmd(SPI_ID, DISABLE);            
    SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_Mode =SPI_Mode_Master;       
    SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;      
    SPI_InitStructure.SPI_CPOL =SPI_CPOL_High;        //CPOL=1?????
    SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;       //CPHA=1 ?????2?
    SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;        //??NSS
    SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_256;  //2??
    SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;      //????
    SPI_InitStructure.SPI_CRCPolynomial =7;        //CRC7
       
     SPI_Init(SPI_ID,&SPI_InitStructure);
     SPI_Cmd(SPI_ID, ENABLE);

     //rst
     GPIO_ResetBits(GPIOB,GPIO_Pin_10);
     for(i = 0;i<1024;i++)
     {
        __NOP();
     }

     GPIO_SetBits(GPIOB,GPIO_Pin_10);

     GPIO_SetBits(GPIOB,GPIO_Pin_11);
    for(i = 0;i<1024;i++)
    {
        __NOP();
    }
    GPIO_ResetBits(GPIOB,GPIO_Pin_11);
}

u8 SPIByte(u8 byte)

{
 
 //while((SPI2->SR &SPI_I2S_FLAG_TXE)==RESET);
 while((SPI_I2S_GetFlagStatus(SPI_ID,SPI_I2S_FLAG_TXE))==RESET);
  
 //SPI2->DR = byte;
 SPI_I2S_SendData(SPI_ID,byte);
 
 //while((SPI2->SR &SPI_I2S_FLAG_RXNE)==RESET);

while((SPI_I2S_GetFlagStatus(SPI_ID,SPI_I2S_FLAG_RXNE))==RESET);
 //return(SPI2->DR);

 return SPI_I2S_ReceiveData(SPI_ID);
 //SPI_I2S_ClearFlag(SPI2,SPI_I2S_FLAG_RXNE) ;
}

#if 0
void OpenLock(u8 * LockId,u8 Numer)
{
    u32 i;

    for(i = 0;i<Numer;i++)
    {
        SPIByte(LockId[i]);
    }
    //SPIByte(LockId>>8);
    //SPIByte(LockId);
    __NOP();
    GPIO_SetBits(GPIOB,GPIO_Pin_11);
    // for(i = 0;i<1000;i++)
     {
    __NOP();
     }
    
    GPIO_ResetBits(GPIOB,GPIO_Pin_11);
}
#endif

void OpenLock(u8 LockId, u8 Numer)
{
    u8 u8Remainder = LockId % 8;
    u8 u8Quotient = LockId / 8;
    u8 u8loop = Numer / 8;
    u8 i;

    if (LockId == 0xff)
    {
        u8Quotient = 0xff;
    }

    for(i = u8loop; i > 0; i--)
    {
       /* 第33 - 39、 41 - 47 路由于PCB布线问题，需要作以下调整。*/
       /* 32 33 34 35 36 37 38 39 */ 
       /* 32 39 38 37 36 35 34 33 */ //-- 实际被控制的线路

       /* 40 41 42 43 44 45 46 47 */
       /* 40 47 46 45 44 43 42 41 */ //-- 实际被控制的线路
       if (i == u8Quotient+1)
       {
            if (u8Quotient+1 == 5 || u8Quotient+1 == 6)
            {
                if (u8Remainder != 0)
                {
                    u8Remainder = 8 - u8Remainder;
                }
            }
        
            SPIByte(1<<u8Remainder);
       }
       else
       {
            SPIByte(0);
       }
    }

    __NOP();
    GPIO_SetBits(GPIOB,GPIO_Pin_11);
    // for(i = 0;i<1000;i++)
     {
    __NOP();
     }
    
    GPIO_ResetBits(GPIOB,GPIO_Pin_11);
}

