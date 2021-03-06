#include "roomInfoManage.h"
#ifdef USE_EEPROM
#include "eeprom.h"
#else
#include "w25qxx.h"
#endif
#include "stm32f10x.h"
#include "string.h"

#define M_FLASH_ADDRESS_ADMIN_CARD_ID                (2 * M_MAX_BOX * FLASH_SECTOR_SIZE) // 紧接在箱门信息之后

stRoomInfo roomInfo[M_MAX_BOX];
uint8_t adminCardID[M_CARD_ID_MAX_LENGTH];

uint8_t adminPassword[M_ADMIN_PASSWORD_LENGTH];         // 管理员密码

long long bitMapFlashDateError;


// number: pointer to the array stored the ASCII of room number
uint8_t SetRoomNum(uint8_t index, uint8_t* number)
{
    if (index < M_MAX_BOX)
    {
        // update RAM
        memcpy(roomInfo[index].number, number, M_ROOM_NUM_MAX_LENGTH);

        // update Flash
        saveRoomInfo(index);
    }
    else
    {
        // to do
    }

    return 0;
}

uint8_t SetRoomPassword(uint8_t index, uint8_t* data)
{
    // update RAM
    memcpy(roomInfo[index].password, data, M_ROOM_PASSWORD_MAX_LENGTH);

    // update Flash
    saveRoomInfo(index);
    
    return 0;
}

uint8_t SetRoomCardID(uint8_t index, uint8_t* data)
{
    uint8_t i = 0, j = 0;

    /* if existed */
    for (i = 0; i < M_ROOM_MAX_USER; i++)
    {
        if (memcmp(roomInfo[index].cardID[i], data, M_CARD_ID_MAX_LENGTH) == 0)
        {
            return 0xFF;
        }
    }


    /* if there is empty space to stored */
    for (i = 0; i < M_ROOM_MAX_USER; i++)
    {
        for (j = 0; j < M_CARD_ID_MAX_LENGTH; j++)
        {
            if (roomInfo[index].cardID[i][j] != 0)
                break;;
        }

        if (j == M_CARD_ID_MAX_LENGTH)
            break;
    }


    if (i < M_ROOM_MAX_USER)    /* Store to RAM and EEPROM */
    {
        // update RAM
        memcpy(roomInfo[index].cardID[i], data, M_CARD_ID_MAX_LENGTH);

        // update Flash
        saveRoomInfo(index);
    }
    else                        /* FIFO */
    {
        for (j = 0; j < M_ROOM_MAX_USER - 1; j++)
        {
            // update RAM
            memcpy(roomInfo[index].cardID[j], roomInfo[index].cardID[j+1], M_CARD_ID_MAX_LENGTH);

            // update Flash
            saveRoomInfo(index);
        }

        // update RAM
        memcpy(roomInfo[index].cardID[M_ROOM_MAX_USER - 1], data, M_CARD_ID_MAX_LENGTH);

        // update Flash
        saveRoomInfo(index);
    }


    return 0;
}


void writeAdminCardIDToFlash(uint8_t* cardID)
{
    Flash_EraseSector(M_FLASH_ADDRESS_ADMIN_CARD_ID);

    Flash_PageWrite(adminCardID, M_FLASH_ADDRESS_ADMIN_CARD_ID, sizeof(adminCardID));
}


void recoverAdminCardIDFromFlash(void)
{
    Flash_Read(adminCardID, M_FLASH_ADDRESS_ADMIN_CARD_ID, sizeof(adminCardID));
}

void writeAdminPWToFlash(uint8_t* pw)
{
    memcpy(adminPassword, pw, M_ADMIN_PASSWORD_LENGTH);
    
    Flash_EraseSector(M_FLASH_ADDRESS_ADMIN_CARD_ID + FLASH_SECTOR_SIZE);

    Flash_PageWrite(pw, M_FLASH_ADDRESS_ADMIN_CARD_ID + FLASH_SECTOR_SIZE, M_ADMIN_PASSWORD_LENGTH);
}

void GetAdminPWFromFlash(void)
{
    uint8_t tmp[M_ADMIN_PASSWORD_LENGTH] ="7091";
    
    Flash_Read(adminPassword, M_FLASH_ADDRESS_ADMIN_CARD_ID + FLASH_SECTOR_SIZE, sizeof(adminPassword));

    if (adminPassword[0] == 0xFF &&
        adminPassword[1] == 0xFF &&
        adminPassword[2] == 0xFF &&
        adminPassword[3] == 0xFF)
    {
        memcpy(adminPassword, tmp, M_ADMIN_PASSWORD_LENGTH);
    }
}

uint8_t matchRoomNum(uint8_t *num, uint8_t len)
{
    uint8_t i = 0;

    for(i = 0; i < M_MAX_BOX; i++)
    {
        if ((memcmp(roomInfo[i].number, num, len) == 0) &&
             roomInfo[i].number[len] == '\0')
        {
            break;
        }
    }

    return i;
}

bool matchRoomPassword(uint8_t index, uint8_t *pw, uint8_t lenPW)
{
    if ((memcmp(roomInfo[index].password, pw, lenPW) == 0) &&
         roomInfo[index].password[lenPW] == '\0')
    {
        return TRUE;
    }

    return FALSE;
}

