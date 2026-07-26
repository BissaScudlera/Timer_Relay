#ifndef TIMEBASE_H
#define TIMEBASE_H

#include <Arduino.h>

extern uint32_t currentMillis;

inline uint32_t nowMs()
{
    return currentMillis;
}

#endif