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
pin_t clockEnPin = WD8;

SmartStepper* slideStepper;
IOBoard* ioBoard;

void timerSetup() {
  /*noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();*/
}

void setup() {
   /* Test stuff */
  /*pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, LOW);
*/
  debugSetup();
  /*initSteppers();
  serverSetup();
  timerSetup();*/


  pinMode(dataInPin, INPUT);
  pinMode(clockEnPin, OUTPUT);

  ioBoard = new IOBoard(latchPin, clockPin, dataOutPin, 16);
}

void loop() {
  //serverRoutine();

  /*while(1) {

    ioBoard->toggleAndApplyOutput(1);
    delay(1000);

    ioBoard->setAndApplyOutput(14, HIGH);
    delay(1000);
    ioBoard->setAndApplyOutput(14, LOW);
    delay(1000);
    ioBoard->setAndApplyOutput(15, HIGH);
    delay(1000);
    ioBoard->setAndApplyOutput(15, LOW);
    delay(1000);
    ioBoard->setOutput(14, HIGH);
    ioBoard->setOutput(15, HIGH);
    ioBoard->applyOutputState();
    delay(1000);
    ioBoard->setAndApplyOutput(14, LOW);
    delay(1000);
    ioBoard->setAndApplyOutput(15, LOW);

    delay(1000);
  }*/

  while(1) {

    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    delay(1);
    digitalWrite(latchPin, HIGH);
    
    digitalWrite(clockEnPin, LOW);
    debugMessage("Read: " + String(shiftIn(dataInPin, clockPin, MSBFIRST)));
    digitalWrite(clockEnPin, HIGH);

    delay(1000);
  }
}