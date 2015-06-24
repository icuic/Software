#include "InOutcontrol.h"
#include "core_cmInstr.h"

#define SPI_ID  SPI1


void InOutControlInit(void)
{
    u32 i;
    
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;
    //init spi1 port 
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    //SCK,MOSI,LSCK,RST
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_ResetBits(GPIOA,GPIO_Pin_4);    

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
     GPIO_ResetBits(GPIOA,GPIO_Pin_3);
     for(i = 0;i<1024;i++)
     {
        __NOP();
     }

     GPIO_SetBits(GPIOA,GPIO_Pin_3);

     GPIO_SetBits(GPIOA,GPIO_Pin_4);
	for(i = 0;i<1024;i++)
    {
        __NOP();
    }
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
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


void OpenLock(u16 LockId)
{
    u32 i;

    SPIByte(LockId>>8);
    SPIByte(LockId);
    __NOP();
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
    __NOP();
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

