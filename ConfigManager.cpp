#include "ConfigManager.h"

Config config =
{
    .relayDuration = 60,

    .relayEnableMask =
    {
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH
    },

    .dayEnableMask =
    {
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH
    },

    .startHour = 23,
    .startMinute = 0,
    .startSecond = 0
};