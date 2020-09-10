#ifndef IO_H
#define IO_H

#include <Arduino.h>

#include "common.h"
#include "IOBoard.h"

void setupIOBoard();

IOBoard* getIOBoard();

int32_t getEncoderRotation();

int32_t getEncoderDeltaRotation();

String getIOJsonData(String tabString);

String getEncoderJsonData(String tabString);

void takePhoto(unsigned int time);

void playStartLEDRoutine();

void allLEDOff();

void setLEDStatus(int sliderState, int timelapseState);

void setBatteryStatusLED();

int getBatteryLevel();

void setCAMStatusLED();

void setStepperStatusLED(int sliderState, int timelapseState);

void handleCameraState();

bool timelapsePreviewIsInRealSpeed();
void setTimelapsePreviewIsInRealSpeed(bool value);

#endif