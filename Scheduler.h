#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

struct TaskTimer
{
    uint32_t periodMs;
    uint32_t lastRunMs;
};

inline bool taskExpired(TaskTimer &task)
{
    uint32_t now = millis();

    if ((uint32_t)(now - task.lastRunMs) >= task.periodMs)
    {
        task.lastRunMs = now;
        return true;
    }

    return false;
}

#endif