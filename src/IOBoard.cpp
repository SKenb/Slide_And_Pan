#include "IOBoard.h"

IOBoard::IOBoard(pin_t setLatchPin,
            pin_t setClockPin,
            pin_t setdataOutPin,
            ioport_t numOutputs = 8) {

    outputState = 0;

    latchPin = setLatchPin;
    clockPin = setClockPin;
    dataOutPin = setdataOutPin;

    numberOutputPorts = numOutputs;

    setup();

    applyOutputState();
}

void IOBoard::setup() {

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataOutPin, OUTPUT);
}

void IOBoard::applyOutputState() {
    
    debugMessage("OuputState: " + String(outputState));

    digitalWrite(latchPin, LOW);

    unsigned int boardNum = ceil(numberOutputPorts/8);

    for(unsigned int index = boardNum; index > 0; index--) {
        
        uint8_t value = ((outputState >> (8*(index-1))) & 0xFF);
    
        debugMessage("Index: " + String(index-1) + "\tValue: " + String(value));
        
        shiftOut(dataOutPin, clockPin, MSBFIRST, value);
    }
    
    digitalWrite(latchPin, HIGH);
}

void IOBoard::setOutput(ioport_t port, unsigned int level) {
    if(port >= numberOutputPorts) return;
    
    unsigned int mask = (1 << port);
    if(level == HIGH) outputState |= mask;
    else  outputState &= (~mask);
}

unsigned int IOBoard::getOutputState(ioport_t port) {
    return ((outputState & (1 << port)) == 0 ? LOW : HIGH);
}

void IOBoard::toggleOutput(ioport_t port) {
    setOutput(port, getOutputState(port) == LOW ? HIGH : LOW);
}
