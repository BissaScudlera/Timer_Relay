#ifndef I2CMANAGER_H
#define I2CMANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "DeviceStatus.h"

extern DeviceStatus i2cStatus;

bool i2cBegin();
bool i2cDevicePresent(uint8_t address);
uint8_t i2cLastError();

const DeviceStatus& i2cGetStatus();

#endif
