#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "RTCManager.h"
#include "I2CManager.h"

//------------------------------------------------------------
// Stato sintetico dei moduli
//------------------------------------------------------------

inline const char* diagnosticsRtcState()
{
    static char buffer[64];

    const auto& status = rtcGetStatus();

    snprintf(buffer, sizeof(buffer),
         "Running:[%s] Last Error (%d) %s",
         status.available ? "OK" : "ERROR",
         status.lastError,
         status.message);
		 
    return buffer;
}

inline const char* diagnosticsI2CState()
{
    static char buffer[32];

    const auto& status = i2cGetStatus();

    if (status.available)
        return "OK";

    snprintf(buffer, sizeof(buffer), "ERROR (%d)", status.lastError);
    return buffer;
}

inline const char* diagnosticsWiFiState()
{
#ifdef ARDUINO_ARCH_ESP32
    return "OK";
#else
    return "N/A";
#endif
}

inline const char* diagnosticsSummary()
{
    return "RTC / I2C / WiFi";
}

#endif