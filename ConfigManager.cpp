#include "ConfigManager.h"
#include "Debug.h"

ConfigT configTimer =
{	
	//Active time per relay in seconds
    .relayDuration = 20*60,
    //Relay channel activation mask (HIGH = active, LOW = skipped)
    .relayEnableMask =
    {
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,  //8
        HIGH,HIGH,HIGH,HIGH,
        HIGH,HIGH,HIGH,HIGH,  //16 Active
        LOW,LOW,LOW,LOW,
        LOW,LOW,              //22 Available 
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

bool saveConfigTimer()
{
	Preferences prefs;
	
	if (!prefs.begin("timer", false))
        return false;

    size_t written = prefs.putBytes("config", &configTimer, sizeof(configTimer));

    prefs.end();

    return (written == sizeof(configTimer));
}

bool loadConfigTimer()
{
	Preferences prefs;
	
    if (!prefs.begin("timer", true)) // sola lettura
        return false;   

    if (prefs.getBytesLength("config") != sizeof(configTimer))
    {
        prefs.end();
	    DBG_PRINTLN("[CFG] Error reading Timer settings");
        return false;             // nessuna configurazione valida
    }

    prefs.getBytes("config", &configTimer, sizeof(configTimer));

    prefs.end();

    return true;
}