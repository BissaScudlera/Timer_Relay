
/*********************************************
 Relay Timer
 V 0.9 (Multi-Architecture Update Nano/ESP32)
 Jack, Gemini and ChatGPT
**********************************************/
#include <Arduino.h>
#include "Version.h"
#include "Scheduler.h"
#include "Globals.h"
#include "DeviceStatus.h"
#include "RTCManager.h"
#include "I2CManager.h"

const int DEBUG = true; //false; // Software override for debug jumpers

/*- SETUP PARAMETERS ------------------------------------------------------------------*/
const int relayNumber = 16;  // Total number of connected relays

// Active time per relay in seconds
unsigned long relayDuration = 1*60;

// Target start time configuration (24h format)
const int startHour = 23;
const int startMinute = 0;
const int startSecond = 0;

// Relay channel activation mask (HIGH = active, LOW = skipped)
bool relayEnableMask[relayNumber] = {
  HIGH, HIGH, HIGH, HIGH,  // Relays 1-4
  HIGH, HIGH, HIGH, HIGH,  // Relays 5-8
  HIGH, HIGH, HIGH, HIGH,  // Relays 9-12
  HIGH, HIGH, HIGH, HIGH   // Relays 13-16
};

// Weekday execution mask (Index 0 = Sunday, ..., 6 = Saturday)
bool dayEnableMask[7] = {
  HIGH, // Sunday
  HIGH, // Monday
  HIGH, // Tuesday
  HIGH, // Wednesday
  HIGH, // Thursday
  HIGH, // Friday
  HIGH  // Saturday
};

/*-------------------------------------------------------------------------------------*/

// Architecture-specific I/O and communication settings----------------------------------
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
  // ESP32 DevKit V1 configuration
  #include "WebServer.h"
  const int iTriggerButton = 4;  // PIN D2: Manual Sequence Start, debounced push button
  const int iResetButton   = 5;  // PIN D5: stops active sequence, long press to toggle wifi
  const int iComDebug      = 16; // PIN RX2 (GPIO 16): Enable Serial Debug, jumper read once on startup
  const int iOutDebug      = 17; // PIN TX2 (GPIO 17): Enable Output Testing, jumper read once on startup
  const int oLedDebug      = LED_BUILTIN; // Uses ESP32 native built-in LED (GPIO 2)
  const unsigned long SERIAL_BAUD = 115200; // Recommended baud rate for ESP32
#else
  // Arduino Nano configuration
  const int iTriggerButton = 2;  // PIN D2: Manual Sequence Start, debounced push button 
  const int iResetButton   = 5;  // PIN D5: stops active sequence
  const int iComDebug      = 3;  // PIN D3: Enable Serial Debug, jumper read once on startup
  const int iOutDebug      = 4;  // PIN D4: Enable Output Testing, jumper read once on startup
  const int oLedDebug      = LED_BUILTIN; // Uses Nano native built-in LED (Pin 13)
  const unsigned long SERIAL_BAUD = 9600;
#endif
//---------------------------------------------------------------------------------------

// Debug and initialization flags
bool isFirstRun = true;
int ErrState = 0;
int ComDebug = false;
int OutDebug = false;
int MsgNum;

// Subroutine execution timers
unsigned long currentMillis = 0;  
unsigned long previousMillis;

#include <Wire.h> //I2C library
DeviceStatus rtcStatus;

DeviceStatus i2cStatus;

DeviceStatus tempStatus;

DeviceStatus relayStatus;

