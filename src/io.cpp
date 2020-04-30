#include "io.h"
#include "stepper.h"

IOBoard* ioBoard;

int32_t encoderRotation = 0; 
unsigned int dir = 0;
unsigned int prevClk = 0;

void setupIOBoard() {
  ioBoard = new IOBoard(PIN_LATCH, PIN_CLOCK, PIN_DATAOUT, 16,
                        PIN_LATCHIN, PIN_DATAIN, 8);

  ioBoard->enableAutoRead(1);
  ioBoard->addOnPortChangeHandler(4, [](ioport_level_t level){ debugMessage("Toggled switch: " + String(level) + "\tRotation: " + String(encoderRotation)); });
  ioBoard->addOnChangeHandler([ioBoard](unsigned int changed_values){ 

    ioport_level_t direction = ioBoard->getInputState(ENCODER_PIN_CLK);
    ioport_level_t clk = ioBoard->getInputState(ENCODER_PIN_DIRECTION);

    if(clk && !prevClk) encoderRotation += direction ? -1 : 1;
    if(!clk && prevClk) encoderRotation += direction ? 1 : -1;
    
    prevClk = clk;
  });

}

IOBoard* getIOBoard() { return ioBoard; }

int32_t getEncoderRotation() { return encoderRotation; }

String getIOJsonData(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Encoder", getEncoderJsonData(myTabString), myTabString, false);
    return json + tabString + "}";
}

String getEncoderJsonData(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Rotation", String(getEncoderRotation()), myTabString, false);
    jsonAddField(json, "DirectionPin", String(ENCODER_PIN_DIRECTION), myTabString, true);
    jsonAddField(json, "ClockPin", String(ENCODER_PIN_CLK), myTabString, true);

    return json + tabString +"}";
}