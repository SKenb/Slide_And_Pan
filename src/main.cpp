#include <Arduino.h>

#include "common.h"
#include "debug.h"
#include "wifi.h"
#include "stepper.h"
#include "io.h"

void timer0_ISR (void);

void setup() {

  debugSetup();
  serverSetup();
  setupIOBoard();
  setupSteppers();

}

void loop() {
  serverRoutine();
  //getSlideStepper()->rotateToAbsoluteAngle((angle_t)getEncoderRotation()*45);
}