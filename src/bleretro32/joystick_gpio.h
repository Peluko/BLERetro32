#ifndef JOYSTICK_GPIO_H
#define JOYSTICK_GPIO_H

#include "./bleretro32.h"

void GPIOSetup();

void StatusToGPIO(retro_joystick_status_t &status);

void DoAutofire();

#endif
