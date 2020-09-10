#include "stepper.h"

SmartStepper* slideStepper;
SmartStepper* panStepper;

void setupSteppers() {

#ifdef SLIDE
  slideStepper = new SmartStepper(SLIDE_STEPPER_PIN_DIRECTION, SLIDE_STEPPER_PIN_STEP, SLIDE_STEPPER_PIN_SLEEP);
#ifdef SLIDE_CAN_CHANGE_RESOLUTION  
  slideStepper->setChangeResolutionMethod([](MICROSTEPRESOLUTION resolution) { genericChangeResolutionMethod(SLIDE_STEPPER_DRIVER_M1, SLIDE_STEPPER_DRIVER_M2, SLIDE_STEPPER_DRIVER_M3, resolution); });
#endif
  slideStepper->setResolution(MICROSTEPRESOLUTION::SIXTEENTH);
  slideStepper->setStepsPerTurn(STEPPER_STEPS_PER_TURN);
  slideStepper->setTargetSpeed(1);
  slideStepper->setTargetAcceleration(20);

  slideStepper->setLazyMode(SLIDE_STEPPER_LAZYMODE_DEFAULT_VALUE);

  getSlideStepper()->setPhysicalValueInfos("Position", "mm");
  getSlideStepper()->setPhysicalValueRatio(SLIDE_GEAR_DIAMETER * PI / STEPPER_STEPS_PER_TURN);

  slideStepper->setName("slideStepper");
#endif

#ifdef PAN
  panStepper = new SmartStepper(PAN_STEPPER_PIN_DIRECTION, PAN_STEPPER_PIN_STEP, PAN_STEPPER_PIN_SLEEP);
#ifdef PAN_CAN_CHANGE_RESOLUTION
  panStepper->setChangeResolutionMethod([](MICROSTEPRESOLUTION resolution) { genericChangeResolutionMethod(PAN_STEPPER_DRIVER_M1, PAN_STEPPER_DRIVER_M2, PAN_STEPPER_DRIVER_M3, resolution); });
#endif
  panStepper->setResolution(MICROSTEPRESOLUTION::SIXTEENTH);
  panStepper->setStepsPerTurn(STEPPER_STEPS_PER_TURN);
  panStepper->setTargetSpeed(1);
  panStepper->setTargetAcceleration(20);

  panStepper->setLazyMode(PAN_STEPPER_LAZYMODE_DEFAULT_VALUE);

  getPanStepper()->setPhysicalValueInfos("Angle", "°");
  getPanStepper()->setPhysicalValueRatio((360.0 * PAN_GEAR_SMALL_COG_NUMBER) / (STEPPER_STEPS_PER_TURN * PAN_GEAR_LARGE_COG_NUMBER));

  panStepper->setName("panStepper");
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
  jsonAddField(json, "Stepper", slideStepper->toJson("SlideStepper", myTabString), myTabString, false, true);
#else
  jsonAddField(json, "Available", "false", myTabString, false, true);
#endif

  return json + tabString + "}";
}

String panJsonData(String tabString) {
  String json = "{\n";
  String myTabString = tabString + "\t";

  jsonAddField(json, "Name", "Pan", myTabString, true);

#ifdef PAN
  jsonAddField(json, "Available", "true", myTabString, false);
  jsonAddField(json, "Stepper", panStepper->toJson("SlideStepper", myTabString), myTabString, false, true);
#else
  jsonAddField(json, "Available", "false", myTabString, false, true);
#endif

  return json + tabString + "}";
}

