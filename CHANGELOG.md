## [1.2.9] - 2026-07-26

### Changed
- Planned integration of Scheduler into the main loop.
- Reserved patch identifier for scheduler migration.
- No functional changes introduced in this patch.



## [1.2.8] - 2026-07-26

### Changed
- Completed Version module public API.
- Prepared firmware for WebServer decoupling.

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

## [1.1.3] - 2026-07-26

### Added
- Cache centralizzata della data/ora (`cachedNow`).

### Changed
- Tutte le letture del RTC passano attraverso `rtcUpdate()` e `rtcNow()`.

### Fixed
- Eliminata la possibilità di letture multiple incoerenti del DS3231 nello stesso ciclo.

