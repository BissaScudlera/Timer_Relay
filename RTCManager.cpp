#include "I2CManager.h"
#include "RTCManager.h"

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* shortDays[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

static DateTime cachedNow;

static char timeBuffer[9];
static char dateBuffer[11];

static bool rtcReadCached(DateTime &value);

bool rtcInit(void)
{
    rtcStatus.lastCheck = millis();

    if (!rtc.begin())
    {
        rtcStatus.setError(-1);
        return false;
    }

    rtcStatus.setOk();

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
        rtcStatus.setError(-1);
        return false;
    }

    if (!rtcReadCached(cachedNow))
    {
        return false;
    }

    rtcStatus.setOk();
    return true;
}

bool rtcUpdate(void)
{
    rtcStatus.lastCheck = millis();

    if (!rtcReadCached(cachedNow))
    {
        return false;
    }

    rtcStatus.setOk();
    return true;
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

const char* rtcDayString(void)
{
    if (!rtcStatus.available)
    {
        return "-----";
    }
	
    return daysOfTheWeek[cachedNow.dayOfTheWeek()];
}


const char* formatCountTime(uint32_t totalSeconds)
{
    if (totalSeconds <= 0){
		return "--";
	}
	else{
	
	static char buffer[16];

    uint32_t hours   = totalSeconds / 3600;
    uint32_t minutes = (totalSeconds % 3600) / 60;
    uint32_t seconds = totalSeconds % 60;

    if (hours > 0)
    {
        snprintf(
            buffer,
            sizeof(buffer),
            "%2uh %2lum %2lus",
            (unsigned long)hours,
            (unsigned long)minutes,
            (unsigned long)seconds);
    }
    else if (minutes > 0)
    {
        snprintf(
            buffer,
            sizeof(buffer),
            "%lum %2lus",
            (unsigned long)minutes,
            (unsigned long)seconds);
    }
    else
    {
        snprintf(
            buffer,
            sizeof(buffer),
            "%lus",
            (unsigned long)seconds);
    }

    return buffer;
	}
}


static constexpr uint8_t RTC_I2C_ADDRESS = 0x68;

static bool rtcReadCached(DateTime &value)
{
    if (!i2cDevicePresent(RTC_I2C_ADDRESS))
    {
        rtcStatus.setError(i2cLastError());
        return false;
    }

    value = rtc.now();
    rtcStatus.setOk();
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
    rtcStatus.setOk();

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

float rtcTemperature(void)
{
    if (!rtcStatus.available)
        return NAN;

    return rtc.getTemperature();
}