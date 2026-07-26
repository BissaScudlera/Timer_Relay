# Timer Relay ESP32
## Firmware Roadmap

Versione roadmap: 2.0
Stato: Attiva

Obiettivi principali

- Firmware sempre compilabile.
- Patch piccole e reversibili.
- Nessuna regressione.
- Nessun delay().
- Nessun while() bloccante.
- Solo millis().
- WebServer sempre operativo.
- Errori I²C mai bloccanti.
- Recupero automatico delle periferiche.
- Compatibilità completa Arduino IDE.

---

# FASE 1 - Hardening

## v1.1.0 - Version Manager
Stato: ✔ COMPLETATA

### Added
- Version.h
- Version.cpp
- Versione firmware
- Build date
- Build time
- Identificazione hardware

---

## v1.1.1 - Device Status
Stato: ✔ COMPLETATA

### Added
- DeviceStatus.h
- DeviceState
- DeviceStatus
- Struttura comune per tutti i dispositivi

---

## v1.1.2 - RTC Abstraction Layer
Stato: ✔ COMPLETATA

### Added
- RTCManager.h
- RTCManager.cpp

### Changed
- Accesso centralizzato al DS3231

---

## v1.1.3 - Scheduler Base
Stato: ✔ COMPLETATA

### Added
- Scheduler.h
- TaskTimer
- taskExpired()

### Note
Scheduler minimale.
Nessuna dipendenza aggiuntiva.
Nessun utilizzo obbligatorio.

---

## v1.1.4 - Repository Cleanup
Stato: ✔ COMPLETATA

### Added
- README
- ROADMAP
- CHANGELOG
- CONTRIBUTING
- TODO
- documentazione

---

## v1.1.5 - RTC Fault Tolerance
Stato: ☐

### Obiettivi

- RTC mai bloccante
- cache ultima data valida
- rtcAvailable()
- rtcUpdate() sicuro
- ora "--:--:--"
- data "--/--/----"
- nessun crash del WebServer

---

## v1.1.6 - I²C Manager
Stato: ☐

### Added

I2CManager.h

I2CManager.cpp

### Funzioni

i2cBegin()

i2cDevicePresent()

i2cLastError()

---

## v1.1.7 - I²C Fault Tolerance
Stato: ☐

### Obiettivi

Errore I²C

↓

DeviceStatus = ERROR

↓

continua il firmware

↓

nessun restart

↓

nessun blocco

---

## v1.1.8 - RTC Auto Recovery
Stato: ☐

Ogni 5 secondi

↓

controllo presenza RTC

↓

rtc.begin()

↓

RECOVERY

↓

DeviceStatus = OK

---

## v1.1.9 - WebServer Hardening
Stato: ☐

### Obiettivi

WebServer indipendente

Visualizzazione:

RTC

Errore (-4)

NaN

?

--:--:--

---

## v1.1.10 - Diagnostics
Stato: ☐

Nuovo modulo

Diagnostics

Gestione errori

Statistiche

Error counter

---

# FASE 2 - Modularizzazione

## v1.2.0
RelayManager

Spostamento gestione relè

---

## v1.2.1
ConfigManager

EEPROM

---

## v1.2.2
WebServer modularizzato

---

## v1.2.3
Globals cleanup

Riduzione variabili globali

---

## v1.2.4
Memory optimization

Riduzione String

snprintf()

buffer statici

---

# FASE 3 - Diagnostica

Pagina

/diag

Visualizza

Firmware

Heap

RAM libera

Stack

Reset reason

Uptime

WiFi RSSI

RTC

I²C

Temperature

Relay

Versione firmware

---

# FASE 4 - Robustezza

Watchdog software

Recovery WiFi

Recovery RTC

Recovery I²C

Recovery sensori

Log errori

Ring buffer

---

# FASE 5 - Nuove funzioni

OTA

MQTT

REST API

JSON

NTP

Cronologia eventi

Backup configurazione

---

# Regole di sviluppo

✔ Una patch modifica massimo 2 file esistenti.

✔ Una patch può aggiungere nuovi file.

✔ Ogni patch deve compilare.

✔ Ogni patch aggiorna CHANGELOG.

✔ Ogni patch aggiorna ROADMAP.

✔ Nessuna patch lascia codice incompleto.

✔ Nessuna funzione bloccante.

✔ Nessun delay().

✔ Nessun while() di attesa.

✔ Nessun ESP.restart() per errori periferiche.

✔ WebServer sempre operativo.

✔ Errori I²C sempre confinati al dispositivo interessato.

✔ Tutte le nuove stringhe devono usare snprintf() con buffer dimensionati.

✔ Evitare String nelle nuove funzionalità.
