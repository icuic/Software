#include "roomInfoManage.h"
#include "eeprom.h"
#include "stm32f10x.h"
#include "string.h"

/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NumbOfVar];



stRoomInfo roomInfo[M_MAX_BOX];                             // 24 * 8 = 192 bytes
uint8_t adminCardID[M_CARD_ID_MAX_LENGTH];                  // 6 bytes

uint16_t getVirtAdd(eRoomInfoType type, uint8_t index, uint8_t cardIndex)
{    
    if (index >= M_MAX_BOX || cardIndex >= M_ROOM_MAX_USER)
        return 0xFFFF;
        
    switch(type)
    {
        case E_ROOM_INFO_NUMBER:
            return index * sizeof(stRoomInfo) / 2;

        case E_ROOM_INFO_PASSWORD:
            return index * sizeof(stRoomInfo) / 2 + M_ROOM_NUM_MAX_LENGTH * sizeof(uint8_t) / 2;

        case E_ROOM_INFO_CARD_ID:
            return index * sizeof(stRoomInfo) / 2 + M_ROOM_NUM_MAX_LENGTH * sizeof(uint8_t) / 2 + \
                    + M_ROOM_PASSWORD_MAX_LENGTH * sizeof(uint8_t) / 2 + \
                    cardIndex * M_CARD_ID_MAX_LENGTH * sizeof(uint8_t) / 2;
        default:
            return 0xFFFF;
    }
}

// Write one piece room information to eeprom
void writeRoomInfoToEEP(stRoomInfo *roomData,  uint8_t index)
{
    uint8_t i = 0;
    uint16_t *pdata = (uint16_t *)roomData;    
    uint16_t roomNumAddr = getVirtAdd(E_ROOM_INFO_NUMBER, index, 0);

    for(i = 0; i < sizeof(stRoomInfo) / 2; i++) 
        EE_WriteVariable(roomNumAddr + i, *(pdata + i));
}

// Read one piece room information from eeprom
void readRoomInfoFromEEP(stRoomInfo *roomData,  uint8_t index)
{
    uint8_t i = 0;
    uint16_t *pdata = (uint16_t *)roomData;
    uint16_t roomNumAddr = getVirtAdd(E_ROOM_INFO_NUMBER, index, 0);

    for(i = 0; i < sizeof(stRoomInfo) / 2; i++) 
        EE_ReadVariable(roomNumAddr + i, pdata + i);
}

uint8_t writeFragToEEP(eRoomInfoType type, uint8_t index, uint8_t cardIndex, uint8_t* data)
{
        
    uint8_t i = 0;    
    uint16_t *pdata = (uint16_t *)data;
    uint16_t virtAddr = getVirtAdd(type, index, cardIndex);

    if (index >= M_MAX_BOX || cardIndex >= M_ROOM_MAX_USER)
        return 0xFF;


    switch(type)
    {
        case E_ROOM_INFO_NUMBER:
        {
            for(i = 0; i < M_ROOM_NUM_MAX_LENGTH / 2; i++)
                EE_WriteVariable(virtAddr + i, *(pdata + i));
                
            break;
        }
         
        case E_ROOM_INFO_PASSWORD:
        {
            for(i = 0; i < M_ROOM_PASSWORD_MAX_LENGTH / 2; i++)
                EE_WriteVariable(virtAddr + i, *(pdata + i));

            break;
        }

        case E_ROOM_INFO_CARD_ID:
        {
            for(i = 0; i < M_CARD_ID_MAX_LENGTH / 2; i++)
                EE_WriteVariable(virtAddr + i, *(pdata + i));
        
            break;
        }

        
        default:
            // to do
            break;
    }

    return 0;
}

