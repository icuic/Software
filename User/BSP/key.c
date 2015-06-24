#include "key.h"
#include "core_cmInstr.h"

/*********�û�һ�㲻���ԸĶ�************************/

#define NTime		58	//����ȥ������ʱ��ֵ	
#define MaxRate		3528	//���������İ���ǰʱ��Ϊ600ms
#define MinRate		588	//�������µ�����


#define KEYVAL1	    0x10
#define KEYVAL2	    0x20
#define KEYVAL3	    0x30



const KeyValueTable MyKey[17] = 
{
    {0xf0,{"1/A"}},
    {0xf4,{"5/E"}},
    {0xf2,{"9/I"}},
    {0xf6,{"Switch"}},
    {0xf8,{"2/B"}},
    {0xfc,{"6/F"}},
    {0xfa,{"0/J"}},
    {0xfe,{"Password"}},
    {0x6f,{"3/C"}},
    {0x2f,{"7/G"}},
    {0x4f,{"*/K"}},
    {0x0f,{"Esc"}},
    {0xef,{"4/D"}},
    {0xaf,{"8/H"}},
    {0xcf,{"#/L"}},
    {0x8f,{"OK"}},
    {0xff,{"No key"}},
} ;


volatile  u8 KeyValue=0xff;	 //����û�а�������
volatile  u8 KeyValueOld=0xff;
volatile  u8 CurrentKeyValue = 0xff;
u8 state=0;
u16	keycnt=0;
u8	task=0;
u8   KeyEsc=0;		//�����ͷű�־


volatile u8     PressPressFlag=0;
volatile u8     ReleasePressFlag=0;

void key(void);

void InitSPIcommon(void)
{
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;
    //init spi1 port 
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    //SCK,MOSI,
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    //MISO
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    SPI_Cmd(SPI2, DISABLE);            
    SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_Mode =SPI_Mode_Master;       
    SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;      
    SPI_InitStructure.SPI_CPOL =SPI_CPOL_High;        //CPOL=1?????
    SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;       //CPHA=1 ?????2?
    SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;        //??NSS
    SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_256;  //2??
    SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;      //????
    SPI_InitStructure.SPI_CRCPolynomial =7;        //CRC7
       
     SPI_Init(SPI2,&SPI_InitStructure);
     SPI_Cmd(SPI2, ENABLE);
}

void InitKey(void)
{
    //
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    //CE,LK
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    InitSPIcommon();
    //register process function
    //BspTim3SetIRQCallBack(key);
}

/*******************************************����ɨ�����*********************************/
//scankeyһ����Ҫ�����û���Ӳ���Ķ��ĺ���
u8 scankey(void)  //ֻ����һ�����������
{
    u8 keyvalue;
    
    //select key
    GPIO_ResetBits(GPIOA,GPIO_Pin_12);
    __NOP();
    __NOP();
    __NOP();
    //lock input
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
    
    //Read value
    while((SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE))==RESET);
    SPI_I2S_SendData(SPI2,0x00);
    while((SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE))==RESET);

    keyvalue = SPI_I2S_ReceiveData(SPI2);

    GPIO_SetBits(GPIOA,GPIO_Pin_12);

	return keyvalue;
    
}

#if 0
void key(void)
{
	if(!PressPressFlag && !ReleasePressFlag)
    {
		scankey();   //ɨ�����
		if(0==keycnt&&!KeyEsc)
        {  //��һ��ɨ�����
			KeyValueOld=KeyValue;
	   	}
	   	if(KeyValueOld==KeyValue&&0xff!=KeyValueOld)
        {  //�м�����
			if(!KeyEsc)
            {
				keycnt++;
				switch(task)
                {
				case 0:         //ȥ���׶�
					if(keycnt==NTime)
                    {   //�����������N
					
						keycnt=0;
						PressPressFlag=1;   //��λ��־λ��֪ͨ�������а�������
						CurrentKeyValue = KeyValue;
						KeyEsc=1; //�ȴ��������ͷ�
				    }
					break;
		    	default:
					break;
				}
			}
			
		}
		else 
        {
			keycnt=0;
			KeyEsc=0;
			task = 0;
		}
	}
}
#endif

/*******************************************����ɨ��������*********************************/
