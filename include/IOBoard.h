#ifndef IOBoard_H
#define IOBoard_H

#include <Arduino.h>
#include <Ticker.h>
#include "common.h"
#include "debug.h"

typedef unsigned int ioport_t;
typedef unsigned int ioport_level_t;

class IOBoard {

    private:

    pin_t latchPin;
    pin_t clockPin;
    pin_t dataOutPin;
    pin_t latchInPin;
    pin_t dataInPin;

    ioport_t numberOutputPorts;
    ioport_t numberInputPorts;
    unsigned int outputState;
    unsigned int inputState;

    Ticker readTicker;
    std::vector<std::function<void (unsigned int)>> onChangeHandlers;

    void setup();

    void readInput();

    public:

    IOBoard(pin_t setLatchPin,
            pin_t setClockPin,
            pin_t setDataOutPin,
            ioport_t numOutputs,
            pin_t setLatchInPin,
            pin_t setDataInPin,
            ioport_t numInputs);

    void applyOutputState();

    void setOutput(ioport_t port, ioport_level_t level);
    unsigned int getOutputState(ioport_t port);
    void toggleOutput(ioport_t port);

    void setAndApplyOutput(ioport_t port, ioport_level_t level) {
        setOutput(port, level);
        applyOutputState();
    };

    void toggleAndApplyOutput(ioport_t port) {
        toggleOutput(port);
        applyOutputState();
    }

    ioport_level_t readAndGetInput(ioport_t port);
    ioport_level_t getInputState(ioport_t port) { return (inputState & (1 << port)) ? HIGH : LOW; }

    void enableAutoRead(unsigned int rate);
    void disableAutoRead() { readTicker.detach(); };
    void addOnChangeHandler(std::function<void (unsigned int)> handler) { onChangeHandlers.push_back(handler); } 
    void clearAllOnChangeHandler(std::function<void (unsigned int)> handler) { onChangeHandlers.clear(); } 
    void addOnPortChangeHandler(ioport_t port, std::function<void (ioport_level_t)> handler);
};

#endif