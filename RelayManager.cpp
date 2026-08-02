#include "RelayManager.h"
#include "Globals.h"
#include "TimeBase.h"
#if DEBUG
 #include "Debug.h"
#endif
 

#include <cstring>

static RelayState state = RelayState::Idle;
static uint32_t relayStartMs = 0;

RelayState relayState()
{
    return state;
}

const char* relayStateString()
{
    switch (relayState())
    {
        case RelayState::Idle:
            return "Idle";

        case RelayState::Starting:
            return "Starting";

        case RelayState::Running:
            return "Running";

        case RelayState::Finished:
            return "Finished";

        default:
            return "Unknown";
    }
}

bool relayRunning()
{
    return state == RelayState::Running;
}

void relayStart()
{
    if (state == RelayState::Idle)
    {
        state = RelayState::Starting;
    }
}

void relayStop()
{
    state = RelayState::Finished;
}

uint32_t relayElapsedSeconds()
{
    if (!relayRunning() || relayStartMs == 0)
        return 0;

    return (nowMs() - relayStartMs) / 1000UL;
}

uint32_t relayRemainingSeconds()
{
    uint32_t elapsed = relayElapsedSeconds();

    if (elapsed >= configTimer.relayDuration)
        return 0;

    return configTimer.relayDuration - elapsed;
}

uint32_t relayProgramDurationSeconds()
{
    uint8_t activeRelays = 0;

    for (uint8_t i = 0; i < RELAY_NUMBER; i++)
    {
        if (configTimer.relayEnableMask[i])
            activeRelays++;
    }

    return activeRelays * configTimer.relayDuration;
}

void runTimedSequence()
{
    if (state == RelayState::Idle)
    {
		DBG_PRINTLN("[relay manager] timed sequence: IDLE");
        if (configTimer.dayEnableMask[now.dayOfTheWeek()] &&
            now.hour() == configTimer.startHour &&
            now.minute() == configTimer.startMinute &&
            now.second() == configTimer.startSecond)
        {
            state = RelayState::Starting;
        }
    }
	
	if (state == RelayState::Finished)
    {
        DBG_PRINTLN("[relay manager] timed sequence: FINISHED");
    
        currentRelayIndex = 0;
        relayStartMs = 0;
    
        memset(relay, LOW, sizeof(relay));
    
        state = RelayState::Idle;
    
        return;
    }

    if (state == RelayState::Starting)
    {
		DBG_PRINTLN("[relay manager] timed sequence: STARTING");
        if (configTimer.relayDuration == 0) {
          state = RelayState::Finished;
          currentRelayIndex = 0;
          relayStartMs = 0;
          return;
        }
        state = RelayState::Running;

        memset(relay, LOW, sizeof(relay));

        currentRelayIndex = 0;
        relayStartMs = 0;

        while (currentRelayIndex < RELAY_NUMBER &&
               !configTimer.relayEnableMask[currentRelayIndex])
        {
            currentRelayIndex++;
        }

        if (currentRelayIndex < RELAY_NUMBER)
        {
            relay[currentRelayIndex] = HIGH;
            relayStartMs = nowMs();
        }
        else
        {
            state = RelayState::Finished;
        }

        return;
    }

    if (state != RelayState::Running){
		DBG_PRINTLN("[relay manager] timed sequence: NOT RUNNING");
        return;
	}
    DBG_PRINTLN("[relay manager] timed sequence: RUNNING");
	
    if ((nowMs() - relayStartMs) < (configTimer.relayDuration * 1000UL))
        return;

    relay[currentRelayIndex] = LOW;

    do
    {
        currentRelayIndex++;
    }
    while (currentRelayIndex < RELAY_NUMBER &&
           !configTimer.relayEnableMask[currentRelayIndex]);

    if (currentRelayIndex < RELAY_NUMBER)
    {
        relay[currentRelayIndex] = HIGH;
        relayStartMs = nowMs();
    }
    else
    {
        state = RelayState::Finished;
        currentRelayIndex = 0;
        relayStartMs = 0;
    }
}