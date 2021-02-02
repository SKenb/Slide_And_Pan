#include "io.h"
#include "stepper.h"

IOBoard* ioBoard;

int32_t encoderRotation = 0; 
int32_t oldEncoderRotation = 0;
bool encoderPressed = false;

unsigned int dir = 0;
unsigned int prevClk = 0;

bool timelapsePreviewIsInRealSpeedFlag = false;

void setupIOBoard() {
  ioBoard = new IOBoard(PIN_LATCH, PIN_CLOCK, PIN_DATAOUT, 16,
                        PIN_LATCHIN, PIN_DATAIN, 8);

  //ioBoard->enableAutoRead(10);
  ioBoard->addOnPortChangeHandler(ENCODER_PIN_SWITCH, [](ioport_level_t level){ 
    encoderPressed = level <= 0;
  });

  ioBoard->addOnChangeHandler([ioBoard](unsigned int changed_values){ 

    ioport_level_t direction = ioBoard->getInputState(ENCODER_PIN_CLK);
    ioport_level_t clk = ioBoard->getInputState(ENCODER_PIN_DIRECTION);

    oldEncoderRotation = encoderRotation;
    if(clk && !prevClk) encoderRotation += direction ? -1 : 1;
    if(!clk && prevClk) encoderRotation += direction ? 1 : -1;
    
    prevClk = clk;
  });

}

IOBoard* getIOBoard() { return ioBoard; }

int32_t getEncoderRotation() { return encoderRotation; }

int32_t getEncoderDeltaRotation() { return encoderRotation - oldEncoderRotation;}

String getIOJsonData(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Battery", String(getIOBoard()->getBatteryLevel()), myTabString, false, false);
    jsonAddField(json, "Camera", getIOBoard()->cameraStateToString(), myTabString, false, false);
    jsonAddField(json, "Encoder", getEncoderJsonData(myTabString), myTabString, false, true);
    return json + tabString + "}";
}

String getEncoderJsonData(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Rotation", String(getEncoderRotation()), myTabString, false);
    jsonAddField(json, "DirectionPin", String(ENCODER_PIN_DIRECTION), myTabString, true);
    jsonAddField(json, "ClockPin", String(ENCODER_PIN_CLK), myTabString, true, true);

    return json + tabString +"}";
}


bool isEncoderPressed() { return encoderPressed; }

void takePhoto(unsigned int time) {
  getIOBoard()->setAndApplyOutput(PIN_CAM, HIGH);
  delay(time);
  getIOBoard()->setAndApplyOutput(PIN_CAM, LOW);
}

void playStartLEDRoutine() {
  int delayTime = 200;
  getIOBoard()->setAndApplyOutput(PIN_LED_1_RED, HIGH);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_2_RED, HIGH);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_3_RED, HIGH);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_1_GREEN, HIGH);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_2_GREEN, HIGH);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_3_GREEN, HIGH);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_1_RED, LOW);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_2_RED, LOW);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_3_RED, LOW);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_1_GREEN, LOW);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_2_GREEN, LOW);
  delay(delayTime);
  getIOBoard()->setAndApplyOutput(PIN_LED_3_GREEN, LOW);

}

void allLEDOff() {
  getIOBoard()->setOutput(PIN_LED_1_RED, LOW);
  getIOBoard()->setOutput(PIN_LED_2_RED, LOW);
  getIOBoard()->setOutput(PIN_LED_3_RED, LOW);
  getIOBoard()->setOutput(PIN_LED_1_GREEN, LOW);
  getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
  getIOBoard()->setOutput(PIN_LED_3_GREEN, LOW);
  //getIOBoard()->applyOutputState();
}


void handleCameraState() {
  
  switch (getIOBoard()->cameraGetStatus())
  {
    case CAM_STATE_IDLE:
      getIOBoard()->setOutput(PIN_CAM, LOW);
      break;

    case CAM_STATE_PREPARE:
      getIOBoard()->setOutput(PIN_CAM, LOW);
      break;

    case CAM_STATE_PHOTO:
      getIOBoard()->setOutput(PIN_CAM, HIGH);
      break;

    case CAM_STATE_REST:
      getIOBoard()->setOutput(PIN_CAM, LOW);
      break;

    default:
      getIOBoard()->setOutput(PIN_CAM, LOW);
      break;
  }
}

void setLEDStatus(int sliderState, int timelapseState) {
  allLEDOff();

  if(sliderState == STATE_BETTERY_EMPTY) {
    getIOBoard()->setOutput(PIN_LED_1_RED, HIGH);
    getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
    getIOBoard()->setOutput(PIN_LED_3_RED, HIGH);
  }
  else {
    setStepperStatusLED(sliderState, timelapseState);
    setBatteryStatusLED();
    setCAMStatusLED();
  }

  //getIOBoard()->applyOutputState();
}

