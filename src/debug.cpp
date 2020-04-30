#include "debug.h"

bool debug;

void debugSetup() {
    debug = true;

    if(!debug) return;

    Serial.begin(9600);
    Serial.println("-------------");
    debugMessage("debug Setup");
}

void debugMessage(String message) {
    if(!debug) return;

    Serial.println("[INF]\t" + message);
}

void debugError(String errorMessage) {
    if(!debug) return;

    Serial.println("[ERR]\t" + errorMessage);
}