# BLERetro32

## ESP32 Bluetooth LE HID host for gamepad.

This could be used as a generic joystick host, but its main use is to serve as
an adapter for retro computers.
Current implementation emulates a two buttons two axis joystick, like the one
used on the MSX or the Commodore Amiga (take a look on
[joystick_gpio.cpp](src/bleretro32/joystick_gpio.cpp) for the pinout).
It also provides adjustable rate autofire, making use of extra buttons on modern
gamepads.

There are other similar projects (for example [Bluepad32](https://gitlab.com/ricardoquesada/bluepad32))
but all that I've found only works with Bluetooth Classic, not Bluetooth LE. My current favorite
gamepads are Bluetooth LE, so I've made this little project to support them. By now it's configured
to work with the gamepads I use the most: the newer versions of the
Microsoft XBox Controller (the ones with Bluetooth LE) [^1] and
my own [ESP32 based arcade controller](https://github.com/Peluko/esp32-arcade). It should be easy
to add support for other gamepads, just look at the use of ```pad_list[]``` on [main.cpp](src/main.cpp).

[^1]: I still don't know why, but Xbox gamepad sometimes doesn't link. Sometimes you have
to repeat the process, restarting the gamepad and resetting the ESP32.