uint8_t matchCardID(uint8_t* uid)
{
    uint8_t i = 0, j = 0;
    

    for(i = 0; i < M_MAX_BOX; i++)
    {
        for(j = 0; j < M_ROOM_MAX_USER; j ++)
        {
            if (memcmp(roomInfo[i].cardID[j], uid, M_CARD_ID_MAX_LENGTH) == 0)
            {
                return i;
            }
            else
            {
                continue;
            }
        }
    }

    return i;
}

void clearCard(uint8_t roomIndex)
{
    uint8_t i = 0, j = 0;

    if (roomIndex < M_MAX_BOX)
    {
        for (i = 0; i < M_ROOM_MAX_USER; i++)
        {
            for(j = 0; j < M_CARD_ID_MAX_LENGTH; j++)
            {
                if (roomInfo[roomIndex].cardID[j] != 0)
                {
                    break;
                }
            }
        
            if (j != M_CARD_ID_MAX_LENGTH)
            {
                // update RAM
                memset(roomInfo[roomIndex].cardID[i], 0, M_CARD_ID_MAX_LENGTH);

                // update Flash
                saveRoomInfo(roomIndex);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
sector 0 -- roomInfo[0]
sector 1 -- roomInfo[0] backup

sector 2 -- roomInfo[1]
sector 3 -- roomInfo[1] backup

sector 4 -- roomInfo[2]
sector 5 -- roomInfo[2] backup

...

sector (2 * (M_MAX_BOX - 1)) -- roomInfo[M_MAX_BOX - 1]
sector (2 * (M_MAX_BOX - 1) + 1) -- roomInfo[M_MAX_BOX - 1] backup

sector (2 * M_MAX_BOX) -- adminCardID
sector (2 * M_MAX_BOX + 1) -- adminCardID

*/

// Cyclical Redundancy Check code high byte
const uint8_t HiCRC_Table[]=
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

// Cyclical Redundancy Check code low byte
const uint8_t LoCRC_Table[]=
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2,
    0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
    0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE,
    0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,
    0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62,
    0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE,
    0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,
    0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A,
    0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
    0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


/*******************************************************************************
* Function Name  : UART_CRC16
*
* Description    : 2bytes CRC value calculate
*
* Arguments      : None
*
* Returns        : None
*******************************************************************************/
uint16_t CRC16(uint8_t *px, uint8_t ucLen)
{
     uint8_t ax,cx,ex;
     ax=0xFF;
     ex=0xFF;
     while(ucLen)
     {
         cx=ax^*px++;
         ax=ex^HiCRC_Table[cx];
         ex=LoCRC_Table[cx];
         ucLen--;
     }
     return (ax*0x100+ex);
}


static uint32_t getFlashAddress(uint8_t index)
{
    if (index > M_MAX_BOX)
        return;
    
    return (2 * index * FLASH_SECTOR_SIZE);
}

void saveRoomInfo(uint8_t index)
{
    uint32_t addr = getFlashAddress(index);
    roomInfo[index].crc = CRC16((uint8_t *)&roomInfo[index], sizeof(stRoomInfo) - sizeof(uint16_t));

    Flash_EraseSector(addr);
    Flash_EraseSector(addr + FLASH_SECTOR_SIZE);

    Flash_PageWrite((uint8_t *)&roomInfo[index], addr, sizeof(stRoomInfo));    
    Flash_PageWrite((uint8_t *)&roomInfo[index], addr + FLASH_SECTOR_SIZE, sizeof(stRoomInfo));

    bitMapFlashDateError &= ~(1 << index);
}

static void getOnePieceOfRoomInfo(uint8_t index, uint8_t* buff)
{
    uint32_t addr = getFlashAddress(index);

    uint16_t tmpCRC[2] = {0, 0};
    stRoomInfo tmpInfo[2];
    memset((uint8_t *)&tmpInfo[0], 0, sizeof(stRoomInfo));
    memset((uint8_t *)&tmpInfo[1], 0, sizeof(stRoomInfo));


    Flash_Read((uint8_t *)&tmpInfo[0], addr, sizeof(stRoomInfo));
    Flash_Read((uint8_t *)&tmpInfo[1], addr + FLASH_SECTOR_SIZE, sizeof(stRoomInfo));


    tmpCRC[0] = CRC16((uint8_t *)&tmpInfo[0], sizeof(stRoomInfo) - sizeof(uint16_t));
    tmpCRC[1] = CRC16((uint8_t *)&tmpInfo[1], sizeof(stRoomInfo) - sizeof(uint16_t));


    if ((tmpCRC[0] == tmpInfo[0].crc) && (tmpCRC[1]== tmpInfo[1].crc))
    {
        memcpy(buff, (uint8_t *)&tmpInfo[0], sizeof(stRoomInfo));
    }
    else if(tmpCRC[0] == tmpInfo[0].crc)
    {
        memcpy(buff, (uint8_t *)&tmpInfo[0], sizeof(stRoomInfo));
    }
    else if(tmpCRC[1]== tmpInfo[1].crc)
    {
        memcpy(buff, (uint8_t *)&tmpInfo[1], sizeof(stRoomInfo));
    }
    else
    {
        bitMapFlashDateError |= (1 << index);
    }
}

void recoverRoomInfoFromFlash(void)
{
    uint8_t i = 0;

    for(i = 0; i < M_MAX_BOX; i++)
    {
        getOnePieceOfRoomInfo(i, (uint8_t*)&roomInfo[i]);
    }

}
