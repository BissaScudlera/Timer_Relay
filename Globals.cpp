#include "Globals.h"

TaskTimer rtcTask         = {1000, 0};
TaskTimer i2cTask         = {5000, 0};
TaskTimer sensorTask      = {1000, 0};
TaskTimer diagTask        = {10000, 0};
TaskTimer displayTask     = {250, 0};
TaskTimer eepromTask      = {1000, 0};
TaskTimer rtcRecoveryTask = {5000, 0};


RTC_DS3231 rtc;
DateTime now;
bool rtcFound=false;

MCP23017 mcp1(0x20);

byte BankA=0;
byte BankB=0;

DeviceStatus rtcStatus;
DeviceStatus i2cStatus;
DeviceStatus tempStatus;
DeviceStatus relayStatus;

bool sequenceActive=false;
bool sequenceInit=false;

int currentRelayIndex=0;
unsigned long relayActiveSeconds=0;


unsigned long relayDuration = 1 * 60;

const int startHour = 23;
const int startMinute = 0;
const int startSecond = 0;

bool relayEnableMask[RELAY_NUMBER] =
{
    HIGH, HIGH, HIGH, HIGH,
    HIGH, HIGH, HIGH, HIGH,
    HIGH, HIGH, HIGH, HIGH,
    HIGH, HIGH, HIGH, HIGH
};

bool dayEnableMask[7] =
{
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH
};

bool relay[RELAY_NUMBER];