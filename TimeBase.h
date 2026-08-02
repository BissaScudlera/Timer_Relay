#ifndef TIMEBASE_H
#define TIMEBASE_H

#include <Arduino.h>
#include "Scheduler.h"

extern uint32_t currentMillis;

//extern TaskTimer rtcTask;
//extern TaskTimer i2cTask;
//extern TaskTimer sensorTask;
//extern TaskTimer diagTask;
//extern TaskTimer displayTask;
//extern TaskTimer eepromTask;
//extern TaskTimer rtcRecoveryTask;

inline uint32_t nowMs()
{
    return currentMillis;
}

#endif