void genericChangeResolutionMethod(pin_t M1, pin_t M2, pin_t M3, MICROSTEPRESOLUTION resolution) {
  switch (resolution)
  {
    case MICROSTEPRESOLUTION::SIXTEENTH:
      getIOBoard()->setOutput(M1, HIGH);
      getIOBoard()->setOutput(M2, HIGH);
      getIOBoard()->setOutput(M3, HIGH);
      break;

    case MICROSTEPRESOLUTION::EIGHTH:
      getIOBoard()->setOutput(M1, HIGH);
      getIOBoard()->setOutput(M2, HIGH);
      getIOBoard()->setOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::QUARTER:
      getIOBoard()->setOutput(M1, LOW);
      getIOBoard()->setOutput(M2, HIGH);
      getIOBoard()->setOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::HALF:
      getIOBoard()->setOutput(M1, HIGH);
      getIOBoard()->setOutput(M2, LOW);
      getIOBoard()->setOutput(M3, LOW);
      break;

    case MICROSTEPRESOLUTION::FULL:
      getIOBoard()->setOutput(M1, LOW);
      getIOBoard()->setOutput(M2, LOW);
      getIOBoard()->setOutput(M3, LOW);
      break;

    default:
      getIOBoard()->setOutput(M1, LOW);
      getIOBoard()->setOutput(M2, LOW);
      getIOBoard()->setOutput(M3, LOW);
      break;

    getIOBoard()->applyOutputState();
  }
}

String stepperHandleCommand(String command, String commandValue, String tabString = "") {
  SmartStepper* stepper = nullptr;
  
  if(command.indexOf("slide") >= 0) stepper = getSlideStepper();
  else stepper = getPanStepper();
  
  if(stepper == nullptr) return "";

  command = command.substring(command.indexOf("_") + 1, command.length());

  String stepperCommandResult = tabString + "{\n";
  String innerTabString = tabString + "\t";

  if(command == "sleep") {
      stepper->setSleepState(((commandValue == "0") ? SLEEPSTATE::SLEEP : SLEEPSTATE::AWAKE));
      jsonAddField(stepperCommandResult, "New Sleep State", ((commandValue == "0") ? "Sleeping" : "Awake"), innerTabString, true);
  }

  if(command == "to") {
      stepper->rotateToAbsoluteAngle((angle_t)commandValue.toFloat());
      jsonAddField(stepperCommandResult, "Rotation goal", String((angle_t)commandValue.toFloat()), innerTabString, false);
  }

  if(command == "speed") {
      stepper->setTargetSpeed((speed_t)commandValue.toFloat());
      jsonAddField(stepperCommandResult, "TargetSpeed set to", String((speed_t)commandValue.toFloat()), innerTabString, false);
      jsonAddField(stepperCommandResult, "TargetSpeed is", String(stepper->getTargetSpeed()), innerTabString, false);
  }

  if(command == "acc") {
      stepper->setTargetAcceleration((acceleration_t)commandValue.toFloat());
      jsonAddField(stepperCommandResult, "TargetAcceleration set to", String((acceleration_t)commandValue.toFloat()), innerTabString, false);
      jsonAddField(stepperCommandResult, "TargetAcceleration is", String(stepper->getTargetAcceleration()), innerTabString, false);
  }

  if(command == "res") {
      stepper->setResolutionFromInteger(commandValue.toInt());
      jsonAddField(stepperCommandResult, "Resolution set to", "1/" + String(1/stepper->resolutionToSteps()), innerTabString, false);
      jsonAddField(stepperCommandResult, "hasChangeResolutionMethod", String(stepper->hasChangeResolutionMethod()), innerTabString, false);
  }

  if(command == "stop") {
      stepper->stop();
      jsonAddField(stepperCommandResult, "Motor", "Stopped", innerTabString, false);
  }

  if(command == "lazy") {
      stepper->setLazyMode(commandValue.toInt() > 0);
      jsonAddField(stepperCommandResult, "LazyMode", stepper->isInLazyMode() ? "Yes" : "No", innerTabString, false);
  }

  if(command == "reset") {
      stepper->resetStepsToZero();
      jsonAddField(stepperCommandResult, "Reset", "Steps set to zero", innerTabString, false);
  }

  if(command == "physical_value") {
      stepper->setTargetPhysicalValue(commandValue.toFloat());
      jsonAddField(stepperCommandResult, "Target physical value set to", commandValue, innerTabString, false);
  }


  jsonAddField(stepperCommandResult, "Stepper", stepper->getName(), innerTabString, true, true);

  return stepperCommandResult + "\n" + tabString + "}";
}