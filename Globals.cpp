#include "Globals.h"

//configuration flags and HW inputs
//#define DEBUG 1
bool DBG_SETUP= false;
bool FactoryReset= false;

bool relay[RELAY_NUMBER];
int currentRelayIndex=0;
//unsigned long relayActiveSeconds=0;

byte BankA=0;
byte BankB=0;
byte BankC=0;
byte BankD=0;

//#if MCP1_ENABLE
MCP23017 mcp1(MCP1_ADR);
//#endif
//#if MCP2_ENABLE
MCP23017 mcp2(MCP2_ADR);
//#endif

RTC_DS3231 rtc;
DateTime now;


DeviceStatus rtcStatus;
DeviceStatus i2cStatus;
DeviceStatus tempStatus;
DeviceStatus relayStatus;
