        #include "RelayManager.h"
        #include "Globals.h"

static RelayState state = RelayState::Idle;

RelayState relayState(){ return state; }
bool relayRunning(){ return state==RelayState::Starting || state==RelayState::Running; }
void relayStart(){ if(state==RelayState::Idle||state==RelayState::Finished) state=RelayState::Starting; }
void relayStop(){ state=RelayState::Idle; memset(relay,LOW,sizeof(relay)); currentRelayIndex=0; relayActiveSeconds=0; }

        void runTimedSequence() {
  // Automated scheduling evaluation logic
  if (state == RelayState::Idle) {
    if (config.dayEnableMask[now.dayOfTheWeek()]) {
      if (now.hour() == config.startHour && now.minute() == config.startMinute && now.second() == config.startSecond) {
        state = RelayState::Starting; 
      }
    }
  }

  // Run initialization routines on transition edge
  if (state == RelayState::Starting) {
    state = RelayState::Running;
    memset(relay, LOW, sizeof(relay));
    relayActiveSeconds = 0;
    currentRelayIndex = 0;

    // Locate the first non-masked relay index 
    while (currentRelayIndex < RELAY_NUMBER && !config.relayEnableMask[currentRelayIndex]) {
      currentRelayIndex++;
    }
    if (currentRelayIndex < RELAY_NUMBER) { 
      relay[currentRelayIndex] = HIGH; 
      return; 
    } else {
      state = RelayState::Finished;
      return;
    }
  }

  // Process standard ongoing sequencing step logic
  if (state == RelayState::Running) {
    relayActiveSeconds++;

    // Process step interval transitions upon context timeout
    if (relayActiveSeconds >= config.relayDuration) {
      relayActiveSeconds = 0;              
      relay[currentRelayIndex] = LOW;      

      // Advance index pointer while bypassing disabled entries
      do {
        currentRelayIndex++;
      } while (currentRelayIndex < RELAY_NUMBER && !config.relayEnableMask[currentRelayIndex]);

      // Commit to next element state or finalize pipeline execution
      if (currentRelayIndex < RELAY_NUMBER) {
        relay[currentRelayIndex] = HIGH;
      } else {
        state = RelayState::Finished;
        currentRelayIndex = 0;
      }
    }
  }
}
