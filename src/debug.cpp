#include "debug.h"

bool debug;

const int RING_BUFFER_SIZE = 20;
int ringBufferIndex = 0;
String ringBuffer[RING_BUFFER_SIZE];

void debugSetup() {
    debug = true;

    if(!debug) return;

    Serial.begin(9600);
    Serial.println("-------------");
    debugMessage("debug Setup");
}

void addMessageToBuffer(String message) {
    if(ringBufferIndex >= RING_BUFFER_SIZE) ringBufferIndex = 0;
    ringBuffer[ringBufferIndex] = message;
    ringBufferIndex++;
}

void debugMessage(String message) {
    if(!debug) return;

    Serial.println("[INF]\t" + message);
    addMessageToBuffer("[INF]\t" + message);
}

void debugError(String errorMessage) {
    if(!debug) return;

    Serial.println("[ERR]\t" + errorMessage);
    addMessageToBuffer("[ERR]\t" + errorMessage);
}

String getRingBufferForWeb() {
    String builder = "";
    
    for(int index = 1; index <= RING_BUFFER_SIZE; index++) {
        int modIndex = ringBufferIndex - index;
        builder += ringBuffer[((modIndex >= 0) ? modIndex : RING_BUFFER_SIZE + modIndex)] + "<br>";
    }

    return builder;
}