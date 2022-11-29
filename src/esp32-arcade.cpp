#include <stdlib.h>
#include <esp32-arcade.h>

struct esp32_arcade_data_t {
    bool btn1 : 1; // byte 0
    bool btn2 : 1;
    bool btn3 : 1;
    bool btn4 : 1;
    bool btn5 : 1;
    bool btn6 : 1;
    bool btn7 : 1;
    bool btn8 : 1;

    bool btn9 : 1; // byte 1
    bool btn10 : 1;
    bool btn11 : 1;
    bool btn12 : 1;
    bool btn13 : 1;
    bool btn14 : 1;
    bool btn15 : 1;
    bool btn16 : 1;

    int8_t x; // 2. X axis
    int8_t y; // 3. Y axis
    int8_t z; // 4. unused on esp32-arcade
    int8_t rZ; // 5. unused on esp32-arcade
    int8_t rX; // 6. unused on esp32-arcade
    int8_t rY; // 7. unused on esp32-arcade

    int8_t hat; // 8. unused on esp32-arcade
};

#define AXIS_TOLERANCE 50

bool esp32_arcade_to_retro(uint8_t *bt_data, size_t bt_data_length, retro_joystick_status_t *retro_status)
{
    if (bt_data_length == sizeof(esp32_arcade_data_t))
    {
        auto esp32 = (esp32_arcade_data_t *)bt_data;
        retro_status->btnA = esp32->btn1;
        retro_status->btnA_alt = esp32->btn3;
        retro_status->btnB = esp32->btn2;
        retro_status->btnB_alt = esp32->btn4;

        retro_status->up = esp32->y < -AXIS_TOLERANCE;
        retro_status->right = esp32->x > AXIS_TOLERANCE;
        retro_status->down = esp32->y > AXIS_TOLERANCE;
        retro_status->left = esp32->x < -AXIS_TOLERANCE;

        retro_status->auto_inc = esp32->btn6;
        retro_status->auto_dec = esp32->btn5;

        return true;
    }
    else
    {
        return false;
    }
}
