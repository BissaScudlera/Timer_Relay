#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

#include <Arduino.h>

enum class DeviceState { OK, ERROR };

struct DeviceStatus
{
    bool available = false;
    bool initialized = false;
    bool lastOperationOk = false;
    DeviceState state = DeviceState::ERROR;
    uint32_t lastCheck = 0;
    uint32_t lastOk = 0;
    uint32_t errorCount = 0;

    uint32_t lastUpdateMs = 0;
    int16_t lastError = 0;

    char message[32] = "Not initialized";

    inline void clear()
    {
        available = false;
        initialized = false;
        lastOperationOk = false;
        state = DeviceState::ERROR;

        lastCheck = 0;
        lastOk = 0;
        errorCount = 0;

        lastUpdateMs = millis();
        lastError = 0;

        strncpy(message, "Not initialized", sizeof(message));
        message[sizeof(message) - 1] = '\0';
    }

    inline void setOk(const char *msg = "OK")
    {
        available = true;
        initialized = true;
        lastOperationOk = true;
        state = DeviceState::OK;

        lastUpdateMs = millis();
        lastOk = lastUpdateMs;
        //lastError = 0;

        //strncpy(message, msg, sizeof(message));
        //message[sizeof(message) - 1] = '\0';
    }

    inline void setError(int16_t errorCode,
                         const char *msg = " ")
    {
        available = false;
        lastOperationOk = false;
        state = DeviceState::ERROR;

        lastUpdateMs = millis();
        lastError = errorCode;
        errorCount++;

        strncpy(message, msg, sizeof(message));
        message[sizeof(message) - 1] = '\0';
    }
};

#endif
