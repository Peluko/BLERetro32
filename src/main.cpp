#include <Arduino.h>
#include <NimBLEDevice.h>

#include "./bleretro32/bleretro32.h"
#include "./xbox.h"
#include "./esp32-arcade.h"


pad_definition_t pad_list[] = {
    {"Xbox Wireless Controller", xbox_to_retro},
    {"ESP32 Arcade", esp32_arcade_to_retro}};

#define PIN_STATUS_LED 7 // 2 is board led

#define STATUS_LED_FAST_TIME 200
#define STATUS_LED_SLOW_TIME 1200

bool GetLedStatus(CnnStatus cnn_status)
{
    static long last = 0;

    long on_time;
    long off_time;
    switch (cnn_status)
    {
    case CnnStatus::Scanning:
        on_time = STATUS_LED_FAST_TIME;
        off_time = STATUS_LED_FAST_TIME;
        break;
    case CnnStatus::DeviceFound:
    case CnnStatus::Connecting:
        on_time = STATUS_LED_SLOW_TIME;
        off_time = STATUS_LED_FAST_TIME;
        break;
    case CnnStatus::Connected:
        on_time = STATUS_LED_FAST_TIME;
        off_time = STATUS_LED_SLOW_TIME;
        break;
    default:
        on_time = STATUS_LED_SLOW_TIME;
        off_time = STATUS_LED_FAST_TIME;
        break;
    }

    auto act_time = millis();
    if (act_time < (last + on_time))
    {
        return true;
    }
    else if (act_time < (last + on_time + off_time))
    {
        return false;
    }
    else
    {
        last = act_time;
        return true;
    }
}

#define LOOP_DELAY 5

void setup()
{
    Serial.begin(115200);
    Serial.printf("Starting BLERetro32\n");

    pinMode(PIN_STATUS_LED, OUTPUT);

    BLERetro32_Setup(pad_list, sizeof(pad_list) / sizeof(pad_definition_t));

}

void loop()
{
    auto cnn_status = BLERetro32_Loop();
    digitalWrite(PIN_STATUS_LED, GetLedStatus(cnn_status));

    delay(LOOP_DELAY);
}
