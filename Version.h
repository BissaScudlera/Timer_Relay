#ifndef VERSION_H
#define VERSION_H

#define FW_NAME        "Timer Relay ESP32"
#define FW_MAJOR       1
#define FW_MINOR       1
#define FW_PATCH       0

#define FW_BUILD_DATE  __DATE__
#define FW_BUILD_TIME  __TIME__

#define FW_STRING  "1.1.0"

const char* firmwareVersion();

#endif
