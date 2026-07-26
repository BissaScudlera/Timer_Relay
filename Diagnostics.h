#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "RTCManager.h"
#include "I2CManager.h"

//------------------------------------------------------------
// Stato sintetico dei moduli
//------------------------------------------------------------

inline const char* diagnosticsRtcState()
{
    return rtcGetStatus().available ? "OK" : "ERROR";
}

inline const char* diagnosticsI2CState()
{
    return i2cGetStatus().available ? "OK" : "ERROR";
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