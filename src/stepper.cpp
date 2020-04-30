#include "stepper.h"

SmartStepper* slideStepper;
SmartStepper* panStepper;

void setupSteppers() {

#ifdef SLIDE
  slideStepper = new SmartStepper(SLIDE_STEPPER_PIN_DIRECTION, SLIDE_STEPPER_PIN_STEP, SLIDE_STEPPER_PIN_SLEEP);
#ifdef SLIDE_CAN_CHANGE_RESOLUTION  
  slideStepper->setChangeResolutionMethod([](MICROSTEPRESOLUTION resolution) { genericChangeResolutionMethod(SLIDE_STEPPER_DRIVER_M1, SLIDE_STEPPER_DRIVER_M2, SLIDE_STEPPER_DRIVER_M3, resolution); });
#endif
  slideStepper->setResolution(MICROSTEPRESOLUTION::QUARTER);
  slideStepper->setStepsPerTurn(STEPPER_STEPS_PER_TURN);
  slideStepper->setTargetSpeed(2);
#endif

#ifdef PAN
  panStepper = new SmartStepper(PAN_STEPPER_PIN_DIRECTION, PAN_STEPPER_PIN_STEP, PAN_STEPPER_PIN_SLEEP);
#ifdef PAN_CAN_CHANGE_RESOLUTION
  panStepper->setChangeResolutionMethod([](MICROSTEPRESOLUTION resolution) { genericChangeResolutionMethod(PAN_STEPPER_DRIVER_M1, PAN_STEPPER_DRIVER_M2, PAN_STEPPER_DRIVER_M3, resolution); });
#endif
  panStepper->setResolution(MICROSTEPRESOLUTION::QUARTER);
  panStepper->setStepsPerTurn(STEPPER_STEPS_PER_TURN);
  panStepper->setTargetSpeed(2);
#endif

}

SmartStepper* getSlideStepper() { return slideStepper; }
SmartStepper* getPanStepper() { return panStepper; }


String slideJsonData(String tabString) {
  String json = "{\n";
  String myTabString = tabString + "\t";

  jsonAddField(json, "Name", "Slide", myTabString, true);

#ifdef SLIDE
  jsonAddField(json, "Available", "true", myTabString, false);
  jsonAddField(json, "Stepper", slideStepper->toJson("SlideStepper", myTabString), myTabString, false);
#else
  jsonAddField(json, "Available", "false", myTabString, false);
#endif

  return json + tabString + "}";
}

String panJsonData(String tabString) {
  String json = "{\n";
  String myTabString = tabString + "\t";

  jsonAddField(json, "Name", "Pan", myTabString, true);

#ifdef PAN
  jsonAddField(json, "Available", "true", myTabString, false);
  jsonAddField(json, "Stepper", slideStepper->toJson("SlideStepper", myTabString), myTabString, false);
#else
  jsonAddField(json, "Available", "false", myTabString, false);
#endif

  return json + tabString + "}";
}

void genericChangeResolutionMethod(pin_t M1, pin_t M2, pin_t M3, MICROSTEPRESOLUTION resolution) {
  switch (resolution)
  {
    case MICROSTEPRESOLUTION::FULL:
      getIOBoard()->setAndApplyOutput(M1, LOW);
      getIOBoard()->setAndApplyOutput(M2, LOW);
      getIOBoard()->setAndApplyOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::HALF:
      getIOBoard()->setAndApplyOutput(M1, HIGH);
      getIOBoard()->setAndApplyOutput(M2, LOW);
      getIOBoard()->setAndApplyOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::QUARTER:
      getIOBoard()->setAndApplyOutput(M1, LOW);
      getIOBoard()->setAndApplyOutput(M2, HIGH);
      getIOBoard()->setAndApplyOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::EIGHTH:
      getIOBoard()->setAndApplyOutput(M1, HIGH);
      getIOBoard()->setAndApplyOutput(M2, HIGH);
      getIOBoard()->setAndApplyOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::SIXTEENTH:
      getIOBoard()->setAndApplyOutput(M1, HIGH);
      getIOBoard()->setAndApplyOutput(M2, HIGH);
      getIOBoard()->setAndApplyOutput(M3, HIGH);
      break;

    default:
      getIOBoard()->setAndApplyOutput(M1, LOW);
      getIOBoard()->setAndApplyOutput(M2, LOW);
      getIOBoard()->setAndApplyOutput(M3, LOW);
      break;
  }
}