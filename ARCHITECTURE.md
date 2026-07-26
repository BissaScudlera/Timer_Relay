# Firmware Architecture

## Livelli

Application

↓

Services

↓

Drivers

↓

Hardware

---

# Diagramma

Timer_Relay.ino

├── Scheduler

│   └── TimeBase

│

├── RTCManager

│   └── I2CManager

│       └── Wire

│

├── WebServer

│

├── Diagnostics

│

└── Version

---

# Dipendenze

Timer_Relay.ino

usa

Scheduler

RTCManager

WebServer

Version

Diagnostics

---

RTCManager

usa

RTClib

I2CManager

DeviceStatus

---

I2CManager

usa

Wire

---

Scheduler

usa

TimeBase

---

WebServer

dipende da

RTCManager

Timer_Relay.ino

relay[]

sequenceActive

generaHtmlRele()

generaHtmlGiorni()

---

Version

indipendente

---

Diagnostics

usa

RTCManager

I2CManager

---

DeviceStatus

indipendente

---

# Accoppiamento

Version            ★☆☆☆☆

DeviceStatus       ★☆☆☆☆

TimeBase           ★☆☆☆☆

I2CManager         ★☆☆☆☆

Scheduler          ★★☆☆☆

RTCManager         ★★★☆☆

Diagnostics        ★★★☆☆

WebServer          ★★★★★

Timer_Relay.ino    ★★★★★

---

# Obiettivo finale

Il file Timer_Relay.ino dovrà contenere solo

setup()

loop()

inizializzazione moduli

scheduler

senza logica applicativa.