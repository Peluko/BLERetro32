#include <Arduino.h>

#include "./joystick_gpio.h"
#include "./log_macros.h"

#define PIN_UP 1
#define PIN_DOWN 2
#define PIN_LEFT 3
#define PIN_RIGHT 4
#define PIN_TRG1 5
#define PIN_TRG2 6

#define STATUS_LED_FAST_TIME 300
#define STATUS_LED_SLOW_TIME 1500

void GPIOSetup()
{
    pinMode(PIN_UP, OUTPUT);
    pinMode(PIN_DOWN, OUTPUT);
    pinMode(PIN_LEFT, OUTPUT);
    pinMode(PIN_RIGHT, OUTPUT);
    pinMode(PIN_TRG1, OUTPUT);
    pinMode(PIN_TRG2, OUTPUT);

    digitalWrite(PIN_UP, 1);
    digitalWrite(PIN_DOWN, 1);
    digitalWrite(PIN_LEFT, 1);
    digitalWrite(PIN_RIGHT, 1);
    digitalWrite(PIN_TRG1, 1);
    digitalWrite(PIN_TRG2, 1);
}

#define AUTOFIRE_PCT_CHANGE 5 // (AUTOFIRE_INITIAL_RATE * AUTOFIRE_PCT_CHANGE) / 100 must be >= 1
#define AUTOFIRE_INITIAL_RATE 20
#define AUTOFIRE_MIN_RATE 10
#define AUTOFIRE_MAX_RATE 2000

int32_t autofire_rate = AUTOFIRE_INITIAL_RATE;

bool autofire_A = false;
bool autofire_B = false;

void StatusToGPIO(retro_joystick_status_t &status)
{
    digitalWrite(PIN_UP, !status.up);
    digitalWrite(PIN_RIGHT, !status.right);
    digitalWrite(PIN_DOWN, !status.down);
    digitalWrite(PIN_LEFT, !status.left);

    autofire_A = status.btnA_alt;
    autofire_B = status.btnB_alt;

    if(status.btnA) {
        digitalWrite(PIN_TRG1, 0);
    } else if(!autofire_A) {
        digitalWrite(PIN_TRG1, 1);
    } // else managed by autofire

    if(status.btnB) {
        digitalWrite(PIN_TRG2, 0);
    } else if(!autofire_B) {
        digitalWrite(PIN_TRG2, 1);
    } // else managed by autofire

    if (status.auto_inc)
    {
        auto inc = autofire_rate * AUTOFIRE_PCT_CHANGE / 100;
        inc = inc > 0 ? inc : 1;
        autofire_rate += inc;
        if (autofire_rate > AUTOFIRE_MAX_RATE)
        {
            autofire_rate = AUTOFIRE_MAX_RATE;
        }
    }
    if (status.auto_dec)
    {
        auto dec = autofire_rate * AUTOFIRE_PCT_CHANGE / 100;
        dec = dec > 0 ? dec : 1;
        autofire_rate -= dec;
        if (autofire_rate < AUTOFIRE_MIN_RATE)
        {
            autofire_rate = AUTOFIRE_MIN_RATE;
        }
    }
}

void DoAutofire()
{
    static uint32_t last_millis = 0;
    static bool auto_fire_status = false;

    auto now = millis();
    auto ellapsed = now - last_millis;
    if (ellapsed > autofire_rate)
    {
        auto_fire_status = !auto_fire_status;
        last_millis = now;
    }

    if (autofire_A)
    {
        digitalWrite(PIN_TRG1, !auto_fire_status);
    }
    if (autofire_B)
    {
        // alternating
        digitalWrite(PIN_TRG2, auto_fire_status);
    }
}
