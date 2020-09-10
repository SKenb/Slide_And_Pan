#include "SmartStepper.h"
#include "io.h"

SmartStepper::SmartStepper(pin_t setDirectionPin, pin_t setStepPin, pin_t setSleepPin)
{
    debugMessage("SmartStepper: new Stepper (current #" + String(SmartStepper::steppers.size()) + ")");

    name = "<NOT SPECIFIED>";
    lazyMode = false;

    pinDirection = setDirectionPin;
    pinSleep = setSleepPin;
    pinStep = setStepPin;

    pinMode(pinDirection, OUTPUT);
    pinMode(pinStep, OUTPUT);
    //pinMode(pinSleep, OUTPUT);

    setResolution(MICROSTEPRESOLUTION::SIXTEENTH);
    setChangeResolutionMethod(nullptr);

    steps = 0;
    targetSteps = 0;

    speedMin = .05;
    setTargetSpeed(1);
    setSpeed(0);

    setTargetAcceleration(1);

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
            it = SmartStepper::steppers.erase(it);
        }
        else
        {
            it++;    
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
    
    setAcceleration();

    if(fabs(acceleration) > 0) {
        acceleration_t accelerationPerTick = acceleration / SmartStepper::TICK_INTERRUPT_FREQ;
        setSpeed(speed + accelerationPerTick);
    }

    if((timerCount) % speedModulo == 0) doStep();
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

void SmartStepper::setChangeResolutionMethod(std::function<void (MICROSTEPRESOLUTION)> setChangeResolutionMethod) {
    changeResolutionMethod = setChangeResolutionMethod;
}


void SmartStepper::setIdealResolutionIfPossible() {
    if(changeResolutionMethod) {
        MICROSTEPRESOLUTION resolution = calculateIdealResolutionFromSpeed();
        changeResolutionMethod(resolution);
        setResolution(resolution);
    }
}

MICROSTEPRESOLUTION SmartStepper::calculateIdealResolutionFromSpeed() {
    if(speed >= 2) return MICROSTEPRESOLUTION::FULL;
    else if(speed >= 1) return MICROSTEPRESOLUTION::HALF;
    else if(speed >= .5) return MICROSTEPRESOLUTION::QUARTER;
    else if(speed >= .25) return MICROSTEPRESOLUTION::EIGHTH;
    else if(speed >= .125) return MICROSTEPRESOLUTION::SIXTEENTH;

    return MICROSTEPRESOLUTION::SIXTEENTH;
}


void SmartStepper::setAcceleration() {
    acceleration = 0;

    speed_t curentTargetSpeed = targetSpeed;
    steps_t delta_steps = fabs(targetSteps - steps);
    if((speed * speed * stepsPerTurn / (2 * targetAcceleration)) >= delta_steps) curentTargetSpeed = 0;

    speed_t delta_speed = curentTargetSpeed - speed;
    if(fabs(delta_speed) < speedMin) {
        setSpeed(targetSpeed);
    }
    else {
        acceleration = (delta_speed > 0) ? targetAcceleration : (-1 * targetAcceleration);
    }

    /*if(timerCount % 5000 == 0)
        debugMessage("Speed: " + String(speed) + "\tTarget speed: " + String(targetSpeed) + "\tDelta steps: " + String(delta_steps) + "\tAcc: " + String(acceleration));
    */
}

void SmartStepper::doStep() {

    if(targetSteps == steps) {
        //if(isInLazyMode() && !isSleeping()) setSleepState(SLEEP);
        if(isInLazyMode()) setSleepState(SLEEP);
        return;
    }
    
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
    //digitalWrite(pinSleep, ((setState == SLEEPSTATE::SLEEP) ? LOW : HIGH));
    getIOBoard()->setAndApplyOutput(pinSleep, ((setState == SLEEPSTATE::SLEEP) ? LOW : HIGH));
    sleepState = setState;
}

void SmartStepper::setSpeed(speed_t setSpeed) { 
    speed = min(max(setSpeed, speedMin), getMaxSpeed());
    setSpeedModulo();
}

void SmartStepper::setTargetSpeed(speed_t setTargetSpeed) {
    targetSpeed = min(max(setTargetSpeed, speedMin), getMaxSpeed());
}

void SmartStepper::setTargetAcceleration(acceleration_t setAcc) {
    acceleration_t minAcc = 0.01;
    acceleration_t maxAcc = 20;
    targetAcceleration = min(max(setAcc, minAcc), maxAcc);
}

speed_t SmartStepper::getMaxSpeed() {
    speed_t technical_possible = resolutionToSteps() * SmartStepper::TICK_INTERRUPT_FREQ / stepsPerTurn;
    return min(technical_possible, (speed_t)20);
}

void SmartStepper::setResolution(MICROSTEPRESOLUTION setRes) {
    resolution = setRes;
    if(changeResolutionMethod) changeResolutionMethod(setRes);

    setSpeed(speed);
}

void SmartStepper::setResolutionFromInteger(int setRes) {
    switch (setRes)
    {
        case 1:
            setResolution(MICROSTEPRESOLUTION::FULL);
            break;
        case 2:
            setResolution(MICROSTEPRESOLUTION::HALF);
            break;
        case 4:
            setResolution(MICROSTEPRESOLUTION::QUARTER);
            break;
        case 8:
            setResolution(MICROSTEPRESOLUTION::EIGHTH);
            break;
        case 16:
            setResolution(MICROSTEPRESOLUTION::SIXTEENTH);
            break;        
        default:
            break;
    }
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
    //debugMessage("Rotate " + String(angle) + "°");
}

void SmartStepper::rotateToAbsoluteAngle(angle_t angle) {
    setTargetSteps(angle / 360 * stepsPerTurn);
}

void SmartStepper::waitUntilTargetReached() {
    while(fabs(targetSteps - steps) > resolutionToSteps()) yield();
    return;
}

String SmartStepper::getDebugMessage() {
    return "SmartStepper: Steps:" + String(steps, '\004') 
            + "\tTarget stpes: " + String(targetSteps, '\004');
}


String SmartStepper::toJson(String myName, String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Name", myName, myTabString, true);
    jsonAddField(json, "Hardware", hardwareJson(myTabString), myTabString);
    jsonAddField(json, "Position", postionJson(myTabString), myTabString);
    jsonAddField(json, "Speed", speedJson(myTabString), myTabString);
    jsonAddField(json, "Acceleration", accJson(myTabString), myTabString, false, true);

    return json + "}";
}

String SmartStepper:: hardwareJson(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "DirectionPin", String(pinDirection), myTabString, true);
    jsonAddField(json, "StepPin", String(pinStep), myTabString, true);
    jsonAddField(json, "SleepPin", String(pinSleep), myTabString, true);
    jsonAddField(json, "StepsPerTurn", String(stepsPerTurn), myTabString);
    jsonAddField(json, "Resolution", "1/" + String(1/resolutionToSteps()), myTabString, true);
    jsonAddField(json, "Sleep", sleepState == SLEEPSTATE::AWAKE ? "false" : "true", myTabString);
    jsonAddField(json, "CanChangeResolution", changeResolutionMethod == nullptr ? "false" : "true", myTabString, false, true);

    return json + tabString + "}";
}

