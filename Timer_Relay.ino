
/*********************************************
 Relay Timer with ESP32 web interface
 Written by Jack and Gemini
 Refactored byChatGPT
**********************************************/

#include <Arduino.h>
#include "Version.h"
#include "ConfigManager.h"  //SETUP PARAMETERS
#include "TimeBase.h"
#include "Scheduler.h"
#include "Globals.h"
#include "DeviceStatus.h"
#include "RTCManager.h"
#include "I2CManager.h"
#include "RelayManager.h"
#include "HttpServer.h"

// Architecture-specific I/O and communication settings----------------------------------
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
  // ESP32 DevKit V1 configuration
  const unsigned long SERIAL_BAUD = 115200; // Recommended baud rate for ESP32: 115200
  
  const int iTriggerButton = 4;  // PIN D4:  Manual Sequence Start, debounced push button
  const int pTriggerButton = 7;  // MCP2.PortA.p7: alternate Manual Sequence Start
  
  const int iResetButton   = 5;  // PIN D5:  Manual Sequence Stop, long press to toggle wifi
  const int pResetButton   = 6;  // MCP2.PortA.p6: alternate Manual Sequence Stop
  
  const int iFactoryReset  = 18; // PIN D18: stops loading parameters from NVM and overrides them with Source code defaults
  const int iDBG_SETUP     = 19; // PIN D19: pauses the program after setup(), waits for serial input.
  
  const int oLedDebug      = LED_BUILTIN; // Uses ESP32 native built-in LED (GPIO 2)
#endif
//---------------------------------------------------------------------------------------

// Debug and initialization flags
bool isFirstRun = true;
int ErrState = 0;
int MsgNum;

// Subroutine execution timers
uint32_t currentMillis = 0;  
uint32_t previousMillis;

#include <Wire.h> //I2C library

// DS3231 RTC configurations
#include "RTClib.h"
//const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//const char* shortDays[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

// MCP23017 I2C Port Expander configurations
bool MCP1_ON = false; //used in read inputs to avoid generating too much error messages
bool MCP2_ON = false;

// Hardware input debounce filters
bool ManualStart;
bool lastTrigState = HIGH;              // Default idle high due to internal pull-up
unsigned long lastDebounceTime = 0;      // Transient timer snapshot
const unsigned long debounceDelay = 50;  // Settling threshold interval

bool ManualStop;
unsigned long t_iReset = 0;                  // Stores the millisecond timestamp when pressing starts
bool lastResetStatus = false;                // Tracking flag for the button state
const unsigned long T_iResetLong = 5000;     // Threshold for long press action (3 seconds)
const unsigned long T_iResetShort = 50;      // Minimum debounce threshold for short press action (50ms)


// Forward function declarations------------------------------------------------------------------

void runTimedSequence();
void readInputs();
void checkManualTrigger(int value);
void checkResetButton(int value);
bool DeviceAlive(byte Address, const char* Name);
//void RelayTest();
void printBin8();
uint8_t reverseByte();

#if DEBUG
void SerialMonitor(); 
void checkSystemResetReason();
void serialPause();
#endif
#ifdef WEB_SERVER_H
  String generaHtmlRele();
  String generaHtmlGiorni();
#endif
//------------------------------------------------------------------------------------------------

