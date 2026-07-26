#include "Version.h"
#include <stdio.h>

static char versionBuffer[16];

const char* getFirmwareVersion()
{
    snprintf(versionBuffer,sizeof(versionBuffer),"%d.%d.%d",FW_MAJOR,FW_MINOR,FW_PATCH);
    return versionBuffer;
}

const char* getBuildDate()
{
    return FW_BUILD_DATE " " FW_BUILD_TIME;
}

const char* getHardwareName()
{
    return HW_NAME;
}
