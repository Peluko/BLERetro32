#include <Arduino.h>
#include <NimBLEDevice.h>

#include "./log_macros.h"
#include "./bleretro32.h"
#include "./joystick_gpio.h"

#define SCAN_TIME 10

NimBLEScan *scanner;
NimBLEAdvertisedDevice *foundDevice;

pad_definition_t *supported_pads = nullptr;
size_t number_of_supported_pads = 0;
pad_definition_t *found_pad;

CnnStatus cnn_status = CnnStatus::Idle;

class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
    void onResult(NimBLEAdvertisedDevice *advertisedDevice)
    {
        BLERETRO_LOGF("Found device: %s\n", advertisedDevice->toString().c_str());
        auto deviceName = advertisedDevice->getName();
        for (size_t i = 0; i < number_of_supported_pads;  i++)
        {
            if (deviceName == supported_pads[i].name)
            {
                BLERETRO_LOGF("** Found '%s'\n", supported_pads[i].name);
                foundDevice = advertisedDevice;
                found_pad = &supported_pads[i];
                cnn_status = CnnStatus::DeviceFound;
                scanner->stop();
            }
        }
    }
};

void BLERetro32_Setup(pad_definition_t *pad_list, size_t count)
{
    supported_pads = pad_list;
    number_of_supported_pads = count;

    BLERETRO_MSGF("Setting up Bluetooth\n");
    NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);
    NimBLEDevice::setScanDuplicateCacheSize(200);
    NimBLEDevice::init("BLERetro32");
    NimBLEDevice::setOwnAddrType(BLE_OWN_ADDR_PUBLIC);
    NimBLEDevice::setSecurityAuth(true, false, false);

    BLERETRO_LOGF("Setting up scanner\n");
    scanner = NimBLEDevice::getScan();
    scanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    scanner->setActiveScan(true);
    scanner->setInterval(1000);
    scanner->setWindow(900);

    BLERETRO_LOGF("Setting up GPIO\n");
    GPIOSetup();
}

void ScanCompleteCB(NimBLEScanResults results)
{
    BLERETRO_LOGF("** Scan ended");
    if (cnn_status == CnnStatus::Scanning)
    {
        cnn_status = CnnStatus::Idle;
        BLERETRO_LOGF(" ...restarting\n");
    }
    else
    {
        BLERETRO_LOGF("\n");
    }
}

CnnStatus ScanAndConnect()
{
    BLERETRO_MSGF("Starting scanner\n");
    scanner->clearResults();
    scanner->clearDuplicateCache();
    scanner->setDuplicateFilter(true);

    scanner->start(SCAN_TIME, &ScanCompleteCB, false);
    return CnnStatus::Scanning;
}

class ClientCallbacks : public NimBLEClientCallbacks
{
public:
    ClientCallbacks()
    {
    }

    void onConnect(NimBLEClient *pClient)
    {
        BLERETRO_LOGF("** onConnect\n");
    };

    void onDisconnect(NimBLEClient *pClient)
    {
        BLERETRO_LOGF("** onDisconnect\n");
        BLERETRO_LOGF("Disconnected\n");
        cnn_status = CnnStatus::Idle;
    };

    uint32_t onPassKeyRequest()
    {
        BLERETRO_LOGF("** onPassKeyRequest\n");
        return 0;
    };

    bool onConfirmPIN(uint32_t pass_key)
    {
        BLERETRO_LOGF("** onConfirmPIN\n");
        return true;
    };
};

void CharacteristicNofifyCB(NimBLERemoteCharacteristic *characteristic, uint8_t *data, size_t length, bool is_notify)
{
    static retro_joystick_status_t old_status;
    auto status = old_status;
    if (found_pad->bt_process_fn(data, length, &status))
    {
        if (memcmp(&old_status, &status, sizeof(retro_joystick_status_t)))
        {
            BLERETRO_LOGF("        Data %d-> ", millis());
            BLERETRO_LOGF("isNotify: %d", is_notify);
            BLERETRO_LOGF("btnA: %d ", status.btnA);
            BLERETRO_LOGF("btnA_alt: %d ", status.btnA_alt);
            BLERETRO_LOGF("btnB: %d ", status.btnB);
            BLERETRO_LOGF("btnB_alt: %d ", status.btnB_alt);
            BLERETRO_LOGF("Up: %d ", status.up);
            BLERETRO_LOGF("Left: %d ", status.left);
            BLERETRO_LOGF("Right: %d ", status.right);
            BLERETRO_LOGF("Down: %d ", status.down);
            BLERETRO_LOGF("Auto inc: %d ", status.auto_inc);
            BLERETRO_LOGF("Auto dec: %d ", status.auto_dec);
            BLERETRO_LOGF("\n");

            old_status = status;

            StatusToGPIO(status);
        }
    }
}

void ConnectToHID(NimBLEAdvertisedDevice *device)
{
    BLERETRO_LOGF("Connecting\n");
    auto client = NimBLEDevice::createClient(foundDevice->getAddress());
    client->setClientCallbacks(new ClientCallbacks(), true);
    client->setConnectTimeout(5); // Por defecto son 30 segundos
    cnn_status = CnnStatus::Connecting;
    int retryCount = 9;
    do
    {
        BLERETRO_LOGF("Connecting %d\n", retryCount);
        client->connect(true);
        delay(500);
        retryCount--;
    } while (!client->isConnected() && retryCount > 0);

    auto services = client->getServices(true);
    static auto hidServiceUUID = NimBLEUUID("1812");
    for (auto service : *services)
    {
        auto serviceUUID = service->getUUID();
        if (serviceUUID.equals(hidServiceUUID))
        {
            BLERETRO_LOGF("-> Found HID\n");
            auto characteristics = service->getCharacteristics(true);
            for (auto characteristic : *characteristics)
            {
                BLERETRO_LOGF("---> Found characteristic %s\n", characteristic->toString());
                if (characteristic->canRead())
                {
                    BLERETRO_LOGF("-----> Can read...\n");
                    int retryCount = 3;
                    auto str = characteristic->readValue();
                    while (str.size() == 0 && retryCount > 0)
                    {
                        retryCount--;
                        str = characteristic->readValue();
                    }
                    BLERETRO_LOGF("-----> ...'%s'\n", str);
                }
                if (characteristic->canNotify())
                {
                    BLERETRO_LOGF("-----> Can notify. Suscribing.\n");
                    characteristic->subscribe(true, CharacteristicNofifyCB, true);
                    cnn_status = CnnStatus::Connected;
                    BLERETRO_LOGF("Connected.\n");
                }
            }
        }
    }
    if (cnn_status != CnnStatus::Connected)
    {
        NimBLEDevice::deleteClient(client);
        BLERETRO_LOGF("Can't connect.\n");
        cnn_status = CnnStatus::Idle;
    }
}

CnnStatus BLERetro32_Loop()
{
    if (cnn_status == CnnStatus::Idle)
    {
        cnn_status = ScanAndConnect();
    }
    else if (cnn_status == CnnStatus::DeviceFound)
    {
        if (foundDevice != nullptr)
        {
            ConnectToHID(foundDevice);
        }
        else
        {
            cnn_status = CnnStatus::Idle;
        }
    } else if (cnn_status == CnnStatus::Connected)
    {
        DoAutofire();
    }

    return cnn_status;
}
