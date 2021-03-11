#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>

#include "common.h"
#include "io.h"
#include "SmartStepper.h"

#define STEPPER_STEPS_PER_TURN 200

#define SLIDE
#ifdef SLIDE
#define SLIDE_STEPPER_PIN_DIRECTION WD3
#define SLIDE_STEPPER_PIN_STEP WD4
#define SLIDE_STEPPER_PIN_SLEEP 7
#define SLIDE_STEPPER_LAZYMODE_DEFAULT_VALUE true

#define SLIDE_GEAR_DIAMETER 10.11

#endif

#define SLIDE_CAN_CHANGE_RESOLUTION
#ifdef SLIDE_CAN_CHANGE_RESOLUTION
#define SLIDE_STEPPER_DRIVER_M1 3
#define SLIDE_STEPPER_DRIVER_M2 4
#define SLIDE_STEPPER_DRIVER_M3 5
#endif

#define PAN
#ifdef PAN
#define PAN_STEPPER_PIN_DIRECTION WD1
#define PAN_STEPPER_PIN_STEP WD2
#define PAN_STEPPER_PIN_SLEEP 6
#define PAN_STEPPER_LAZYMODE_DEFAULT_VALUE false

#define PAN_GEAR_SMALL_COG_NUMBER 16.0
#define PAN_GEAR_LARGE_COG_NUMBER 60.0

#endif

#define PAN_CAN_CHANGE_RESOLUTION
#ifdef PAN_CAN_CHANGE_RESOLUTION
#define PAN_STEPPER_DRIVER_M1 0
#define PAN_STEPPER_DRIVER_M2 1
#define PAN_STEPPER_DRIVER_M3 2
#endif

void setupSteppers();

SmartStepper* getSlideStepper();
SmartStepper* getPanStepper();

String slideJsonData(String tabString);
String panJsonData(String tabString);

void genericChangeResolutionMethod(pin_t M1, pin_t M2, pin_t M3, MICROSTEPRESOLUTION resolution);

String stepperHandleCommand(String command, String commandValue, String tabString);

#endif