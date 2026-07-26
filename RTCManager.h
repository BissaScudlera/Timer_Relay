#ifndef RTCMANAGER_H
#define RTCMANAGER_H

#include <Arduino.h>
#include <RTClib.h>
#include "DeviceStatus.h"

// Oggetti definiti nel file principale
extern RTC_DS3231 rtc;
extern DeviceStatus rtcStatus;

// Inizializzazione RTC
bool rtcInit(void);
bool rtcRecover(void);

// Aggiornamento periodico (placeholder)
bool rtcUpdate(void);

// Stato RTC
bool rtcAvailable(void);

// Restituisce l'ultima data/ora valida
DateTime rtcNow(void);

// Stringhe formattate
const char* rtcTimeString(void);
const char* rtcDateString(void);

#endif