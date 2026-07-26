#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

#include <Arduino.h>


enum class DeviceState { OK, ERROR };


/**
 * @brief Generic device status shared by hardware managers.
 *
 * Each manager (RTC, I2C, WiFi, ...) owns one DeviceStatus instance
 * that describes the current operating state.
 */
struct DeviceStatus
{
    bool available = false;          // Device detected and operational
    bool initialized = false;        // Initialization completed
    bool lastOperationOk = false;
    DeviceState state = DeviceState::ERROR;
    uint32_t lastCheck = 0;
    uint32_t lastOk = 0;
    uint32_t errorCount = 0;    // Last operation result

    uint32_t lastUpdateMs = 0;       // Last status update (millis)
    int16_t lastError = 0;           // Last error code (0 = OK)

    char message[32] = "Not initialized";

    /**
     * @brief Reset the status structure.
     */
    inline void clear()
    {
        available = false;
        initialized = false;
        lastOperationOk = false;
        lastUpdateMs = millis();
        lastError = 0;
        strncpy(message, "Not initialized", sizeof(message));
        message[sizeof(message) - 1] = '\0';
    }

    /**
     * @brief Mark the device as operational.
     */
    inline void setOk(const char *msg = "OK")
    {
        available = true;
        initialized = true;
        lastOperationOk = true;
        lastUpdateMs = millis();
        lastError = 0;

        strncpy(message, msg, sizeof(message));
        message[sizeof(message) - 1] = '\0';
    }

    /**
     * @brief Mark the device as failed.
     */
    inline void setError(int16_t errorCode,
                         const char *msg = "Error")
    {
        lastOperationOk = false;
        lastUpdateMs = millis();
        lastError = errorCode;

        strncpy(message, msg, sizeof(message));
        message[sizeof(message) - 1] = '\0';
    }
};

#endif