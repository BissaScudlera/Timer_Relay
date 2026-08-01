#ifndef RTCMANAGER_H
#define RTCMANAGER_H

#include <Arduino.h>
#include <RTClib.h>
#include "DeviceStatus.h"

//variabili globali
extern const char daysOfTheWeek[7][12];
extern const char* shortDays[7];

// Oggetti definiti nel file principale
extern RTC_DS3231 rtc;
extern DeviceStatus rtcStatus;

// Inizializzazione RTC
bool rtcInit(void);
bool rtcRecover(void);

// Aggiornamento periodico (placeholder)
bool rtcUpdate(void);

// Restituisce l'ultima data/ora valida
DateTime rtcNow(void);

// Stringhe formattate
const char* rtcTimeString(void);
const char* rtcDateString(void);
const char* rtcDayString(void);

const char* formatCountTime(uint32_t totalSeconds); // stringa formattata a partire da un valore in secondi

bool rtcSetTime(uint8_t hour, uint8_t minute);
float rtcTemperature(void);
bool rtcSetDateTime(const DateTime& dt);

const DeviceStatus& rtcGetStatus(void);


#endif
