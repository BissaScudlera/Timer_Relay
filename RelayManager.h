#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

#include <Arduino.h>

enum class RelayState
{
    Idle,
    Starting,
    Running,
    Finished
};

void runTimedSequence();
RelayState relayState();
bool relayRunning();
void relayStart();
void relayStop();
uint32_t relayElapsedSeconds();
uint32_t relayRemainingSeconds();
uint32_t relayProgramDurationSeconds();

#endif
