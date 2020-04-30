#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

void debugSetup();
void debugMessage(String message);
void debugError(String errorMessage);

#endif