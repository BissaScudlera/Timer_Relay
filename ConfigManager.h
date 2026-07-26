#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <Arduino.h>

#define RELAY_NUMBER 16

struct Config
{
    unsigned long relayDuration;

    bool relayEnableMask[RELAY_NUMBER];

    bool dayEnableMask[7];

    uint8_t startHour;
    uint8_t startMinute;
    uint8_t startSecond;
};

extern Config config;

#endif