#include "SmartStepper.h"

SmartStepper::SmartStepper(pin_t setDirectionPin, pin_t setStepPin, pin_t setSleepPin)
{
    debugMessage("SmartStepper: new Stepper (current #" + String(SmartStepper::steppers.size()) + ")");

    pinDirection = setDirectionPin;
    pinSleep = setSleepPin;
    pinStep = setStepPin;

    pinMode(pinDirection, OUTPUT);
    pinMode(pinStep, OUTPUT);
    pinMode(pinSleep, OUTPUT);

    speedMin = .01;
    setSpeed(1);

    //ticker.attach_ms_scheduled(1, [this](){ this->tick(); });

    timerCount = 0;
    if(SmartStepper::steppers.size() <= 0) {
        digitalWrite(PIN_LED_RED, HIGH);
        SmartStepper::INIT_TIMER();
    }

    SmartStepper::steppers.push_back(this);
}

SmartStepper::~SmartStepper()
{
    std::vector<SmartStepper*>::iterator it = SmartStepper::steppers.begin();
    while(it != SmartStepper::steppers.end()) {
        if(*it == this) {
            SmartStepper::steppers.erase(it);
        }
    }

    debugMessage("SmartStepper: removed Stepper (still #" + String(SmartStepper::steppers.size()) + ")");
}

std::vector<SmartStepper*> SmartStepper::steppers;

void SmartStepper::INIT_TIMER(void) {

    debugMessage("SmartStepper: Init timer");

    noInterrupts();
    timer0_isr_init();
    timer0_attachInterrupt(SmartStepper::TICK);
    timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
    interrupts();
}

void SmartStepper::TICK(void) {
    
    for(SmartStepper* stepper : SmartStepper::steppers) {
        stepper->internTick();
    }
    
    uint16_t delta = CPU_CLK_FREQ / SmartStepper::TICK_INTERRUPT_FREQ;
    timer0_write(ESP.getCycleCount()+delta); 
}

void SmartStepper::internTick() {
    timerCount++;

    float stepsPerSecond = speed * stepsPerTurn;
    float res = resolutionToSteps() * SmartStepper::TICK_INTERRUPT_FREQ;

    uint16_t mod = max((uint16_t)(res/stepsPerSecond), (uint16_t)1);

    if(timerCount % mod == 0) doStep();

    if(timerCount >= SmartStepper::TICK_INTERRUPT_FREQ) {
        timerCount = 0;
        debugMessage("halfMSPerTotlaDoStep: " + String(res/stepsPerSecond));
        debugMessage(getDebugMessage());
    }
}

steps_t SmartStepper::resolutionToSteps() {
    switch (resolution)
    {
        case MICROSTEPRESOLUTION::FULL: return 1;
        case MICROSTEPRESOLUTION::HALF: return .5;
        case MICROSTEPRESOLUTION::QUARTER: return .25;
        case MICROSTEPRESOLUTION::EIGHTH: return .125;
        case MICROSTEPRESOLUTION::SIXTEENTH: return .0625;
        default:
            debugError("Resolution not known!"); 
            return 0;
    }
}

void SmartStepper::doStep() {


    if(target_steps == steps) return;
    
    direction = (target_steps > steps) ? DIRECTION::FORWARD : DIRECTION::BACKWARD;

    setSleepState(AWAKE);

    digitalWrite(pinStep , LOW); 
    digitalWrite(pinStep , HIGH);

    steps_t made_steps = resolutionToSteps();
    steps += ((direction == DIRECTION::FORWARD) ? made_steps : -made_steps);
}


void SmartStepper::setTargetSteps(steps_t setTargetSteps) {
    steps_t resolution = resolutionToSteps();

    target_steps = setTargetSteps - fmod(setTargetSteps, resolution);
}

void SmartStepper::setSleepState(SLEEPSTATE setState) {
    digitalWrite(pinSleep, ((setState == SLEEPSTATE::SLEEP) ? LOW : HIGH));
    sleepState = setState;
}

void SmartStepper::setSpeed(speed_t setSpeed) { 
    speed = min(max(setSpeed, speedMin), getMaxSpeed()); 
    debugMessage("SmartStepper: Speed set to: " + String(speed) + "( <= " + String(getMaxSpeed()) + ")");
}

speed_t SmartStepper::getMaxSpeed() {
    return resolutionToSteps() * SmartStepper::TICK_INTERRUPT_FREQ / stepsPerTurn;
}

void SmartStepper::setResolution(MICROSTEPRESOLUTION setRes) {
    resolution = setRes;
    setSpeed(speed);
}

void SmartStepper::setStepsPerTurn(steps_t setSPT) {
    stepsPerTurn = setSPT;
    setSpeed(speed);
}

void SmartStepper::rotate(angle_t angle) {
    setTargetSteps(steps + angle / 360 * stepsPerTurn);
}

void SmartStepper::waitUntilTargetReached() {
    while(fabs(target_steps - steps) < resolutionToSteps()) yield();
    return;
}

String SmartStepper::getDebugMessage() {
    return "SmartStepper: Steps:" + String(steps, '\004') 
            + "\tTarget stpes: " + String(target_steps, '\004');
}