void setup() {
  // Initialize hardware digital I/O channels
  pinMode(oLedDebug, OUTPUT);
  pinMode(iTriggerButton, INPUT_PULLUP);
  pinMode(iResetButton, INPUT_PULLUP);
  pinMode(iFactoryReset, INPUT_PULLUP);
  pinMode(iDBG_SETUP, INPUT_PULLUP);

  checkJumpers();

  // Clear memory registers for all I/O elements
  memset(relay, LOW, sizeof(relay));
  BankD=0;
  ManualStart= LOW;
  ManualStop= LOW;

  // Initialize hardware serial console
  #if DEBUG
    DBG_BEGIN(SERIAL_BAUD);
    // Hold execution until terminal connects
   uint32_t serialStart = millis();
    while (!Serial && (millis() - serialStart < 2000)) {
    yield();
    }
    //Serial.println("\n\r\033[2J\033[H");  // Clear screen terminal escape sequence
	
    checkSystemResetReason();
	
    DBG_PRINT("[CFG] Debug flag: ");
    DBG_PRINTLN(DEBUG);
    DBG_PRINT("[CFG] RELAY_NUMBER: ");
    DBG_PRINTLN(RELAY_NUMBER);
    DBG_PRINT("[CFG] MCP1_ENABLE: ");
    DBG_PRINTLN(MCP1_ENABLE);
    DBG_PRINT("[CFG] MCP2_ENABLE: ");
    DBG_PRINTLN(MCP2_ENABLE);
    DBG_PRINT("[CFG] Restore Defaults: ");
    DBG_PRINTLN(FactoryReset);
    DBG_PRINT("[CFG] Setup serial pause: ");
    DBG_PRINTLN(DBG_SETUP);
	DBG_PRINT("\n\r\n\r");
  #endif

  // Conditional I2C interface hardware setup
    i2cBegin();  // Assign physical hardware pins for ESP32 I2C bus
    #define _ESP32_BUS

  // Configure MCP23017 GPIO data direction registers
  if (MCP1_ENABLE && DeviceAlive(MCP1_ADR,"Relay Interface 1")){
	MCP1_ON = true;
    mcp1.writeRegister(MCP23017Register::OLAT_A, 0xFF);  // Purge/Reset internal latch register A, prevents initialization glitch
    mcp1.writeRegister(MCP23017Register::OLAT_B, 0xFF);  // Purge/Reset internal latch register B, prevents initialization glitch
    mcp1.portMode(MCP23017Port::A, 0); // Define Port A banks as digital output channels
    mcp1.portMode(MCP23017Port::B, 0b11111111); // Define Port A banks as digital input channels
	//input bank options
	mcp1.writeRegister(MCP23017Register::GPPU_B, 0xFF);   //Internal pull-up enabled on Port A
    //mcp1.writeRegister(MCP23017Register::IPOL_B, 0x00);   //Same logic as the input pins state
    mcp1.writeRegister(MCP23017Register::IPOL_B, 0xFF);  // Uncomment this line to invert inputs
  }
  if (MCP2_ENABLE && DeviceAlive(MCP2_ADR,"Relay Interface 2")){
	MCP2_ON = true;
    mcp2.writeRegister(MCP23017Register::OLAT_A, 0xFF);  // Purge/Reset internal latch register A, prevents initialization glitch
    mcp2.writeRegister(MCP23017Register::OLAT_B, 0xFF);  // Purge/Reset internal latch register B, prevents initialization glitch
    mcp2.portMode(MCP23017Port::A, 0); // Define Port A banks as digital output channels
    mcp2.portMode(MCP23017Port::B, 0); // Define Port B banks as digital output channels
  }
  
  // Validate real-time clock operational status
  rtcInit();
  if (!rtcGetStatus().available) {
    DeviceAlive(0x68, "RTC");
    DBG_PRINTLN("[RTC] Setup failed");
  }
  else{
    if (rtc.lostPower()){
      ErrState++;
      DBG_PRINTLN("[RTC] Clock lost power, set the time!");
    }
    // Deactivate hardware auxiliary square wave generation channels
    rtc.disable32K();
    rtc.disableAlarm(1);
    rtc.disableAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);
  }
  
  // Initialize timer configuration
  if (FactoryReset)
  {
      DBG_PRINTLN("[CFG] Factory Reset: using default Timer settings");
      saveConfigTimer();      // Save defaults from source code
  }
  else
  {
      if (!loadConfigTimer())
      {
          DBG_PRINTLN("[CFG] Invalid Timer configuration, restoring defaults");
          saveConfigTimer();
      }
  }
  
  
  #ifdef WEB_SERVER_H
    // Initialize WiFi configuration
    if (FactoryReset)
    {
        DBG_PRINTLN("[CFG] Factory Reset: using default WiFi settings");
    
        strlcpy(configWifi.ssid, AP_SSID, sizeof(configWifi.ssid));
        strlcpy(configWifi.pswd, AP_PWD, sizeof(configWifi.pswd));
    
        saveConfigWifi();
    }
    else
    {
        if (!loadConfigWifi())
        {
            DBG_PRINTLN("[CFG] Invalid WiFi configuration, restoring defaults");
    
            strlcpy(configWifi.ssid, AP_SSID, sizeof(configWifi.ssid));
            strlcpy(configWifi.pswd, AP_PWD, sizeof(configWifi.pswd));
    
            saveConfigWifi();
        }
    }
    serverSetup();
  #endif

  DBG_PRINT("\r\n\[SYS] Setup errors: ");
  DBG_PRINTLN(ErrState);
  #if DEBUG
  if (DBG_SETUP){
    ErrState = 0;
    serialPause();
  }
  #endif
  DBG_PRINT("\033[92m"); //Terminal text color
  DBG_PRINT("\033[44m"); //Terminal background color
}
void loop() {
  currentMillis = millis();
  yield();; //Prevent ESP32 watchdog triggers by yielding processor time to background routines
  
  #ifdef WEB_SERVER_H
    serverLoop();
  #endif
  
  
	//checkJumpers();
  readInputs();
  //checkManualTrigger();
  //checkResetButton();

  // Primary execution block triggered exactly once per second
  if (currentMillis - previousMillis >= 1000) {
    /*
	if (!rtcGetStatus().available)
    {
      if (taskExpired(rtcRecoveryTask))
      {
        rtcRecover();
      }
    }
	*/ 
    rtcUpdate();
    previousMillis = currentMillis;
    
    // Handle I2C peripheral hardware bus errors (AVR architecture only)
    #if !defined(_ESP32_BUS)
      if (Wire.getWireTimeoutFlag()) {
        Wire.clearWireTimeoutFlag();
        ErrState++;
        DBG_PRINTLN("[I2C] Bus Timeout");
      }
    #endif

    // Query active clock timestamp from hardware rtc element
    if ( rtcGetStatus().available && DeviceAlive(0x68, "RTC") ){
      now = rtcNow();
    }

    // Evaluate automated sequence pipeline cycles
    runTimedSequence();

    // Map linear state array into 8-bit registers for I2C transmissions
    BankA = 0;
    BankB = 0;
    BankC = 0;
    for (uint8_t i = 0; i < RELAY_NUMBER; i++)
    {
        if (i < 8)
        {
            BankA |= (relay[i] << i);
        }
        else if (i < 16)
        {
            BankB |= (relay[i] << (i - 8));
        }
        else if (i < 24)
        {
            BankC |= (relay[i] << (i - 16));
        }
    }
	//invert logic and mirror MCP1-portA (it's upside down on the PCB)
	BankA= reverseByte(~BankA);
	BankB= reverseByte(~BankB);
	BankC= ~BankC;

    // Write Digital Outputs
    if (MCP1_ENABLE && DeviceAlive(MCP1_ADR,"Relay Interface 1")){
	  MCP1_ON = true;
      mcp1.writePort(MCP23017Port::A, BankA);
      //mcp1.writePort(MCP23017Port::B, BankD);
    }
	else 
		MCP1_ON = false;
	if (MCP2_ENABLE && DeviceAlive(MCP2_ADR,"Relay Interface 2")){
	  MCP2_ON = true;
      mcp2.writePort(MCP23017Port::B, BankB);
      mcp2.writePort(MCP23017Port::A, BankC);
    }
	else 
		MCP2_ON = false;
	
    if (ErrState != 0){
      digitalWrite(oLedDebug, HIGH);
    }
	delay(1);
    
    // Process diagnostics outputs
    #if DEBUG	
    SerialMonitor(); 
	#endif
    isFirstRun = false;
    ErrState = 0;
  }
}

