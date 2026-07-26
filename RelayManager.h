#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

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

#endif
