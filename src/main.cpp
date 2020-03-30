#include <Arduino.h>

#include "common.h"
#include "wifi.h"
#include "stepper.h"
#include "SmartStepper.h"
#include "debug.h"

void timer0_ISR (void);


SmartStepper* slideStepper;

void timerSetup() {
  /*noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();*/
}

void setup() {
   /* Test stuff */
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, LOW);

  debugSetup();
  initSteppers();
  serverSetup();
  timerSetup();

  slideStepper = new SmartStepper(STEPPER_A_DIR_PIN, STEPPER_A_STEP_PIN, STEPPER_A_SLEEP_PIN);
  slideStepper->setStepsPerTurn(200);
  slideStepper->setResolution(MICROSTEPRESOLUTION::FULL);


  slideStepper->setTargetAccerlaration(1);
  slideStepper->setTargetSpeed(10);
  slideStepper->rotate(3600000);
}

void loop() {
  serverRoutine();

}