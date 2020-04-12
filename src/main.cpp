#include <Arduino.h>

#include "common.h"
#include "wifi.h"
#include "stepper.h"
#include "SmartStepper.h"
#include "debug.h"
#include "IOBoard.h"

void timer0_ISR (void);

pin_t latchPin = WD5; //WD0;
pin_t clockPin = WD6;
pin_t dataOutPin = WD0; //WD7;

pin_t dataInPin = WD7;
pin_t latchInPin = WD8;

SmartStepper* slideStepper;
IOBoard* ioBoard;
float rotation = 0; 
unsigned int dir = 0;
unsigned int prevClk = 0;

void setupIOBoard() {
  ioBoard = new IOBoard(latchPin, clockPin, dataOutPin, 16,
                        latchInPin, dataInPin, 8);

  ioBoard->enableAutoRead(1);
  ioBoard->addOnPortChangeHandler(4, [](ioport_level_t level){ debugMessage("Toggled switch: " + String(level) + "\tRotation: " + String(rotation)); });
  ioBoard->addOnChangeHandler([ioBoard](unsigned int changed_values){ 

    ioport_level_t direction = ioBoard->getInputState(3);
    ioport_level_t clk = ioBoard->getInputState(2);

    if(clk && !prevClk) rotation += direction ? -1 : 1;
    if(!clk && prevClk) rotation += direction ? 1 : -1;
    
    prevClk = clk;
  });
}

void setupSmartSteppers() {
  slideStepper = new SmartStepper(WD1, WD2, WD3);
  slideStepper->setStepsPerTurn(200);
  slideStepper->setResolution(MICROSTEPRESOLUTION::SIXTEENTH);
  slideStepper->setTargetSpeed(1);
}

void setup() {
   /* Test stuff */
  /*pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  initSteppers();
  serverSetup();*/

  debugSetup();
  setupIOBoard();
  setupSmartSteppers();

  slideStepper->rotate(360);
}

void loop() {
  //serverRoutine();
  slideStepper->rotateToAbsoluteAngle(rotation*45);
}