void setBatteryStatusLED() {
  int batteryLevel = getIOBoard()->getBatteryLevel();

  if(batteryLevel >= BATTERY_GREEN_MIN) {

    if(isSliderInDarkMode()) {
      //
    }
    else {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, HIGH);
      getIOBoard()->setOutput(PIN_LED_1_RED, LOW);
    }
  }
  else if(batteryLevel >= BATTERY_ORANGE_MIN) {
  
    if(isSliderInDarkMode()) {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, getIOBoard()->getBlinkSecond());
      getIOBoard()->setOutput(PIN_LED_1_RED, getIOBoard()->getBlinkSecond());
    }
    else {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, HIGH);
      getIOBoard()->setOutput(PIN_LED_1_RED, HIGH);
    }
  }
  else if(batteryLevel >= BATTERY_RED_MIN) {

    if(isSliderInDarkMode()) {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, LOW);
      getIOBoard()->setOutput(PIN_LED_1_RED, getIOBoard()->getBlinkSecond());
    }
    else {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, LOW);
      getIOBoard()->setOutput(PIN_LED_1_RED, HIGH);
    }
  }
  else if(batteryLevel >= BATTERY_RED_BLINK_MIN) {

    if(isSliderInDarkMode()) {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, LOW);
      getIOBoard()->setOutput(PIN_LED_1_RED, getIOBoard()->getBlinkHalfSecond());
    }
    else {
      getIOBoard()->setOutput(PIN_LED_1_GREEN, LOW);
      getIOBoard()->setOutput(PIN_LED_1_RED, getIOBoard()->getBlinkSecond());
    }
  }
  else {
    getIOBoard()->setOutput(PIN_LED_1_GREEN, LOW);
    getIOBoard()->setOutput(PIN_LED_1_RED, getIOBoard()->getBlinkHalfSecond());
  }

}

void setCAMStatusLED() {
  switch (getIOBoard()->cameraGetStatus())
  {
    case CAM_STATE_IDLE:
      if(isSliderInDarkMode()) {
        getIOBoard()->setOutput(PIN_LED_3_GREEN, LOW);
        getIOBoard()->setOutput(PIN_LED_3_RED, LOW);
      }
      else {
        getIOBoard()->setOutput(PIN_LED_3_GREEN, HIGH);
        getIOBoard()->setOutput(PIN_LED_3_RED, LOW);
      }
      break;

    case CAM_STATE_PREPARE:
      getIOBoard()->setOutput(PIN_LED_3_GREEN, LOW);
      getIOBoard()->setOutput(PIN_LED_3_RED, getIOBoard()->getBlinkHalfSecond());
      break;

    case CAM_STATE_PHOTO:
      if(isSliderInDarkMode()) {
        getIOBoard()->setOutput(PIN_LED_3_GREEN, LOW);
        getIOBoard()->setOutput(PIN_LED_3_RED, LOW);
      }
      else {
        getIOBoard()->setOutput(PIN_LED_3_GREEN, LOW);
        getIOBoard()->setOutput(PIN_LED_3_RED, HIGH);
      }
      break;

    case CAM_STATE_REST:
      getIOBoard()->setOutput(PIN_LED_3_GREEN, getIOBoard()->getBlinkHalfSecond());
      getIOBoard()->setOutput(PIN_LED_3_RED, LOW);
      break;

    default:
      getIOBoard()->setOutput(PIN_LED_3_GREEN, getIOBoard()->getBlinkHalfSecond());
      getIOBoard()->setOutput(PIN_LED_3_RED, getIOBoard()->getBlinkHalfSecond() == HIGH ? LOW : HIGH);
      break;
  }
}

void setStepperStatusLED(int sliderState, int timelapseState) {
  getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
  getIOBoard()->setOutput(PIN_LED_2_RED, LOW);

  if(sliderState == STATE_IDLE)  {
    if(isSliderInDarkMode()) {
      getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkSecond());
    }
    else {
      getIOBoard()->setOutput(PIN_LED_2_GREEN, HIGH);
    }
  }
  
  if(sliderState == STATE_TIMELAPSE) {
      switch(timelapseState) {
          case TIMELAPSE_STATE_INIT:
            getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkSecond());
            break;

          case TIMELAPSE_STATE_SET_MOVE_GOAL:
            if(isSliderInDarkMode()) {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
              getIOBoard()->setOutput(PIN_LED_2_RED, LOW);
            }
            else {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
              getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            }

            break;

          case TIMELAPSE_STATE_MOVE:
            if(isSliderInDarkMode()) {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
              getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            }
            else {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
              getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            }
            break;

          case TIMELAPSE_STATE_TRIGGER_SHOOT:
            if(isSliderInDarkMode()) {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, HIGH);
              getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            }
            else {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, HIGH);
              getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            }
            break;

          case TIMELAPSE_STATE_SHOOT:
            if(isSliderInDarkMode()) {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
              getIOBoard()->setOutput(PIN_LED_2_RED, LOW);
            }
            else {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, HIGH);
              getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            }
            break;

          case TIMELAPSE_STATE_WAIT:

            getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkHalfSecond());
            getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            break;

          case TIMELAPSE_STATE_PREVIEW_START:
            getIOBoard()->setOutput(PIN_LED_2_GREEN, LOW);
            getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            break;

          case TIMELAPSE_STATE_PREVIEW_SET_MOVE_GOAL:
            getIOBoard()->setOutput(PIN_LED_2_GREEN,LOW);
            getIOBoard()->setOutput(PIN_LED_2_RED, HIGH);
            break;

          case TIMELAPSE_STATE_PREVIEW_MOVE:
            
            if(timelapsePreviewIsInRealSpeed()) {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkHalfSecond());
              getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            }
            else {
              getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            }

            break;

          case TIMELAPSE_STATE_PREVIEW_WAIT_AT_END:
            getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkHalfSecond());
            getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            break;

          case TIMELAPSE_STATE_PREVIEW_STOP:
            
            if(timelapsePreviewIsInRealSpeed()) {
              getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkHalfSecond());
              getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            }
            else {
              getIOBoard()->setOutput(PIN_LED_2_RED, getIOBoard()->getBlinkHalfSecond());
            }

            break;

          case TIMELAPSE_STATE_FINISHED:
            getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkHalfSecond());
            break;
      }
  } 
}

bool timelapsePreviewIsInRealSpeed() { return timelapsePreviewIsInRealSpeedFlag; }

void setTimelapsePreviewIsInRealSpeed(bool value) { timelapsePreviewIsInRealSpeedFlag = value; }
