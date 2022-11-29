#ifndef XBOX_H
#define XBOX_H

#include "./bleretro32/bleretro32.h"

bool xbox_to_retro(uint8_t *bt_data, size_t bt_data_length, retro_joystick_status_t *retro_status);

#endif