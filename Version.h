#ifndef VERSION_H
#define VERSION_H

//=============================================================================
// Firmware Identification
//=============================================================================

#define FW_NAME            "Timer Relay ESP32"

#define FW_MAJOR           1
#define FW_MINOR           5
#define FW_PATCH           12

// Incrementare ad ogni compilazione ufficiale
#define FW_BUILD           1

//=============================================================================
// Build Information
//=============================================================================

#define FW_BUILD_DATE      __DATE__
#define FW_BUILD_TIME      __TIME__

//=============================================================================
// Hardware
//=============================================================================

#define HW_NAME            "ESP32 DEVKIT V1"
#define HW_REVISION        "1.0"

//=============================================================================
// Project
//=============================================================================

#define PROJECT_AUTHOR     "BissaScudlera"
#define PROJECT_REPOSITORY "https://github.com/BissaScudlera/Timer_Relay"

#endif

// Public API
const char* getFirmwareVersion();
const char* getBuildDate();
const char* getHardwareName();
