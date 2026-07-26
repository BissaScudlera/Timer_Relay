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
    uint32_t now = nowMs();

    if ((uint32_t)(now - task.lastRunMs) >= task.periodMs)
    {
        task.lastRunMs = now;
        return true;
    }

    return false;
}

inline void taskReset(TaskTimer &task)
{
    task.lastRunMs = nowMs();
}

inline void taskForce(TaskTimer &task)
{
    task.lastRunMs = nowMs() - task.periodMs;
}

inline uint32_t taskElapsed(const TaskTimer &task)
{
    return nowMs() - task.lastRunMs;
}

inline uint32_t taskRemaining(const TaskTimer &task)
{
    uint32_t elapsed = nowMs() - task.lastRunMs;

    if (elapsed >= task.periodMs)
        return 0;

    return task.periodMs - elapsed;
}



#endif