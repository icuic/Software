typedef enum
{
    E_UI_WELCOME = 0,
    //E_UI_ADMIN_PASSWORD,
    //E_UI_USER_ROOM_NUM,
    //E_UI_USER_PASSWORD,
    E_UI_OPEN_SUCCESS,
    E_UI_MAX
}eUIIndex;

typedef struct
{
    eUIIndex number;
    eUIIndex parent;
    void (*menu_draw)(void);
    void (*menu_enter)(void);
    void (*menu_action)(uint8_t triggeredKey);
}UIStruct;

void menu_handle(uint8_t key);

