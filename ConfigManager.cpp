#include "ConfigManager.h"

Preferences prefs;

Config config =
{	
	//Active time per relay in seconds
    .relayDuration = 20*60,
    //Relay channel activation mask (HIGH = active, LOW = skipped)
    .relayEnableMask =
    {
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,  //8
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,  //16
        HIGH,HIGH,LOW,LOW     //18 Active, 20 Available 
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

bool saveConfig()
{
    prefs.begin("timer", false);

    size_t written = prefs.putBytes("config", &config, sizeof(config));

    prefs.end();

    return (written == sizeof(config));
}

bool loadConfig()
{
    prefs.begin("timer", true);   // sola lettura

    if (prefs.getBytesLength("config") != sizeof(config))
    {
        prefs.end();
        return false;             // nessuna configurazione valida
    }

    prefs.getBytes("config", &config, sizeof(config));

    prefs.end();

    return true;
}