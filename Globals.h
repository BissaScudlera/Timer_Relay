#ifndef GLOBALS_H
#define GLOBALS_H

//configuration flags and HW inputs
#define DEBUG 0
extern bool DBG_SETUP;
extern bool FactoryReset;

#include "ConfigManager.h"
#include "RTClib.h"             //v2.1.4  https://github.com/adafruit/RTClib
//Depends from: Adafruit BusIO  //v1.17.4 https://github.com/adafruit/Adafruit_BusIO
#include <MCP23017.h>           //v2.0.0  https://github.com/blemasle/arduino-mcp23017
#include "DeviceStatus.h"

extern bool relay[RELAY_NUMBER];
extern int currentRelayIndex;
//extern unsigned long relayActiveSeconds;


#define MCP1_ENABLE	1
#define MCP1_ADR 0x20

#define MCP2_ENABLE	1
#define MCP2_ADR 0x23

extern byte BankA; //mcp1 portA output
extern byte BankB; //mcp1 portB output
extern byte BankC; //mcp2 portB output
extern byte BankD; //mcp2 portA input

extern MCP23017 mcp1;
extern MCP23017 mcp2;

extern RTC_DS3231 rtc;
extern DateTime now;

extern DeviceStatus rtcStatus;
extern DeviceStatus i2cStatus;
extern DeviceStatus tempStatus;
extern DeviceStatus relayStatus;

#endif
