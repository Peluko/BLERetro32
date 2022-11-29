#ifndef BLERETRO32_H
#define BLERETRO32_H

#include <stdlib.h>

enum class CnnStatus : uint8_t
{
    Idle,
    Scanning,
    DeviceFound,
    Connecting,
    Connected
};

struct retro_joystick_status_t {
    bool btnA;
    bool btnA_alt;

    bool btnB;
    bool btnB_alt;

    bool up;
    bool left;
    bool down;
    bool right;

    bool auto_inc;
    bool auto_dec;
};

struct pad_definition_t
{
    const char *name;
    bool (*bt_process_fn)(uint8_t *bt_data, size_t bt_data_length, retro_joystick_status_t *retro_status);
};

void BLERetro32_Setup(pad_definition_t *pad_list, size_t count);
CnnStatus BLERetro32_Loop();

#endif
