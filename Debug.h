#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG

    #define DBG_BEGIN(baud)      Serial.begin(baud)
    #define DBG_PRINT(x)         Serial.print(x)
    #define DBG_PRINTLN(x)       Serial.println(x)
    #define DBG_PRINTF(...)      Serial.printf(__VA_ARGS__)

#else

    #define DBG_BEGIN(baud)
    #define DBG_PRINT(x)
    #define DBG_PRINTLN(x)
    #define DBG_PRINTF(...)

#endif

#endif