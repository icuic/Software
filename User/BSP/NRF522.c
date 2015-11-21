#include "NRF522.H"

#define NSS522_1 GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define NSS522_0 GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define RST522_1 GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define RST522_0 GPIO_ResetBits(GPIOB,GPIO_Pin_5)

extern void InitSPIcommon(void);
void InitNRF522Pin(void);

void Delay(unsigned int time)
{
  unsigned int i,k                             ;
  for(i=0;i<255;i++)
    for(k=0;k<time;k++)
      __NOP()                                   ;	  
}


u8 SpiWriteRead(u8 datav)
{
    u8 keyvalue;
    
    //Read value
    while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))==RESET);
    SPI_I2S_SendData(SPI1,datav);
    while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))==RESET);
    keyvalue = SPI_I2S_ReceiveData(SPI1);
	return keyvalue;
}

void InitNRF522Func(void)
{
    InitNRF522Pin();
    PcdReset();//??RC522
    PcdAntennaOn();//?????? 
}
void InitNRF522Pin(void)
{
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);

    //NCS
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    NSS522_1;

    //RST
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    RST522_1;
        
    InitSPIcommon();
}

//******************************************************************/
//?    ?:?RC522???
//????:Address[IN]:?????
//?    ?:????
//******************************************************************/
unsigned char ReadRawRC(unsigned char Address)
{
  unsigned char ucAddr ;
  unsigned char ucResult=0 ;
  NSS522_0 ;
  ucAddr = ((Address<<1)&0x7E)|0x80 ;
  SpiWriteRead(ucAddr);
  ucResult = SpiWriteRead(0);                       
  NSS522_1;
  return ucResult;
}


void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char  ucAddr;

    NSS522_0;
    ucAddr = ((Address<<1)&0x7E) ;
    SpiWriteRead(ucAddr);
    SpiWriteRead(value);
    NSS522_1;
}

//******************************************************************/
//?    ?:?RC522????
//????:reg[IN]:?????
//          mask[IN]:???
//******************************************************************/
void SetBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0            ;
  tmp = ReadRawRC(reg)| mask;
  WriteRawRC(reg,tmp | mask);  // set bit mask
}

//******************************************************************/
//?    ?:?RC522????
//????:reg[IN]:?????
//          mask[IN]:???
//******************************************************************/
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0              ;
  tmp = ReadRawRC(reg)&(~mask);
  WriteRawRC(reg, tmp)        ;  // clear bit mask
} 

//******************************************************************/
//?    ?:??RC522
//?    ?: ????MI_OK
//******************************************************************/
char PcdReset()
{
  RST522_1                             ;
  Delay(1)                             ;
  RST522_0                             ;
  Delay(1)                             ;
  RST522_1                             ;
  Delay(1)                             ;
  WriteRawRC(CommandReg,PCD_RESETPHASE);
  Delay(1)                             ;
  WriteRawRC(ModeReg,0x3D)             ;
  WriteRawRC(TReloadRegL,30)           ;
  WriteRawRC(TReloadRegH,0)            ;
  WriteRawRC(TModeReg,0x8D)            ;
  WriteRawRC(TPrescalerReg,0x3E)       ;   
//  WriteRawRC(TxASKReg,0x40)            ; // FOR DEBUG AND TEST
  return MI_OK                         ; 
}

//******************************************************************/
//??????  
//?????????????????1ms???
//******************************************************************/
void PcdAntennaOn()
{
  unsigned char i;
  WriteRawRC(TxASKReg,0x40)       ;
  Delay(10)                       ;
  i = ReadRawRC(TxControlReg)     ;
  if(!(i&0x03))
    SetBitMask(TxControlReg, 0x03);
  i=ReadRawRC(TxASKReg)       ;
}

//******************************************************************/
//??????  
//?????????????????1ms???
//******************************************************************/
void PcdAntennaTestOn()
{
//*
  RST522_1                         ;
  Delay(15)                        ; // 2010.10.09 ???? FOR DEBUG
//  Delay(5)                        ; // 2010.10.09 ???? FOR DEBUG
  
  WriteRawRC(TxControlReg,0x02)    ;
//*/
/*
  Delay(1)                         ; 
  SetBitMask(TxControlReg, 0x03);// FOR DEBUG 
*/
}


//******************************************************************/
//??????
//******************************************************************/
void PcdAntennaOff()
{
  ClearBitMask(TxControlReg, 0x03);
}

