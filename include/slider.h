#ifndef SLDIER_H
#define SLDIER_H

#include <Arduino.h>

#include "common.h"

int getSliderState();

int getTimelapseState();

void initSlider();

void sliderLoop();

void timelapseRoutine();

void startTimelapse();

void stopTimelapse();

void startTimelapsePreview();

void stopTimelapsePreview();

void setTimelapsePanSteps(float setSteps);

void setTimelapseSlideSteps(float setSteps);

void setTimelapseInterval(gTime_t setInterval);

void setTimelapsePictureCount(unsigned int setPictureCount);

float getTimelapsePanSteps();

float getTimelapseSlideSteps();

clock_t getTimelapseInterval();

clock_t getTimelapsePictureCount();

float getTimelapseTotalSlideDistance();

float getTimelapseTotalPanDistance();

gTime_t getTimelapseTotalTime();

gTime_t getTimelapseEndTime();

gTime_t getTimelapseVideoTimePrediction(unsigned int frameRate);

bool isTimelapseInNortherHemisphere();

void setTimelapseHemisphere(bool inNorthernHemisphere);


#endif