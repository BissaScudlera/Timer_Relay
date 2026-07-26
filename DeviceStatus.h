#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

#include <Arduino.h>

enum class DeviceState : uint8_t
{
    UNKNOWN = 0,
    OK,
    ERROR,
    RECOVERY
};

struct DeviceStatus
{
    DeviceState state = DeviceState::UNKNOWN;

    int16_t lastError = 0;

    uint32_t lastCheck = 0;

    uint32_t lastOk = 0;

    uint32_t errorCount = 0;

    bool available = false;
};

#endif