//******************************************************************/
//?    ?:??RC522?ISO14443???
//????:Command[IN]:RC522???
//          pInData[IN]:??RC522????????
//          InLenByte[IN]:?????????
//          pOutData[OUT]:??????????
//          *pOutLenBit[OUT]:????????
//******************************************************************/
char PcdComMF522(unsigned char Command  ,unsigned char *pInData , 
                 unsigned char InLenByte,unsigned char *pOutData, 
                 unsigned int  *pOutLenBit                       )
{
  char status = MI_ERR                          ;
  unsigned char irqEn   = 0x00                  ;
  unsigned char waitFor = 0x00                  ;
  unsigned char lastBits                        ;
  unsigned char n                               ;
  unsigned int  i                               ;
  switch (Command)
  {
    case PCD_AUTHENT:
      irqEn   = 0x12                            ;
      waitFor = 0x10                            ;
      break                                     ;
    case PCD_TRANSCEIVE:
      irqEn   = 0x77                            ;
      waitFor = 0x30                            ;
      break                                     ;
    default:
      break                                     ;
  }
  WriteRawRC(ComIEnReg,irqEn|0x80)              ; //
  ClearBitMask(ComIrqReg,0x80)                  ;
  WriteRawRC(CommandReg,PCD_IDLE)               ;
  SetBitMask(FIFOLevelReg,0x80)                 ; // ??FIFO 
  for(i=0; i<InLenByte; i++)
    WriteRawRC(FIFODataReg,pInData[i])          ; // ????FIFO 
  WriteRawRC(CommandReg, Command)               ; // ?????????
  if(Command == PCD_TRANSCEIVE)
    SetBitMask(BitFramingReg,0x80)              ; // ????     
  i = 6000                                      ; //????????,??M1???????25ms
  do 
  {
    n = ReadRawRC(ComIrqReg)                    ;
    i--                                         ;
  }
  while((i!=0)&&!(n&0x01)&&!(n&waitFor))        ;
  ClearBitMask(BitFramingReg,0x80)              ;
  if(i!=0)
  {
    if(!(ReadRawRC(ErrorReg)&0x1B))
    {
      status = MI_OK                            ;
      if (n&irqEn&0x01)
        status = MI_NOTAGERR                    ;
      if(Command==PCD_TRANSCEIVE)
      {
        n = ReadRawRC(FIFOLevelReg)             ;
        lastBits = ReadRawRC(ControlReg)&0x07   ;
        if(lastBits)
          *pOutLenBit = (n-1)*8 + lastBits      ;
        else
          *pOutLenBit = n*8                     ;
        if(n==0)
          n = 1                                 ;
        if(n>MAXRLEN)
          n = MAXRLEN                           ;
        for (i=0; i<n; i++)
          pOutData[i] = ReadRawRC(FIFODataReg)  ; 
      }
    }
    else
      status = MI_ERR                           ;        
  }
  SetBitMask(ControlReg,0x80)                   ;// stop timer now
  WriteRawRC(CommandReg,PCD_IDLE)               ; 
  return status;
}

//******************************************************************/
//?    ?:??                                                    /
//????: req_code[IN]:????                                   /
//                0x52 = ?????????14443A????           /
//                0x26 = ??????????                       /
//          pTagType[OUT]:??????                             /
//                0x4400 = Mifare_UltraLight                        /
//                0x0400 = Mifare_One(S50)                          /
//                0x0200 = Mifare_One(S70)                          /
//                0x0800 = Mifare_Pro(X)                            /
//                0x4403 = Mifare_DESFire                           /
//?    ?: ????MI_OK                                           /
//******************************************************************/
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
  char status                                        ;  
  unsigned int  unLen                                ;
  unsigned char ucComMF522Buf[MAXRLEN]               ; 

  ClearBitMask(Status2Reg,0x08)                      ;
  WriteRawRC(BitFramingReg,0x07)                     ;
  SetBitMask(TxControlReg,0x03)                      ;
 
  ucComMF522Buf[0] = req_code                        ;

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,
                       1,ucComMF522Buf,&unLen       );
  if ((status == MI_OK) && (unLen == 0x10))
  {    
    *pTagType     = ucComMF522Buf[0]                 ;
    *(pTagType+1) = ucComMF522Buf[1]                 ;
  }
  else
    status = MI_ERR                                  ;
  return status                                      ;
}

//******************************************************************/
//?    ?:???                                                  /
//????: pSnr[OUT]:?????,4??                             /
//?    ?: ????MI_OK                                           /
//******************************************************************/
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:????
//????: pSnr[IN]:?????,4??
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

//******************************************************************/
//?    ?:??????
//????: auth_mode[IN]: ??????
//                 0x60 = ??A??
//                 0x61 = ??B?? 
//          addr[IN]:???
//          pKey[IN]:??
//          pSnr[IN]:?????,4??
//?    ?: ????MI_OK
//******************************************************************/
char PcdAuthState(unsigned char auth_mode,unsigned char addr,
                  unsigned char *pKey,unsigned char *pSnr    )
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

