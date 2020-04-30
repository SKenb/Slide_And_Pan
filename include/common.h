/* I/O */
// Die möglichen Pin-Nummern.
// D1 = 5     D2 = 4     D3 = 0    D4 = 2
// TX = 1     RX = 3     D0 = 16   D5 = 14
// D6 = 12    D7 = 13    D8 = 15
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

#define DISABLE     HIGH
#define ENABLE      LOW

#define LONG_MAX 2147483647L
#define LONG_MIN -2147483648L

#define WD1 5
#define WD2 4
#define WD3 0
#define WD4 2
#define WTX 1
#define WRX 3
#define WD0 16
#define WD5 14
#define WD6 12
#define WD7 13
#define WD8 15

#define SLIDEANDPAN_NAME "Slide & Pan"
#define SLIDEANDPAN_VERSION "1.0.0.0"
#define SLIDEANDPAN_COPYRIGHT "Copyright (c) Sebastian Knoll 2020"
#define SLIDEANDPAN_ID "S&P0001"

typedef unsigned int pin_t;

void jsonAddField(String& json, String field, String value, String tabString, boolean wrapValue = false, boolean lastElement = false);

String getBasicDataJson(String tabString);

#endif
