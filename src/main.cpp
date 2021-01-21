#include <Arduino.h>

#include "slider.h"
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

  playStartLEDRoutine();

  initSlider();
}

void loop() {
  serverRoutine();

  handleCameraState();
  setLEDStatus(getSliderState(), getTimelapseState());
  getIOBoard()->applyOutputState();

  

  sliderLoop();
}