#ifndef GLOBALS_H
#define GLOBALS_H

#define DEBUG 1

#include "ConfigManager.h"
#include "RTClib.h"
#include <MCP23017.h>
#include "DeviceStatus.h"

extern bool relay[RELAY_NUMBER];
extern int currentRelayIndex;
//extern unsigned long relayActiveSeconds;


extern byte BankA;
extern byte BankB;

extern MCP23017 mcp1;

extern RTC_DS3231 rtc;
extern DateTime now;

extern DeviceStatus rtcStatus;
extern DeviceStatus i2cStatus;
extern DeviceStatus tempStatus;
extern DeviceStatus relayStatus;

#endif
