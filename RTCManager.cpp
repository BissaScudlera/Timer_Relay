#include "I2CManager.h"
#include "RTCManager.h"


static DateTime cachedNow;

static char timeBuffer[9];
static char dateBuffer[11];

static bool rtcReadCached(DateTime &value);

static void rtcSetError(int error)
{
    if (rtcStatus.available)
    {
        rtcStatus.errorCount++;
    }
    rtcStatus.available = false;
    rtcStatus.state = DeviceState::ERROR;
    rtcStatus.lastError = error;
}

static void rtcSetOk()
{
    rtcStatus.available = true;
    rtcStatus.state = DeviceState::OK;
    rtcStatus.lastError = 0;
}

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

    if (!rtcReadCached(cachedNow))
    {
        return false;
    }

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

    if (!rtcReadCached(cachedNow))
    {
        return false;
    }

    rtcStatus.lastOk = millis();

    return true;
}

bool rtcUpdate(void)
{
    rtcStatus.lastCheck = millis();

    if (!rtcReadCached(cachedNow))
    {
        return false;
    }

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

static constexpr uint8_t RTC_I2C_ADDRESS = 0x68;

static bool rtcReadCached(DateTime &value)
{
    if (!i2cDevicePresent(RTC_I2C_ADDRESS))
    {
        rtcSetError(i2cLastError());
        return false;
    }

    value = rtc.now();
    rtcSetOk();
    return true;
}


const DeviceStatus& rtcGetStatus(void)
{
    return rtcStatus;
}

bool rtcSetDateTime(const DateTime& dt)
{
    if (!rtcStatus.available)
        return false;

    rtc.adjust(dt);
    cachedNow = dt;
    rtcStatus.lastOk = millis();
    rtcStatus.lastError = 0;
    rtcStatus.state = DeviceState::OK;

    return true;
}

bool rtcSetTime(uint8_t hour, uint8_t minute)
{
    DateTime now;

    if (!rtcReadCached(now))
        return false;

    DateTime updated(
        now.year(),
        now.month(),
        now.day(),
        hour,
        minute,
        0);

    return rtcSetDateTime(updated);
}