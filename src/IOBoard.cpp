#include "IOBoard.h"

String byteToBinaryString(uint8_t word) {
    String result = "";

    for(uint8_t index = 128; index > 0; index/=2) result += (word & index) ? "1" : "0";

    return result;
}

IOBoard::IOBoard(   pin_t setLatchPin,
                    pin_t setClockPin,
                    pin_t setDataOutPin,
                    ioport_t numOutputs,
                    pin_t setLatchInPin,
                    pin_t setDataInPin,
                    ioport_t numInputs) {

    outputState = 0;

    latchPin = setLatchPin;
    clockPin = setClockPin;
    dataOutPin = setDataOutPin;

    dataInPin = setDataInPin;
    latchInPin = setLatchInPin;

    numberOutputPorts = numOutputs;
    numberInputPorts = numInputs;

    setup();

    applyOutputState();
}

void IOBoard::setup() {

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataOutPin, OUTPUT);

    pinMode(dataInPin, INPUT);
    pinMode(latchInPin, OUTPUT);
}

void IOBoard::applyOutputState() {
    
    digitalWrite(latchPin, LOW);

    unsigned int boardNum = ceil(numberOutputPorts/8);

    for(unsigned int index = boardNum; index > 0; index--) {
        uint8_t value = ((outputState >> (8*(index-1))) & 0xFF);        
        shiftOut(dataOutPin, clockPin, MSBFIRST, value);
    }
    
    digitalWrite(latchPin, HIGH);
}

void IOBoard::setOutput(ioport_t port, ioport_level_t level) {
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

void IOBoard::readInput() {

    digitalWrite(latchInPin, HIGH);
    digitalWrite(latchInPin, LOW);
    digitalWrite(latchInPin, HIGH);


    unsigned int boardNum = ceil(numberInputPorts/8);
    
    unsigned int previousInputState = inputState;
    inputState = 0;
    
    for(unsigned int index = boardNum; index > 0; index--) {
        uint8_t value = shiftIn(dataInPin, clockPin, MSBFIRST);
        inputState += (value << (8*(index - 1)));
    }

    if(previousInputState != inputState) {
        unsigned int changedInputs = previousInputState ^ inputState;

        for(std::function<void (unsigned int)> onChangeHandler : onChangeHandlers) {
            onChangeHandler(changedInputs);
        }
    }    
}

ioport_level_t IOBoard::readAndGetInput(ioport_t port) {
    readInput();
    return getInputState(port);
}

void IOBoard::enableAutoRead(unsigned int rate) {
    readTicker.detach();
    readTicker.attach_ms(rate, [this](){ this->readInput(); } );
}

void IOBoard::addOnPortChangeHandler(ioport_t port, std::function<void (ioport_level_t)> handler) {
    addOnChangeHandler([this, port, handler](unsigned int changedValues){ 
        if(changedValues & (1 << port)) handler((this->getInputState(port))); 
    });
}