String SmartStepper:: postionJson(String tabString){
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Steps", String(steps), myTabString);
    jsonAddField(json, "TargetSteps", String(targetSteps), myTabString);
    jsonAddField(json, "Direction", String(direction == DIRECTION::BACKWARD ? "Backward" : "Forward"), myTabString, true, true);

    return json + tabString + "}";
}

String SmartStepper:: speedJson(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Speed", String(speed), myTabString);
    jsonAddField(json, "TargetSpeed", String(targetSpeed), myTabString);
    jsonAddField(json, "Min", String(speedMin), myTabString);
    jsonAddField(json, "Max", String(getMaxSpeed()), myTabString, false, true);

    return json + tabString + "}";
}

String SmartStepper:: accJson(String tabString) {
    String json = "{\n";
    String myTabString = tabString + "\t";

    jsonAddField(json, "Acceleration", String(acceleration), myTabString);
    jsonAddField(json, "TargetAcceleration", String(targetAcceleration), myTabString, false, true);
    return json + tabString + "}";
}


String SmartStepper::streamData(String myName) {
    String stream = "\"" + myName + "\": {";

    stream += "\"Steps\": " + String(steps);
    stream += ", \"Direction\": " + String(direction == DIRECTION::BACKWARD ? "Backward" : "Forward");
    stream += ", \"Speed\": " + String(speed);

    return stream + "}";
}

void SmartStepper::setPhysicalValueInfos(String value, String unit) { 
    physicalValue = value;
    physicalValueUnit = unit;
}
String SmartStepper::getPhysicalValueInfo(bool withValuePraefix) {
    String valueAndUnit = String(getPhysicalValue()) + " " + physicalValueUnit;
    return withValuePraefix ? physicalValue + " " + valueAndUnit : valueAndUnit;
}