// DS3231 RTC configurations
#include "RTClib.h"
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* shortDays[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
RTC_DS3231 rtc;
DateTime now;
bool rtcFound;

// MCP23017 I2C Port Expander configurations
#include <MCP23017.h> 
MCP23017 mcp1 = MCP23017(0x20);
byte BankA; // Relays 1 to 8
byte BankB; // Relays 9 to 16

// Relay sequence control state variables
bool relay[relayNumber];
bool sequenceActive = false;           // Active sequence execution flag
bool sequenceInit = false;             // Sequence initialization flag
int currentRelayIndex = 0;             // Active relay element pointer
unsigned long relayActiveSeconds = 0;  // Elapsed runtime tracker for current step

// Hardware input debounce filters
int lastButtonState = HIGH;              // Default idle high due to internal pull-up
unsigned long lastDebounceTime = 0;      // Transient timer snapshot
const unsigned long debounceDelay = 50;  // Settling threshold interval
unsigned long t_iReset = 0;                  // Stores the millisecond timestamp when pressing starts
bool lastResetStatus = false;                // Tracking flag for the button state
const unsigned long T_iResetLong = 5000;     // Threshold for long press action (3 seconds)
const unsigned long T_iResetShort = 50;      // Minimum debounce threshold for short press action (50ms)

// Forward function declarations
void runTimedSequence();
void checkManualTrigger();
void checkResetButton();
void SerialMonitor(); 
bool DeviceAlive();
void RelayTest();
void printBin8();
void serialPause();
void checkSystemResetReason();
#ifdef WEB_SERVER_H
  String generaHtmlRele();
  String generaHtmlGiorni();
#endif


void setup() {
  // Initialize hardware digital I/O channels
  pinMode(oLedDebug, OUTPUT);
  pinMode(iTriggerButton, INPUT_PULLUP);
  pinMode(iResetButton, INPUT_PULLUP);
  pinMode(iComDebug, INPUT_PULLUP);
  pinMode(iOutDebug, INPUT_PULLUP);

  // Evaluate status configurations from hardware strapping pins
  ComDebug = !digitalRead(iComDebug);
  OutDebug = !digitalRead(iOutDebug);

  // Clear memory registers for all output elements
  memset(relay, LOW, sizeof(relay));

  // Initialize hardware serial console
  if(ComDebug | DEBUG){
    Serial.begin(SERIAL_BAUD);
    // Hold execution until terminal connects
   uint32_t serialStart = millis();
    while (!Serial && (millis() - serialStart < 2000)) {
    yield();
    }
    //Serial.print("\n\x1B[H"\n); // Clear screen terminal escape sequence
    checkSystemResetReason();
    Serial.print("Debug flags:");
    Serial.print(DEBUG);
    Serial.print(ComDebug);
    Serial.print(OutDebug);
    Serial.println('\n');
  }

  // Conditional I2C interface hardware setup
    i2cBegin();  // Assign physical hardware pins for ESP32 I2C bus
    #define _ESP32_BUS

  // Configure MCP23017 GPIO data direction registers
  if (DeviceAlive(0x20,"Relay Interface")){
    mcp1.portMode(MCP23017Port::A, 0); // Define Port A banks as digital output channels
    mcp1.portMode(MCP23017Port::B, 0); // Define Port B banks as digital output channels
    mcp1.writeRegister(MCP23017Register::GPIO_A, 0xFF);  // Purge/Reset internal latch register A 
    mcp1.writeRegister(MCP23017Register::GPIO_B, 0xFF);  // Purge/Reset internal latch register B
  }
  
  // Validate real-time clock operational status
  rtcFound = rtcInit();
  if (!rtcFound) {
    DeviceAlive(0x68, "RTC");
    if (ComDebug | DEBUG) {
      Serial.println("RTC setup failed");
    }
  }
  else{
    if (rtc.lostPower()){
      ErrState++;
      if (ComDebug | DEBUG) {
        Serial.println("RTC lost power, set the time!");
      }
    }
    // Deactivate hardware auxiliary square wave generation channels
    rtc.disable32K();
    rtc.disableAlarm(1);
    rtc.disableAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);
  }
  
  #ifdef WEB_SERVER_H
    serverSetup();
  #endif

  if(ComDebug | DEBUG){
    Serial.print("\n\nSetup errors: ");
    Serial.println(ErrState);
    serialPause();
  }
}
void loop() {
  yield();; //Prevent ESP32 watchdog triggers by yielding processor time to background routines
  
  #ifdef WEB_SERVER_H
    serverLoop();
  #endif

  currentMillis = millis();
  checkManualTrigger();
  checkResetButton();

  // Primary execution block triggered exactly once per second
  if (currentMillis - previousMillis >= 1000) {
    rtcUpdate();
    previousMillis = currentMillis; 
    // Handle I2C peripheral hardware bus errors (AVR architecture only)
    #if !defined(_ESP32_BUS)
      if (Wire.getWireTimeoutFlag()) {
        Wire.clearWireTimeoutFlag();
        ErrState++;
        if ( DEBUG | ComDebug ){ 
          Serial.println("I2C bus timeout");
        }
      }
    #endif

    // Query active clock timestamp from hardware rtc element
    if ( rtcFound & DeviceAlive(0x68, "RTC") ){
      now = rtcNow();
    }

    // Evaluate automated sequence pipeline cycles
    runTimedSequence();

    // Map linear state array into 8-bit registers for I2C transmissions
    BankA = 0;
    BankB = 0;
    for (int i = 0; i < 8; i++) {
      if (i < relayNumber) {
        BankA |= (relay[i] << i);
      }
    }
    for (int i = 0; i < 8; i++) {
      if ((i + 8) < relayNumber) {
        BankB |= (relay[i + 8] << i);
      }
    }

    // Refresh current states across physical peripheral devices
    if (DeviceAlive(0x20,"Relay Interface")){
      mcp1.writePort(MCP23017Port::A, ~BankA);
      mcp1.writePort(MCP23017Port::B, ~BankB);
    }
    if (ErrState != 0){
      digitalWrite(oLedDebug, HIGH);
    }
    
    // Process diagnostics outputs
    RelayTest();  
    SerialMonitor(); 
    isFirstRun = false;
    ErrState = 0;
  }
}

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
    while (currentRelayIndex < relayNumber && !relayEnableMask[currentRelayIndex]) {
      currentRelayIndex++;
    }
    if (currentRelayIndex < relayNumber) { 
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
      } while (currentRelayIndex < relayNumber && !relayEnableMask[currentRelayIndex]);

      // Commit to next element state or finalize pipeline execution
      if (currentRelayIndex < relayNumber) {
        relay[currentRelayIndex] = HIGH;
      } else {
        sequenceActive = false;            
        sequenceInit = false;
        currentRelayIndex = 0;
      }
    }
  }
}

