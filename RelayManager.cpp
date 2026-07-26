        #include "RelayManager.h"
        #include "Globals.h"

        void runTimedSequence() {
  // Automated scheduling evaluation logic
  if (!sequenceActive) {
    if (dayEnableMask[now.dayOfTheWeek()]) {
      if (now.hour() == startHour && now.minute() == startMinute && now.second() == startSecond) {
        sequenceActive = true;
        sequenceInit = false; 
      }
    }
  }

  // Run initialization routines on transition edge
  if (sequenceActive && !sequenceInit) {
    sequenceInit = true;
    memset(relay, LOW, sizeof(relay));
    relayActiveSeconds = 0;
    currentRelayIndex = 0;

    // Locate the first non-masked relay index 
    while (currentRelayIndex < RELAY_NUMBER && !relayEnableMask[currentRelayIndex]) {
      currentRelayIndex++;
    }
    if (currentRelayIndex < RELAY_NUMBER) { 
      relay[currentRelayIndex] = HIGH; 
      return; 
    } else {
      sequenceActive = false; // Graceful exit if all entries are masked out
      return;
    }
  }

  // Process standard ongoing sequencing step logic
  if (sequenceActive && sequenceInit) {
    relayActiveSeconds++;

    // Process step interval transitions upon context timeout
    if (relayActiveSeconds >= relayDuration) {
      relayActiveSeconds = 0;              
      relay[currentRelayIndex] = LOW;      

      // Advance index pointer while bypassing disabled entries
      do {
        currentRelayIndex++;
      } while (currentRelayIndex < RELAY_NUMBER && !relayEnableMask[currentRelayIndex]);

      // Commit to next element state or finalize pipeline execution
      if (currentRelayIndex < RELAY_NUMBER) {
        relay[currentRelayIndex] = HIGH;
      } else {
        sequenceActive = false;            
        sequenceInit = false;
        currentRelayIndex = 0;
      }
    }
  }
}
