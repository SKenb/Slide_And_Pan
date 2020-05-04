#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>

#include "common.h"
#include "io.h"
#include "SmartStepper.h"

#define STEPPER_STEPS_PER_TURN 200

#define SLIDE
#ifdef SLIDE
#define SLIDE_STEPPER_PIN_DIRECTION WD1
#define SLIDE_STEPPER_PIN_STEP WD2
#define SLIDE_STEPPER_PIN_SLEEP WD3
#endif

#define SLIDE_CAN_CHANGE_RESOLUTION
#ifdef SLIDE_CAN_CHANGE_RESOLUTION
#define SLIDE_STEPPER_DRIVER_M1 1
#define SLIDE_STEPPER_DRIVER_M2 2
#define SLIDE_STEPPER_DRIVER_M3 3
#endif

#define PAN
#ifdef PAN
#define PAN_STEPPER_PIN_DIRECTION WD4
#define PAN_STEPPER_PIN_STEP WD4
#define PAN_STEPPER_PIN_SLEEP WD4
#endif

#define PAN_CAN_CHANGE_RESOLUTION
#ifdef PAN_CAN_CHANGE_RESOLUTION
#define PAN_STEPPER_DRIVER_M1 4
#define PAN_STEPPER_DRIVER_M2 5
#define PAN_STEPPER_DRIVER_M3 6
#endif

void setupSteppers();

SmartStepper* getSlideStepper();
SmartStepper* getPanStepper();

String slideJsonData(String tabString);
String panJsonData(String tabString);

void genericChangeResolutionMethod(pin_t M1, pin_t M2, pin_t M3, MICROSTEPRESOLUTION resolution);

#endif