#ifndef IOBoard_H
#define IOBoard_H

#include <Arduino.h>
#include "common.h"
#include "debug.h"

typedef unsigned int ioport_t;

class IOBoard {

    private:

    pin_t latchPin;
    pin_t clockPin;
    pin_t dataOutPin;

    ioport_t numberOutputPorts;
    unsigned int outputState;

    void setup();


    public:

    IOBoard(pin_t setLatchPin,
            pin_t setClockPin,
            pin_t setDataPin,
            ioport_t numOutputs);

    void applyOutputState();

    void setOutput(ioport_t port, unsigned int level);
    unsigned int getOutputState(ioport_t port);
    void toggleOutput(ioport_t port);

    void setAndApplyOutput(ioport_t port, unsigned int level) {
        setOutput(port, level);
        applyOutputState();
    };

    void toggleAndApplyOutput(ioport_t port) {
        toggleOutput(port);
        applyOutputState();
    }

};

#endif