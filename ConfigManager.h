#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <Arduino.h>


#define RELAY_NUMBER 20

struct Config
{
    //Active time per relay in seconds
	unsigned long relayDuration;
	
	//Relay channel activation mask (HIGH = active, LOW = skipped)
    bool relayEnableMask[RELAY_NUMBER];

    //Weekday execution mask (Index 0 = Sunday, ..., 6 = Saturday)
	bool dayEnableMask[7];

    //Target start time configuration (24h format)
	uint8_t startHour;
    uint8_t startMinute;
    uint8_t startSecond;
};

extern Config config;

#endif