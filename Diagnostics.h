rtcOk()

i2cOk()

wifiOk()


// Returns a compact diagnostics summary.
inline const char* diagnosticsSummary(){ return "RTC/I2C/WiFi"; }


// Diagnostic helper APIs
inline const char* diagnosticsRtcState(){ return rtcOk() ? "OK" : "ERROR"; }
inline const char* diagnosticsI2CState(){ return i2cOk() ? "OK" : "ERROR"; }
inline const char* diagnosticsWiFiState(){ return wifiOk() ? "OK" : "ERROR"; }
