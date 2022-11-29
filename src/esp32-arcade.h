#ifndef ESP32_ARCADE_H
#define ESP32_ARCADE_H

#include "./bleretro32/bleretro32.h"

bool esp32_arcade_to_retro(uint8_t *bt_data, size_t bt_data_length, retro_joystick_status_t *retro_status);


#endif