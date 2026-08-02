#ifndef WEB_HANDLERS_H
#define WEB_HANDLERS_H

#ifdef ARDUINO_ARCH_ESP32

void gestisciStart();
void gestisciStop();
void gestisciSalvaDurata();
void gestisciSalvaOra();
void gestisciSalvaCfg();
void gestisciSalvaCfgEEPROM();

void gestisciSalvaRTC();
void gestisciSalvaWiFi();
void gestisciSalvaWiFiEEPROM();

#endif

#endif
