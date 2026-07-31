#include "ConfigManager.h"

Config config =
{
	//Active time per relay in seconds
    .relayDuration = 60,
    //Relay channel activation mask (HIGH = active, LOW = skipped)
    .relayEnableMask =
    {
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH
    },
	//Weekday execution mask (Index 0 = Sunday, ..., 6 = Saturday)
    .dayEnableMask =
    {
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH
    },
    //Target start time configuration (24h format)
    .startHour = 23,
    .startMinute = 0,
    .startSecond = 0
};