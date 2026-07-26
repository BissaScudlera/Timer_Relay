#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "RTCManager.h"
#include "I2CManager.h"

inline bool rtcOk(){ return rtcGetStatus().available; }
inline bool i2cOk(){ return i2cLastError()==0; }
inline bool wifiOk(){ return true; }

inline const char* diagnosticsSummary(){ return "RTC/I2C/WiFi"; }
inline const char* diagnosticsRtcState(){ return rtcOk() ? "OK":"ERROR"; }
inline const char* diagnosticsI2CState(){ return i2cOk() ? "OK":"ERROR"; }
inline const char* diagnosticsWiFiState(){ return wifiOk() ? "OK":"ERROR"; }

#endif