//******************************************************************/
//?    ?:??M1?????
//????: addr[IN]:???
//          pData[OUT]:?????,16??
//?    ?: ????MI_OK
//******************************************************************/
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status                                          ;
    unsigned int  unLen                                  ;
    unsigned char i,ucComMF522Buf[MAXRLEN]               ; 

    ucComMF522Buf[0] = PICC_READ                         ;
    ucComMF522Buf[1] = addr                              ;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2])       ;   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,
                         ucComMF522Buf,&unLen           );
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
            *(pData+i) = ucComMF522Buf[i];   
    }
    else
      status = MI_ERR;       
    return status;
}

   #if 0
//******************************************************************/
//?    ?:??M1?????
//????: addr[IN]:???
//          pData[OUT]:?????,16??
//?    ?: ????MI_OK
//******************************************************************/
char Read_Block(unsigned char Block,unsigned char *Buf)
{
  char result                                             ;
  result = PcdAuthState(0x60,Block,Password_Buffer,UID)   ;
  if(result!=MI_OK)
    return result                                         ;
  result = PcdRead(Block,Buf)                             ;
//  return result; // 2011.01.03
  
  if(result!=MI_OK)     return   result                   ;
  if(Block!=0x00&&des_on)
  {
    Des_Decrypt((char *)Buf    ,KK,(char *)Buf    )       ;
    Des_Decrypt((char *)&Buf[8],KK,(char *)&Buf[8])       ;  
  }
  return SUCCESS                                          ; 
}
  #endif
//******************************************************************/
//?    ?:????M1???
//????: addr[IN]:???
//          pData[IN]:?????,16??
//?    ?: ????MI_OK
//******************************************************************/
char PcdWrite(unsigned char addr,unsigned char *pData)
{
  char status                                             ;
  unsigned int  unLen                                     ;
  unsigned char i,ucComMF522Buf[MAXRLEN]                  ; 
    
  ucComMF522Buf[0] = PICC_WRITE                           ;
  ucComMF522Buf[1] = addr                                 ;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2])          ;
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,
                       ucComMF522Buf,&unLen          )    ;
  if(  ( status != MI_OK)||(unLen != 4)
     ||((ucComMF522Buf[0]&0x0F)!= 0x0A))
    status = MI_ERR                                       ;           
  if (status == MI_OK)
  {
    for (i=0; i<16; i++)
      ucComMF522Buf[i] = *(pData+i)                       ;  
    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16])      ;
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,
                         18,ucComMF522Buf,&unLen     )    ;
    if(  (status != MI_OK)||(unLen != 4 )
       ||((ucComMF522Buf[0]&0x0F)!= 0x0A))
      status = MI_ERR                                     ;   
  }    
  return status                                           ;
}

#if 0
//******************************************************************/
//?    ?:????M1???
//????: addr[IN]:???
//          pData[IN]:?????,16??
//?    ?: ????MI_OK
//******************************************************************/

char Write_Block(unsigned char Block)
{
  char result                                             ;
  if(des_on)
  {
    Des_Encrypt((char *)RF_Buffer    ,KK,
                (char *)RF_Buffer        )                ;// for debug
    Des_Encrypt((char *)&RF_Buffer[8],KK,
                (char *)&RF_Buffer[8]    )                ;// for debug  
  }
  result = PcdAuthState(0x60,Block,Password_Buffer,UID)   ;
  if(result!=MI_OK)
    return result                                         ;  
  result = PcdWrite(Block,RF_Buffer)                      ;
  return result                                           ;  
}

	 #endif
//******************************************************************/
//?    ?:?????
//????: dd_mode[IN]:???
//               0xC0 = ??
//               0xC1 = ??
//          addr[IN]:????
//          pValue[IN]:4???(?)?,????
//?    ?: ????MI_OK
//******************************************************************/
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

//******************************************************************/
//?    ?:????
//????: sourceaddr[IN]:???
//          goaladdr[IN]:????
//?    ?: ????MI_OK
//******************************************************************/
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}


//******************************************************************/
//?    ?:??????????
//?    ?: ????MI_OK
//******************************************************************/
char PcdHalt(void)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    return status;
//    return MI_OK;
}

//******************************************************************/
//?    ?:??????????
//?    ?: ????MI_OK
//******************************************************************/
char MIF_Halt(void)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    return status ;  
//    return MI_OK;
}



//******************************************************************/
//?MF522??CRC16??
//******************************************************************/
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

