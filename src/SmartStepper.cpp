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
    setTargetSpeed(1);
    setSpeed(0);

    setTargetAccerlaration(1);

    //ticker.attach_ms_scheduled(1, [this](){ this->tick(); });

    timerCount = 0;
    if(SmartStepper::steppers.size() <= 0) {
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
time_count_t SmartStepper::tickDeltaCount = 0;

void SmartStepper::INIT_TIMER(void) {

    debugMessage("SmartStepper: Init timer");

    noInterrupts();
    timer0_isr_init();
    timer0_attachInterrupt(SmartStepper::TICK);
    timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
    interrupts();
}

void SmartStepper::TICK(void) {
    
    time_count_t start = ESP.getCycleCount();

    for(SmartStepper* stepper : SmartStepper::steppers) {
        stepper->internTick();
    }
    
    time_count_t delta = CPU_CLK_FREQ / SmartStepper::TICK_INTERRUPT_FREQ;

    SmartStepper::tickDeltaCount = ESP.getCycleCount() - start;

    timer0_write(ESP.getCycleCount()+delta); 
}

void SmartStepper::internTick() {
    timerCount++;
    
    setAccelaration();
    if(fabs(accerlaration) > 0) {
        accelaration_t accelarationPerTick = accerlaration / SmartStepper::TICK_INTERRUPT_FREQ;
        setSpeed(speed + accelarationPerTick);
    }

    if((timerCount) % speedModulo == 0) doStep();
    
    if(timerCount % (SmartStepper::TICK_INTERRUPT_FREQ) == 0) {
        debugMessage("Speed: " + String(speed) + "\tTarget speed: " + String(targetSpeed) + "\tTimer: " + String(timerCount) + "\ttickDelta: " + String(SmartStepper::tickDeltaCount) + "\tmod: " + String(speedModulo));
    }

    if(timerCount >= SmartStepper::TICK_INTERRUPT_FREQ) timerCount = 0;
}

void SmartStepper::setSpeedModulo() {
    float stepsPerSecond = speed * stepsPerTurn;
    float res = resolutionToSteps() * SmartStepper::TICK_INTERRUPT_FREQ;
    
    speedModulo = max((time_count_t)(round(res/stepsPerSecond)), (time_count_t)1);
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


void SmartStepper::setAccelaration() {
    accerlaration = 0;

    speed_t curentTargetSpeed = targetSpeed;
    steps_t delta_steps = fabs(targetSteps - steps);
    if((speed * speed * (2*stepsPerTurn) / (2 * targetAccerlaration)) >= delta_steps) curentTargetSpeed = 0;

    speed_t delta_speed = curentTargetSpeed - speed;
    if(fabs(delta_speed) < speedMin) {
        setSpeed(targetSpeed);
    }
    else {
        accerlaration = (delta_speed > 0) ? targetAccerlaration : (-1 * targetAccerlaration);
    }

    //if(timerCount % 2000 == 0)
    //    debugMessage("Speed: " + String(speed) + "\tTarget speed: " + String(targetSpeed) + "\tDelta steps: " + String(delta_steps) + "\tAcc: " + String(accerlaration));
    
}

void SmartStepper::doStep() {

    if(targetSteps == steps) return;
    
    setDirection((targetSteps > steps) ? DIRECTION::FORWARD : DIRECTION::BACKWARD);

    setSleepState(AWAKE);

    digitalWrite(pinStep , LOW); 
    digitalWrite(pinStep , HIGH);

    steps_t made_steps = resolutionToSteps();
    steps += ((direction == DIRECTION::FORWARD) ? made_steps : -made_steps);

}

void SmartStepper::setTargetSteps(steps_t setTargetSteps) {
    steps_t resolution = resolutionToSteps();

    targetSteps = setTargetSteps - fmod(setTargetSteps, resolution);
}

void SmartStepper::setSleepState(SLEEPSTATE setState) {
    digitalWrite(pinSleep, ((setState == SLEEPSTATE::SLEEP) ? LOW : HIGH));
    sleepState = setState;
}

void SmartStepper::setSpeed(speed_t setSpeed) { 
    speed = min(max(setSpeed, speedMin), getMaxSpeed());
    setSpeedModulo();
}

void SmartStepper::setTargetSpeed(speed_t setTargetSpeed) {
    targetSpeed = min(max(setTargetSpeed, speedMin), getMaxSpeed());
}

void SmartStepper::setTargetAccerlaration(accelaration_t setAcc) {
    accelaration_t minAcc = 0.1;
    accelaration_t maxAcc = 2;
    targetAccerlaration = min(max(setAcc, minAcc), maxAcc);
}

speed_t SmartStepper::getMaxSpeed() {
    speed_t technical_possible = resolutionToSteps() * SmartStepper::TICK_INTERRUPT_FREQ / stepsPerTurn;
    return min(technical_possible, (speed_t)20);
}

void SmartStepper::setResolution(MICROSTEPRESOLUTION setRes) {
    resolution = setRes;
    setSpeed(speed);
}

void SmartStepper::setStepsPerTurn(steps_t setSPT) {
    stepsPerTurn = setSPT;
    setSpeed(speed);
}

void SmartStepper::setDirection(DIRECTION setDir) { 
    digitalWrite(pinDirection, ((setDir == DIRECTION::FORWARD) ? LOW : HIGH));
    direction = setDir; 
}

void SmartStepper::rotate(angle_t angle) {
    setTargetSteps(steps + angle / 360 * stepsPerTurn);
    debugMessage("Rotate " + String(angle) + "°");
}

void SmartStepper::waitUntilTargetReached() {
    while(fabs(targetSteps - steps) > resolutionToSteps()) yield();
    return;
}

String SmartStepper::getDebugMessage() {
    return "SmartStepper: Steps:" + String(steps, '\004') 
            + "\tTarget stpes: " + String(targetSteps, '\004');
}