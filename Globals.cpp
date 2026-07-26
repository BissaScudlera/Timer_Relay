#include "Globals.h"


bool relay[RELAY_NUMBER];
int currentRelayIndex=0;
//unsigned long relayActiveSeconds=0;

byte BankA=0;
byte BankB=0;

MCP23017 mcp1(0x20);

RTC_DS3231 rtc;
DateTime now;


DeviceStatus rtcStatus;
DeviceStatus i2cStatus;
DeviceStatus tempStatus;
DeviceStatus relayStatus;
