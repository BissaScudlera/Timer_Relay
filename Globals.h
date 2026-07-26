#ifndef GLOBALS_H
#define GLOBALS_H

#include "Scheduler.h"
#include "RTClib.h"
#include <MCP23017.h>
#include "DeviceStatus.h"

#define RELAY_NUMBER 16

#ifndef DEBUG
#define DEBUG 1
#endif

extern TaskTimer rtcTask;
extern TaskTimer i2cTask;
extern TaskTimer sensorTask;
extern TaskTimer diagTask;
extern TaskTimer displayTask;
extern TaskTimer eepromTask;
extern TaskTimer rtcRecoveryTask;

extern RTC_DS3231 rtc;
extern DateTime now;
extern bool rtcFound;

extern MCP23017 mcp1;

extern byte BankA;
extern byte BankB;

extern DeviceStatus rtcStatus;
extern DeviceStatus i2cStatus;
extern DeviceStatus tempStatus;
extern DeviceStatus relayStatus;

extern bool sequenceActive;
extern bool sequenceInit;

extern int currentRelayIndex;
extern unsigned long relayActiveSeconds;

extern unsigned long relayDuration;

extern const int startHour;
extern const int startMinute;
extern const int startSecond;

extern bool relay[RELAY_NUMBER];
extern bool relayEnableMask[RELAY_NUMBER];
extern bool dayEnableMask[7];

#endif
