#ifndef _ROOM_INFO_MANAGE_H_
#define _ROOM_INFO_MANAGE_H_

#include "stm32f10x.h"



#define M_ROOM_NUM_MAX_LENGTH       6
#define M_ROOM_PASSWORD_MAX_LENGTH  6
#define M_ROOM_MAX_USER             6
#define M_CARD_ID_MAX_LENGTH        4

#define M_MAX_BOX                   48

#define M_ADMIN_PASSWORD_LENGTH     4

typedef enum
{
    E_ROOM_INFO_NUMBER = 0,
    E_ROOM_INFO_PASSWORD,
    E_ROOM_INFO_CARD_ID,
    E_ROOM_INFO_END
}eRoomInfoType;


#pragma pack(1)
typedef struct
{
    uint8_t number[M_ROOM_NUM_MAX_LENGTH];                  // 6 bytes
    uint8_t password[M_ROOM_PASSWORD_MAX_LENGTH];           // 6 bytes
    uint8_t cardID[M_ROOM_MAX_USER][M_CARD_ID_MAX_LENGTH];  // 24 bytes
    uint16_t crc;                                           // 2 bytes
}stRoomInfo;                                             //  38 bytes
#pragma pack()

extern uint8_t adminCardID[M_CARD_ID_MAX_LENGTH];
extern stRoomInfo roomInfo[M_MAX_BOX];
extern long long bitMapFlashDateError;
extern uint8_t adminPassword[M_ADMIN_PASSWORD_LENGTH];

uint8_t SetRoomNum(uint8_t index, uint8_t* number);
uint8_t SetRoomPassword(uint8_t index, uint8_t* data);
uint8_t SetRoomCardID(uint8_t index, uint8_t* data);

uint8_t matchRoomNum(uint8_t *num, uint8_t len);
bool matchRoomPassword(uint8_t index, uint8_t *pw, uint8_t lenPW);

void clearCard(uint8_t roomIndex);


void saveRoomInfo(uint8_t index);
void recoverRoomInfoFromFlash(void);
void recoverAdminCardIDFromFlash(void);

uint8_t matchCardID(uint8_t* uid);
void writeAdminCardIDToFlash(uint8_t* cardID);

void writeAdminPWToFlash(uint8_t* pw);
void GetAdminPWFromFlash(void);

#endif