// number: pointer to the array stored the ASCII of room number
uint8_t SetRoomNum(uint8_t index, uint8_t* number)
{
    if (index < M_MAX_BOX)
    {
        // update RAM
        memcpy(roomInfo[index].number, number, M_ROOM_NUM_MAX_LENGTH);
        // update EEPROM
        writeFragToEEP(E_ROOM_INFO_NUMBER, index, 0, roomInfo[index].number);
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
    // update EEPROM
    writeFragToEEP(E_ROOM_INFO_PASSWORD, index, 0, roomInfo[index].password);

    return 0;
}

uint8_t SetRoomCardID(uint8_t index, uint8_t* data)
{
    uint8_t i = 0, j = 0, len = 0;

    while( *(data + len) != 0xFF)
    {
        len ++;
    }

    /* if existed */
    for (i = 0; i < M_ROOM_MAX_USER; i++)
    {
        if (memcmp(roomInfo[index].cardID[i], data, len) == 0)
        {
            return ;
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
        // update EEPROM
        writeFragToEEP(E_ROOM_INFO_CARD_ID, index, i, roomInfo[index].cardID[i]);
    }
    else                        /* FIFO */
    {
        for (j = 0; j < M_ROOM_MAX_USER - 1; j++)
        {
            // update RAM
            memcpy(roomInfo[index].cardID[j], roomInfo[index].cardID[j+1], M_CARD_ID_MAX_LENGTH);
            // update EEPROM
            writeFragToEEP(E_ROOM_INFO_CARD_ID, index, j, roomInfo[index].cardID[j]);
        }

        // update RAM
        memcpy(roomInfo[index].cardID[M_ROOM_MAX_USER], data, M_CARD_ID_MAX_LENGTH);
        // update EEPROM
        writeFragToEEP(E_ROOM_INFO_CARD_ID, index, M_ROOM_MAX_USER, roomInfo[index].cardID[M_ROOM_MAX_USER]);
    }


    return 0;
}

void recoverRoomInfoFromEEPROM(void)
{
    uint8_t i = 0;

    for (i = 0; i < M_MAX_BOX; i++)
    {
        readRoomInfoFromEEP(&roomInfo[i],  i);
    }
}

void writeAdminCardIDToEEPROM(uint8_t* cardID)
{
    uint8_t i = 0;
    uint16_t *pdata = (uint16_t *)cardID;    
    uint16_t adminCardIDAddr = sizeof(roomInfo) / 2;

    for(i = 0; i < sizeof(adminCardID) / 2; i++) 
        EE_WriteVariable(adminCardIDAddr + i, *(pdata + i));
}


void recoverAdminCardIDFromEEPROM(void)
{
    uint8_t i = 0;
    uint16_t *pdata = (uint16_t *)adminCardID;
    uint16_t adminCardIDAddr = sizeof(roomInfo) / 2;

    for(i = 0; i < sizeof(adminCardID) / 2; i++) 
        EE_ReadVariable(adminCardIDAddr + i, pdata + i);

}

void initVirtAddVarTab(void)
{
    uint8_t i = 0, j = 0;
    
    // virtual address for roomInfo
    for (i = 0; i < sizeof(roomInfo) / 2; i++)
    {
        VirtAddVarTab[i] = i;
    }

    // virtual address for adminCardID
    j = sizeof(roomInfo) / 2;
    for (i = 0; i < sizeof(adminCardID) / 2; i++)
    {
        VirtAddVarTab[j + i] = j + i;
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
    uint8_t i = 0, j = 0, len = 0;

    while( *(uid + len) != 0xFF)
    {
        len ++;
    }

    for(i = 0; i < M_MAX_BOX; i++)
    {
        for(j = 0; j < M_ROOM_MAX_USER; j ++)
        {
            if (memcmp(roomInfo[i].cardID[j], uid, len) == 0)
            {
                if (len < M_CARD_ID_MAX_LENGTH)
                {
                    if (roomInfo[i].cardID[len] == 0)
                    {
                        return i;
                    }
                }
                else
                {
                    return i;
                }
            }
        }
    }

    return i;
}
