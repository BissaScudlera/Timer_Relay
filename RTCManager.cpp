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
    rtcStatus.lastOk = millis();

    cachedNow = rtc.now();

    return true;
}

bool rtcUpdate(void)
{
    return rtcStatus.available;
}

bool rtcAvailable(void)
{
    return rtcStatus.available;
}

DateTime rtcNow(void)
{
    if (rtcStatus.available)
    {
        cachedNow = rtc.now();
    }

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