void checkJumpers(){
  // Evaluate status configurations on startup, from hardware strapping pins
  FactoryReset= !digitalRead(iFactoryReset);
  DBG_SETUP= !digitalRead(iDBG_SETUP);
}

inline bool bufferRead(uint8_t bit)
{
    return (BankD & (1 << bit));
}

void readInputs(){
	
	if (MCP1_ENABLE && MCP1_ON)
        BankD = mcp1.readPort(MCP23017Port::B);  //Read I2C digital inputs
	else
		BankD = 0x00;
  
	ManualStart= ( !digitalRead(iTriggerButton) || bufferRead(pTriggerButton) );
	checkManualTrigger( ManualStart );
	
    ManualStop= ( !digitalRead(iResetButton)  || bufferRead(pResetButton) );
	checkResetButton( ManualStop );
   }

void checkManualTrigger(bool reading) {
	
  if (reading != lastTrigState) {
    lastDebounceTime = millis();
    lastTrigState = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Assert active sequence status if edge requirements evaluate true
    if (reading == HIGH && !relayRunning()) {
      relayStart(); 
    }
  }
}

void checkResetButton(bool actStatus) {
  static bool longPressHandled = false;

  if (actStatus == HIGH) {
    if (!lastResetStatus) {
      t_iReset = millis();
      lastResetStatus = true;
      longPressHandled = false;
    }

    if (!longPressHandled && (millis() - t_iReset >= T_iResetLong)) {
	  DBG_PRINTLN("[I/O] Long Press: Toggle WebServer.");
      longPressHandled = true;
      lastResetStatus = false;
      #if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
        toggleWebServer();
      #endif
    }
  }
  else {
    if (lastResetStatus) {
      unsigned long duration = millis() - t_iReset;
      lastResetStatus = false;

      if (duration < T_iResetLong && duration > T_iResetShort) {
        if (relayRunning()) {
          relayStop();
          for (int i = 0; i < RELAY_NUMBER; i++) {
            relay[i] = LOW;
          }
          DBG_PRINTLN("[I/O] Short Press: Relay sequence RESET.");
        }
      }
    }

    longPressHandled = false;
  }
}


