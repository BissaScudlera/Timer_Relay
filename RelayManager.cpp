#include "RelayManager.h"
#include "Globals.h"
#include "TimeBase.h"

#include <cstring>

static RelayState state = RelayState::Idle;
static uint32_t relayStartMs = 0;

RelayState relayState()
{
    return state;
}

bool relayRunning()
{
    return state == RelayState::Starting ||
           state == RelayState::Running;
}

void relayStart()
{
    if (state == RelayState::Idle ||
        state == RelayState::Finished)
    {
        state = RelayState::Starting;
    }
}

void relayStop()
{
    state = RelayState::Idle;

    memset(relay, LOW, sizeof(relay));

    currentRelayIndex = 0;
    relayStartMs = 0;
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

    if (elapsed >= config.relayDuration)
        return 0;

    return config.relayDuration - elapsed;
}

uint32_t relayProgramDurationSeconds()
{
    uint8_t activeRelays = 0;

    for (uint8_t i = 0; i < RELAY_NUMBER; i++)
    {
        if (config.relayEnableMask[i])
            activeRelays++;
    }

    return activeRelays * config.relayDuration;
}

void runTimedSequence()
{
    if (state == RelayState::Idle)
    {
        if (config.dayEnableMask[now.dayOfTheWeek()] &&
            now.hour() == config.startHour &&
            now.minute() == config.startMinute &&
            now.second() == config.startSecond)
        {
            state = RelayState::Starting;
        }
    }

    if (state == RelayState::Starting)
    {
        if (config.relayDuration == 0) {
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
               !config.relayEnableMask[currentRelayIndex])
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

    if (state != RelayState::Running)
        return;

    if ((nowMs() - relayStartMs) < (config.relayDuration * 1000UL))
        return;

    relay[currentRelayIndex] = LOW;

    do
    {
        currentRelayIndex++;
    }
    while (currentRelayIndex < RELAY_NUMBER &&
           !config.relayEnableMask[currentRelayIndex]);

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