void checkManualTrigger() {
  int reading = digitalRead(iTriggerButton);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
    lastButtonState = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Assert active sequence status if edge requirements evaluate true
    if (reading == LOW && !sequenceActive) {
      sequenceActive = true;
      sequenceInit = false; 
    }
  }
}

void checkResetButton() {
  int actStatus = digitalRead(iResetButton);

  if (actStatus == LOW) { // Button is pressed (active low)
    if (!lastResetStatus) {
      t_iReset = millis();
      lastResetStatus = true;
    }
    
    // Check if the button is held down past the long press threshold
    if (lastResetStatus && (millis() - t_iReset >= T_iResetLong)) {
      lastResetStatus = false; // Reset state to prevent continuous cyclic execution
      
      // ENCLOSED IN IFDEF: CONTAINS EXCLUSIVELY THE CALL TO TOGGLEWEBSERVER
      #if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
        toggleWebServer();
      #endif
      
      // Hardware debounce wait loop for long press release
      while(digitalRead(iResetButton) == LOW) { delay(10); }
    }
  } 
  else { // Button has been released (HIGH)
    if (lastResetStatus) {
      unsigned long duration = millis() - t_iReset;
      lastResetStatus = false; // Reset state

      // SHORT PRESS: Executed if duration is under long threshold and above minimum debounce
      if (duration < T_iResetLong && duration > T_iResetShort) {
        if (sequenceActive) {
          sequenceActive = false;
          sequenceInit = false;
          for (int i = 0; i < relayNumber; i++) {
            relay[i] = LOW; // Immediately turn off all sequence relays
          }
          if (ComDebug | DEBUG) {
            Serial.println("\n[HARDWARE] Short Press: Relay sequence RESET.");
          }
        }
      }
    }
  }
}


void SerialMonitor(){
  if(!ComDebug & !DEBUG){ return; }
  
  Serial.print("\n\x1B[H\n- "); 
  Serial.print(MsgNum);
  Serial.println(" ---------------------------");

  Serial.print("System status: ");
  Serial.println(ErrState);

  Serial.print("Inputs: ");
  Serial.print(lastButtonState);
  Serial.print(ComDebug);
  Serial.println(OutDebug);

  // Format and print current hardware RTC timestamps
  if (DeviceAlive(0x68, "RTC")) {
    Serial.print("\nRTC Date : ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print("\nRTC Time : ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print("\nTemperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
  }

  // Format scheduling metrics
  Serial.print("Target Time: [");
  if(startHour < 10) Serial.print('0'); Serial.print(startHour); Serial.print(':');
  if(startMinute < 10) Serial.print('0'); Serial.print(startMinute); Serial.print(':');
  if(startSecond < 10) Serial.print('0'); Serial.print(startSecond);
  Serial.println("]");
  Serial.print("Enabled Relays: [");
  for(int i = 0; i < 7; i++) { Serial.print(relayEnableMask[i] ? '1' : '0'); }
  Serial.println("]");
  Serial.print("Active Days: [");
  for(int i = 0; i < 7; i++) {
    if (dayEnableMask[i]) {
      Serial.print(shortDays[i]); 
    } else {
      Serial.print("--");         
    }
    if(i < 6) Serial.print(' ');  
  }
  Serial.println("]");

  // Update pipeline state parameters
  Serial.print("Sequence Status : ");
  if (sequenceActive) {
    Serial.print("ACTIVE | Current Relay Index: ");
    Serial.print(currentRelayIndex + 1); 
    Serial.print("/");
    Serial.print(relayNumber);
    Serial.print(" | Time Remaining: ");
    Serial.print(relayDuration - relayActiveSeconds);
    Serial.println("s");
  } else {
    Serial.println("IDLE (Waiting for trigger)");
  }
  // ----------------------------------------------------------

  // Output current software matrix state parameters
  Serial.print("Output SW: [");
  for (int i = 0; i < relayNumber; i++) {
    Serial.print(relay[i] ? '1' : '0'); 
    if (i == 7) Serial.print(' ');      
  }
  Serial.println("]");
  
  // Output literal internal peripheral latched configuration profiles
  if(DeviceAlive(0x20,"Relay Interface")){
    Serial.print("Output HW: [");
    printBin8(mcp1.readRegister(MCP23017Register::OLAT_B));
    Serial.print(' ');
    printBin8(mcp1.readRegister(MCP23017Register::OLAT_A));
    Serial.println("]");
  }

  MsgNum++;
  return;
}

bool DeviceAlive( byte Address, const char* Name ){
  byte error=0;
  Wire.beginTransmission(Address);
  error = Wire.endTransmission();
  if (error != 0) {
    ErrState++;
    if ( DEBUG | ComDebug ){
      Serial.print("Couldn't find ");
      Serial.print(Name);
      Serial.print(". Error: ");
      switch (error) {
        case 0:
          Serial.println("0: Success");
          break;
        case 1:
          Serial.println("1: Data too long to fit in transmit buffer");
          break;
        case 2:
          Serial.println("2: Received NACK on transmit of address");
          break;
        case 3:
          Serial.println("3: Received NACK on transmit of data");
          break;
        case 4:
          Serial.println("4: Other error");
          break;
        case 5:
          Serial.println("5: Timeout");
          break;
        default:
          Serial.print(error);
          Serial.println("Unknown I2C error");
          break;
      }
    }
    return false;
  }
  return true;
}

void serialPause(){
  if ( DEBUG ){
    Serial.println("\nPress any key to continue...");
    while (Serial.available() == 0) {
      delay(100); //Prevent ESP32 watchdog triggers by yielding processor time to background routines
      digitalWrite(LED_BUILTIN, LOW);
      delay(400);
      digitalWrite(LED_BUILTIN, HIGH);
      
    }
    while (Serial.available() > 0) {
      Serial.read(); 
    }
  }
}

void printBin8(uint8_t valore) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((valore >> i) & 1);
  }
}

