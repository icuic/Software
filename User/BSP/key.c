#include "key.h"
#include "core_cmInstr.h"
#include "beep.h"

/*********�û�һ�㲻���ԸĶ�************************/

#define NTime       58  //����ȥ������ʱ��ֵ    
#define MaxRate     3528    //���������İ���ǰʱ��Ϊ600ms
#define MinRate     588 //�������µ�����


#define KEYVAL1     0x10
#define KEYVAL2     0x20
#define KEYVAL3     0x30

#define KEY_NONE    0xff
#define KeyValidTime    3   /* ��Ч�����������5*10ms */

uint8_t KeyCode;          /* ��ż�⵽�İ���ֵ */
static uint8_t ucDetectTime = 0;      /* ��ʱ��ͬһ�������µ�ʱ�� */
static bool  KeyPressFlag = FALSE;
static uint8_t ucKeyCodeTemp= KEY_NONE;           /* ��һ�μ��ʱ����ȡ���ļ�ֵ */




const KeyValueTable MyKey[17] = 
{
#if 0
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
#endif

    {0xf0,{"1"}, '1', 'A'},
    {0xf4,{"5"}, '5', 'E'},
    {0xf2,{"9"}, '9', 'I'},
    {0xf6,{"Switch"}, 0, 0},
    {0xf8,{"2"}, '2', 'B'},
    {0xfc,{"6"}, '6', 'F'},
    {0xfa,{"0"}, '0', 'J'},
    {0xfe,{"Password"}, 0, 0},
    {0x6f,{"3"}, '3', 'C'},
    {0x2f,{"7"}, '7', 'G'},
    {0x4f,{"*/K"}, '*', 'K'},
    {0x0f,{"Esc"}, 0, 0},
    {0xef,{"4"}, '4', 'D'},
    {0xaf,{"8"}, '8', 'H'},
    {0xcf,{"#/L"}, '#', 'L'},
    {0x8f,{"OK"}, '!', 0},
    {0xff,{"No key"}, 0, 0},

} ;


volatile  u8 KeyValue=0xff;  //����û�а�������
volatile  u8 KeyValueOld=0xff;
volatile  u8 CurrentKeyValue = 0xff;
u8 state=0;
u16 keycnt=0;
u8  task=0;
u8   KeyEsc=0;      //�����ͷű�־


volatile u8     PressPressFlag=0;
volatile u8     ReleasePressFlag=0;

void key(void);

void InitSPIcommon(void)
{
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;
    //init spi1 port 
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    //SCK,MOSI,
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    //MISO
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    SPI_Cmd(SPI1, DISABLE);            
    SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_Mode =SPI_Mode_Master;       
    SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;      
    SPI_InitStructure.SPI_CPOL =SPI_CPOL_High;        //CPOL=1?????
    SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;       //CPHA=1 ?????2?
    SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;        //??NSS
    SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_256;  //2??
    SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;      //????
    SPI_InitStructure.SPI_CRCPolynomial =7;        //CRC7
       
     SPI_Init(SPI1,&SPI_InitStructure);
     SPI_Cmd(SPI1, ENABLE);
}
uint8_t key_detect(void);
void InitKey(void)
{
    //
    //init io
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    //CE,LK
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    InitSPIcommon();
    //register process function
    
    //BspTim3SetIRQCallBack(key_detect);
}

/*******************************************����ɨ�����*********************************/
//scankeyһ����Ҫ�����û���Ӳ���Ķ��ĺ���
u8 scankey(void)  //ֻ����һ�����������
{
    u8 keyvalue;
    
    //select key
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
    __NOP();
    __NOP();
    __NOP();
    //lock input
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    
    //Read value
    while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))==RESET);
    SPI_I2S_SendData(SPI1,0x00);
    while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))==RESET);

    keyvalue = SPI_I2S_ReceiveData(SPI1);

    GPIO_SetBits(GPIOB,GPIO_Pin_1);

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


static void ClearKeyFlag(void)
{
    ucKeyCodeTemp = KEY_NONE;
    KeyPressFlag = FALSE;
    ucDetectTime = 0;    
}

extern u8 flagSPI2;

uint8_t key_detect(void)
{
    uint8_t ucKey;  
    
    ucKey = scankey();             //��ȡI/O�ڵİ���ֵ
    
    if (ucKey !=  KEY_NONE)           //�а�������
    {
      if (KeyPressFlag == FALSE)    //�ж��Ƿ��ǵ�һ�ΰ�������
      {
        KeyPressFlag = TRUE;      //�������±�־λ��λ
        ucKeyCodeTemp = ucKey;    //������һ�μ�⵽�İ���ֵ
        ucDetectTime = 0;         //��������ʱ���������
      }
      else
      {
        if (ucKeyCodeTemp == ucKey)   //��ǰ���µİ�������һ�ΰ��µ���ͬһ����
        {
          if (ucDetectTime < 200)
            ucDetectTime ++;
        }
        else     //���ΰ��µİ������ϴβ���ͬһ������
        {
          ucKeyCodeTemp = ucKey;    
          ucDetectTime = 0;
        }
      } 
    }
    else   //û�а������£���̰��ͷ�
    {
        if (KeyPressFlag == TRUE)   
        {
            if (ucDetectTime >= KeyValidTime)
            {
                Beep(100);
                
                KeyCode = ucKeyCodeTemp;
                ClearKeyFlag();   //��䲻��ɾ
                return KeyCode;
            }
            ClearKeyFlag();
        }
    }

    KeyCode = KEY_NONE;
    return KeyCode;
}

/*******************************************����ɨ��������*********************************/