#if DEBUG
void SerialMonitor(){
  if(!DEBUG){ return; }
  
  Serial.println("\n\r\033[2J\033[H");  // Clear screen terminal escape sequence
  Serial.print("Cycle N°");
  Serial.print(MsgNum);
  Serial.print(" ---------------------------\n\r");

  Serial.print("\n\rSystem Errors: ");
  Serial.println(ErrState);

  // Format and print current hardware RTC timestamps
  if (DeviceAlive(0x68, "RTC")) {
    Serial.print("\n\rTemperature: ");
    Serial.print(rtc.getTemperature());
    Serial.print(" C");
    Serial.print("\n\r");
    Serial.print("\n\rRTC Date : ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print("\n\rRTC Time : ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print("\n\r");
  }

  // Format scheduling metrics
  Serial.print("\n\rTarget Time: [");
  if(configTimer.startHour < 10) Serial.print('0'); Serial.print(configTimer.startHour); Serial.print(':');
  if(configTimer.startMinute < 10) Serial.print('0'); Serial.print(configTimer.startMinute); Serial.print(':');
  if(configTimer.startSecond < 10) Serial.print('0'); Serial.print(configTimer.startSecond);
  Serial.println("]");
  
  Serial.print("Enabled Days: ["); /*------------------------------------------------------------*/
  for(int i = 0; i < 7; i++) {
    if (configTimer.dayEnableMask[i]) {
      Serial.print(shortDays[i]); 
    } else {
      Serial.print("--");         
    }
    if(i < 6) Serial.print(' '); 
  }
  Serial.println("]");

  Serial.print("Enabled Relays: ["); /*------------------------------------------------------------*/
  for(int i = 0; i < RELAY_NUMBER; i++) { 
    Serial.print(configTimer.relayEnableMask[i] ? '1' : '0');  
    if (((i + 1) % 8 == 0) && (i + 1 < RELAY_NUMBER)){
        Serial.print(' ');   // Space after every bank of 8, except after the last one
        }
	}
  Serial.println("]"); 

  // Update pipeline state parameters --------------------------------------------------------------*/
  Serial.print("\n\rSequence Status : ");
  Serial.print(relayStateString());
  if (relayRunning()) {
    Serial.print(" | Current Relay Index: ");
    Serial.print(currentRelayIndex + 1); 
    Serial.print("/");
    Serial.print(RELAY_NUMBER);
    Serial.print(" | Time Remaining: ");
    Serial.print(relayRemainingSeconds());
    Serial.println("s");
  } else {
    Serial.println(" (Waiting for trigger)");
  }
  // ------------------------------------------------------------------------------------------------

  // Output current software matrix state parameters
  Serial.print("Output SW: [");
  for (int i = 0; i < RELAY_NUMBER; i++) {
    Serial.print(relay[i] ? '1' : '0');  
    if (((i + 1) % 8 == 0) && (i + 1 < RELAY_NUMBER)){
        Serial.print(' ');   // Space after every bank of 8, except after the last one
        }    
  }
  Serial.println("]");
  
  // Output literal internal peripheral latched configuration profiles
  Serial.print("Output HW: ");
  Serial.print('[');
  printBin8(reverseByte(BankA));
  Serial.print(' ');
  printBin8(BankB);
  Serial.print(' ');
  printBin8(reverseByte(BankC));
/*
  if(MCP1_ENABLE && DeviceAlive(MCP1_ADR,"Interface 1")){
	Serial.print("[");
    printBin8(mcp1.readPort(MCP23017Port::A));
    Serial.print(' ');
    printBin8(mcp1.readPort(MCP23017Port::B));
  }
  if(MCP2_ENABLE && DeviceAlive(MCP2_ADR,"Interface 2")){
    Serial.print(' ');
    printBin8(mcp2.readPort(MCP23017Port::B));
  }
*/
  Serial.println("]");

  Serial.print("Input HW: [");
  printBin8(BankD);
  /*
  if(MCP2_ENABLE && DeviceAlive(MCP2_ADR,"Interface 2")){
    printBin8(mcp2.readPort(MCP23017Port::A));       
  }
  */
  Serial.println("]");
  
  // various inputs debug output, to be removed
  Serial.print("Misc Inputs: ");
  Serial.print("Factory Reset(");
  Serial.print(FactoryReset);
  Serial.print(") PowerOn Pause(");
  Serial.print(DBG_SETUP);
  Serial.print(") Start(");
  Serial.print(ManualStart);
  Serial.print(") Stop (");
  Serial.print(ManualStop);
  Serial.println(")");
	

  MsgNum++;
  return;
}

void serialPause(){
  if ( DEBUG ){
    Serial.println("\n\rPress any key to continue...");
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

#endif

bool DeviceAlive( byte Address, const char* Name ){
  byte error=0;
  Wire.beginTransmission(Address);
  error = Wire.endTransmission();
  if (error != 0) {
    ErrState++;
    #if DEBUG
      Serial.print("[I2C] Couldn't find ");
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
    #endif
    return false;
  }
  return true;
}

void printBin8(uint8_t valore) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((valore >> i) & 1);
  }
}

uint8_t reverseByte(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

#if DEBUG
void checkSystemResetReason() {
  #if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
    esp_reset_reason_t reason = esp_reset_reason();
    
    Serial.println("\n\r============= FREE RTOS BOOT DIAGNOSTICS =============");
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
    Serial.println("======================================================\n\r");
  #else
    Serial.print("\n\rHello!\n\r");
  #endif
}
#endif
#ifdef WEB_SERVER_H
// GENERA GLI SWITCH HTML deve accedere direttamente agli array del programma principale
String generaHtmlRele() {
  String html = "";
  for(int i = 0; i < RELAY_NUMBER; i++) {
    html += "<div class='toggle-container'><span>V" + String(i+1) + "</span><label class='switch'>";
    html += "<input type='checkbox' name='r" + String(i) + "' value='1'" + (configTimer.relayEnableMask[i] ? " checked" : "") + ">";
    html += "<span class='slider'></span></label></div>";
  }
  return html;
}
String generaHtmlGiorni() {
  String html = "";
  for(int i = 0; i < 7; i++) {
    html += "<div class='toggle-container'><span>" + String(shortDays[i]) + "</span><label class='switch'>";
    html += "<input type='checkbox' name='d" + String(i) + "' value='1'" + (configTimer.dayEnableMask[i] ? " checked" : "") + ">";
    html += "<span class='slider'></span></label></div>";
  }
  return html;
}
#endif