void RelayTest(){
  if( sequenceActive | (!OutDebug & !DEBUG) ){
    return;
  }
  // Incrementally cycle array elements during idle testing parameters
  for(int i = 0; i < relayNumber; i++) {
    if(relay[i] == HIGH) {
      relay[i] = LOW;
      if(i==relayNumber-1){
        break;  
      }else{
        relay[i+1] = HIGH;
      }
      break;
    }
  }
  // Default fallback assignment routines during initial loop
  if (isFirstRun && *(uint16_t*)relay == 0) {  
    relay[0] = HIGH;             
  }
}

void checkSystemResetReason() {
  #if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
    esp_reset_reason_t reason = esp_reset_reason();
    
    Serial.println("\n============= FREE RTOS BOOT DIAGNOSTICS =============");
    Serial.print("Last Reset Code: ");
    Serial.print(reason);
    Serial.print(" - ");

    switch (reason) {
      case ESP_RST_POWERON:   Serial.println("Power-on event / Normal Boot"); break;
      case ESP_RST_SW:        Serial.println("Software reset via esp_restart()"); break;
      case ESP_RST_PANIC:     Serial.println("SYSTEM PANIC! Core exception / Fatal Crash"); break;
      case ESP_RST_INT_WDT:   Serial.println("INTERRUPT WATCHDOG! System hung or blocked"); break;
      case ESP_RST_TASK_WDT:  Serial.println("TASK WATCHDOG! FreeRTOS loop overloaded"); break;
      case ESP_RST_WDT:       Serial.println("Other Watchdog intervention"); break;
      case ESP_RST_BROWNOUT:  Serial.println("BROWNOUT! Power supply voltage dropped"); break;
      case ESP_RST_DEEPSLEEP: Serial.println("Wakeup from Deep Sleep"); break;
      default:                Serial.println("Unknown or undetermined reset reason"); break;
    }
    Serial.println("======================================================\n");
  #else
    Serial.print("\nHello!\n");
  #endif
}

#ifdef WEB_SERVER_H
// GENERA GLI SWITCH HTML deve accedere direttamente agli array del programma principale
String generaHtmlRele() {
  String html = "";
  for(int i = 0; i < relayNumber; i++) {
    html += "<div class='toggle-container'><span>V" + String(i+1) + "</span><label class='switch'>";
    html += "<input type='checkbox' name='r" + String(i) + "' value='1'" + (relayEnableMask[i] ? " checked" : "") + ">";
    html += "<span class='slider'></span></label></div>";
  }
  return html;
}
String generaHtmlGiorni() {
  String html = "";
  for(int i = 0; i < 7; i++) {
    html += "<div class='toggle-container'><span>" + String(shortDays[i]) + "</span><label class='switch'>";
    html += "<input type='checkbox' name='d" + String(i) + "' value='1'" + (dayEnableMask[i] ? " checked" : "") + ">";
    html += "<span class='slider'></span></label></div>";
  }
  return html;
}
#endif
