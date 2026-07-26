#include "I2CManager.h"
#include "RTCManager.h"


static DateTime cachedNow;

static char timeBuffer[9];
static char dateBuffer[11];

bool rtcInit(void)
{
    rtcStatus.lastCheck = millis();

    if (!rtc.begin())
    {
        rtcStatus.state = DeviceState::ERROR;
        rtcStatus.available = false;
        rtcStatus.lastError = -1;
        rtcStatus.errorCount++;

        return false;
    }

    rtcStatus.state = DeviceState::OK;
    rtcStatus.available = true;
    rtcStatus.lastOk = rtcStatus.lastCheck;

    cachedNow = rtc.now();

    return true;
}

bool rtcRecover(void)
{
    if (rtcStatus.available)
    {
        return true;
    }

    if (!i2cDevicePresent(0x68))
    {
        return false;
    }

    if (!rtc.begin())
    {
        rtcStatus.lastError = -1;
        return false;
    }

    cachedNow = rtc.now();

    rtcStatus.available = true;
    rtcStatus.state = DeviceState::OK;
    rtcStatus.lastError = 0;
    rtcStatus.lastOk = millis();

    return true;
}

bool rtcUpdate(void)
{
    rtcStatus.lastCheck = millis();

    if (!i2cDevicePresent(0x68))
{
    if (rtcStatus.available)
    {
        rtcStatus.errorCount++;
    }

    rtcStatus.available = false;
    rtcStatus.state = DeviceState::ERROR;
    rtcStatus.lastError = i2cLastError();

    return false;
}

    rtcStatus.available = true;
    rtcStatus.state = DeviceState::OK;
    rtcStatus.lastError = 0;

// TODO v1.1.8
// Lettura protetta con gestione recovery
    cachedNow = rtc.now();

    rtcStatus.lastOk = rtcStatus.lastCheck;

    return true;
}

bool rtcAvailable(void)
{
    return rtcStatus.available;
}

DateTime rtcNow(void)
{
    return cachedNow;
}

const char* rtcTimeString(void)
{
    if (!rtcStatus.available)
    {
        return "--:--:--";
    }

    snprintf(
        timeBuffer,
        sizeof(timeBuffer),
        "%02d:%02d:%02d",
        cachedNow.hour(),
        cachedNow.minute(),
        cachedNow.second());

    return timeBuffer;
}

const char* rtcDateString(void)
{
    if (!rtcStatus.available)
    {
        return "--/--/----";
    }

    snprintf(
        dateBuffer,
        sizeof(dateBuffer),
        "%02d/%02d/%04d",
        cachedNow.day(),
        cachedNow.month(),
        cachedNow.year());

    return dateBuffer;
}