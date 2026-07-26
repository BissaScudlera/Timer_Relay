# Timer Relay ESP32
# Architecture Roadmap

Versione: 1.0

## Obiettivo

Evolvere il firmware verso un'architettura modulare,
mantenendo il firmware sempre compilabile
e senza regressioni funzionali.

---

# Stato iniziale

Architettura:

Timer_Relay.ino
│
├── RTCManager
├── I2CManager
├── Scheduler
├── WebServer
├── Diagnostics
├── Version
└── Globals

---

# Regole

✔ massimo 2 file esistenti modificati per patch

✔ nuovi file ammessi

✔ ogni patch compilabile

✔ aggiornare CHANGELOG

✔ aggiornare ROADMAP

✔ nessun delay()

✔ nessun while() bloccante

✔ nessuna regressione

---

# Refactoring

## STEP A001

Analisi completa
✔ completata

---

## STEP A002

Centralizzazione Scheduler

Stato:
TODO

Obiettivo

eliminare

if(currentMillis-previousMillis)

e utilizzare TaskTimer.

---

## STEP A003

Separazione WebServer

Stato

TODO

Nuovi file

WebServer.cpp

WebPages.cpp

WebHandlers.cpp

---

## STEP A004

RelayManager

Nuovi file

RelayManager.h

RelayManager.cpp

Spostare

runTimedSequence()

---

## STEP A005

Globals cleanup

Ridurre extern.

---

## STEP A006

ConfigManager

Creazione layer configurazione.

---

## STEP A007

Memory optimization

Ridurre String

buffer statici

snprintf()

---

## STEP A008

Dependency cleanup

Riduzione accoppiamento.

---

## STEP A009

Firmware freeze

Refactoring concluso.