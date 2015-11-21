#include "key.h"
#include "core_cmInstr.h"
#include "beep.h"

/*********用户一般不可以改动************************/

#define NTime       58  //键盘去抖动的时间值    
#define MaxRate     3528    //连续按键的按下前时间为600ms
#define MinRate     588 //连续按下的速率


#define KEYVAL1     0x10
#define KEYVAL2     0x20
#define KEYVAL3     0x30

#define KEY_NONE    0xff
#define KeyValidTime    3   /* 有效按键必须持续5*10ms */

uint8_t KeyCode;          /* 存放检测到的按键值 */
static uint8_t ucDetectTime = 0;      /* 计时到同一按键按下的时间 */
static bool  KeyPressFlag = FALSE;
static uint8_t ucKeyCodeTemp= KEY_NONE;           /* 上一次检测时，读取到的键值 */




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


volatile  u8 KeyValue=0xff;  //设置没有按键按下
volatile  u8 KeyValueOld=0xff;
volatile  u8 CurrentKeyValue = 0xff;
u8 state=0;
u16 keycnt=0;
u8  task=0;
u8   KeyEsc=0;      //按键释放标志


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

/*******************************************按键扫描程序*********************************/
//scankey一般是要根据用户的硬件改动的函数
u8 scankey(void)  //只处理一个键按下情况
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
        scankey();   //扫描键盘
        if(0==keycnt&&!KeyEsc)
        {  //第一次扫描键盘
            KeyValueOld=KeyValue;
        }
        if(KeyValueOld==KeyValue&&0xff!=KeyValueOld)
        {  //有键按下
            if(!KeyEsc)
            {
                keycnt++;
                switch(task)
                {
                case 0:         //去抖阶段
                    if(keycnt==NTime)
                    {   //年龄等于下限N
                    
                        keycnt=0;
                        PressPressFlag=1;   //置位标志位，通知主程序有按键按下
                        CurrentKeyValue = KeyValue;
                        KeyEsc=1; //等待按键的释放
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
    
    ucKey = scankey();             //读取I/O口的按键值
    
    if (ucKey !=  KEY_NONE)           //有按键按下
    {
      if (KeyPressFlag == FALSE)    //判断是否是第一次按键按下
      {
        KeyPressFlag = TRUE;      //按键按下标志位置位
        ucKeyCodeTemp = ucKey;    //更新上一次检测到的按键值
        ucDetectTime = 0;         //按键按下时间计数清零
      }
      else
      {
        if (ucKeyCodeTemp == ucKey)   //当前按下的按键和上一次按下的是同一按键
        {
          if (ucDetectTime < 200)
            ucDetectTime ++;
        }
        else     //本次按下的按键和上次不是同一个按键
        {
          ucKeyCodeTemp = ucKey;    
          ucDetectTime = 0;
        }
      } 
    }
    else   //没有按键按下，或短按释放
    {
        if (KeyPressFlag == TRUE)   
        {
            if (ucDetectTime >= KeyValidTime)
            {
                Beep(100);
                
                KeyCode = ucKeyCodeTemp;
                ClearKeyFlag();   //这句不能删
                return KeyCode;
            }
            ClearKeyFlag();
        }
    }

    KeyCode = KEY_NONE;
    return KeyCode;
}

/*******************************************按键扫描程序完毕*********************************/
