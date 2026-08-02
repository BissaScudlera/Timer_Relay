# Architettura Timer_Relay

## Moduli principali

### Core
- Timer_Relay.ino
- Globals
- Version

### Hardware
- I2CManager
- RelayManager
- RTCManager

### Logica
- Scheduler
- TimeBase
- ConfigManager

### Web
- WebServer
- WebPages
- WebHandlers

## Principi
- La logica irrigazione non deve dipendere dalla UI.
- Le pagine web devono richiamare handler dedicati.
- Le configurazioni persistenti future devono essere isolate.
