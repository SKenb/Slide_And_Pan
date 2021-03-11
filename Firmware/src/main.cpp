#include <Arduino.h>

#include "slider.h"
#include "common.h"
#include "debug.h"
#include "wifi.h"
#include "stepper.h"
#include "io.h"

int encoderPressedFlag = -1;

void timer0_ISR (void);

void setup() {

  debugSetup();
  serverSetup();
  setupIOBoard();
  setupSteppers();

  playStartLEDRoutine();

  initSlider();
}

void hanldeEncoderInput() {
  if(isEncoderPressed()) {

    allLEDOff();
    int val = abs(getEncoderRotation() % 6);
    
    if(val <= 2) {
      getIOBoard()->setOutput(PIN_LED_3_GREEN, HIGH);
      encoderPressedFlag = 3;
    }
    else if(val <= 4) {
      getIOBoard()->setOutput(PIN_LED_2_GREEN, HIGH);
      encoderPressedFlag = 2;
    }
    else {
      getIOBoard()->setOutput(PIN_LED_1_RED, HIGH);
      encoderPressedFlag = 1;
    }
  }
  else {
    if(encoderPressedFlag == 1) {
      stopTimelapse(); stopTimelapsePreview();
      getPanStepper()->stop();
      getSlideStepper()->stop();
    }

    if(encoderPressedFlag == 2) {
      setTimelapseHemisphere(true);
      setTimelapseInterval(35);
      setTimelapseSlideSteps(20);
      setTimelapsePictureCount(999);
      getIOBoard()->cameraSetExposureTime(25);
      getPanStepper()->setLazyMode(false);
      getSlideStepper()->setLazyMode(true);

      /* Timelapse Milkyway settings */
      getPanStepper()->setResolution(SIXTEENTH);
      getSlideStepper()->setResolution(QUARTER);

      float coeff = isTimelapseInNortherHemisphere() ? 1.0 : -1.0;
      setTimelapsePanSteps(coeff * ((float)getTimelapseInterval() * 360.0) / EARTH_SECONDS_PER_DAY);

      startTimelapse();
    }

    if(encoderPressedFlag == 3) {
      setTimelapseHemisphere(true);
      setTimelapseInterval(30);
      setTimelapseSlideSteps(20);
      setTimelapsePanSteps(20);
      setTimelapsePictureCount(999);
      getIOBoard()->cameraSetExposureTime(1);
      getPanStepper()->setLazyMode(false);
      getSlideStepper()->setLazyMode(true);

      startTimelapse();
    }

    encoderPressedFlag = 0;
  }
}

void loop() {
  serverRoutine();

  handleCameraState();
  
  if(!isEncoderPressed()) {
    setLEDStatus(getSliderState(), getTimelapseState());
  }

  getIOBoard()->applyOutputState();
  getIOBoard()->readInput();

  hanldeEncoderInput();

  sliderLoop();
}