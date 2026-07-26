# Changelog

Tutte le modifiche rilevanti al firmware sono documentate in questo file.

Il progetto segue una roadmap incrementale:
- patch piccole
- sempre compilabili
- sempre testabili

---

## [1.1.2] - 2026-07-26

### Added
- Nuovo modulo `RTCManager`
- Funzioni di astrazione RTC
- Gestione centralizzata dell'accesso al DS3231

### Changed
- L'accesso al RTC passa attraverso `rtcNow()`
- Preparazione del firmware alla gestione fault tolerant

---

## [1.1.1] - 2026-07-26

### Added
- `DeviceStatus.h`
- `DeviceState`
- `DeviceStatus`

---

## [1.1.0] - 2026-07-26

### Added
- `Version.h`
- Versioning firmware
- Informazioni build

