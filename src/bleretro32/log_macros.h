#ifndef LOG_MACROS_H
#define LOG_MACROS_H

#ifdef BLERETRO_DEBUG
#define BLERETRO_LOGF(MSG, ...) Serial.printf(MSG __VA_OPT__(, ) __VA_ARGS__)
#else
#define BLERETRO_LOGF(MSG, ...)
#endif

#define BLERETRO_MSGF(MSG, ...) Serial.printf(MSG __VA_OPT__(, ) __VA_ARGS__)

#endif
