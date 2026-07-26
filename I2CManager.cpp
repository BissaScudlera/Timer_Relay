#include "I2CManager.h"

static uint8_t lastError = 0;

bool i2cBegin()
{
    Wire.begin(21, 22);
    lastError = 0;
    i2cStatus.setOk();
    return true;
}

bool i2cDevicePresent(uint8_t address)
{
    Wire.beginTransmission(address);
    lastError = Wire.endTransmission();

    if (lastError == 0)
        i2cStatus.setOk();
    else
        i2cStatus.setError(lastError);

    return lastError == 0;
}

uint8_t i2cLastError()
{
    return lastError;
}

const DeviceStatus& i2cGetStatus()
{
    return i2cStatus;
}
