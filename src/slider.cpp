#include "slider.h"
#include "stepper.h"
#include "io.h"

int sliderState;
int timelapseState;

gTime_t timelapseIntervalTime;
clock_t timelapsePhotoCount;
float timelapsePanSteps;
float timelapseSlideSteps;
float timelapseSlideStart;
float timelapsePanStart;
float timelapseSlideSpeedStorage;
float timelapsePanSpeedStorage;
bool timelapseInNortherHemisphere = true;

gTime_t waitUntil;

int getSliderState() { return sliderState; }

int getTimelapseState() { return timelapseState; }

void initSlider() {
    sliderState = STATE_IDLE;
    timelapseState = TIMELAPSE_STATE_INIT;

    timelapseIntervalTime = TIMELAPSE_INTERVAL_TIME_DEFAULT;
    timelapsePhotoCount = TIMELAPSE_PHOT_COUNT_DEFAULT;
    timelapsePanSteps = TIMELAPSE_PAN_STEPS_DEFAULT;
    timelapseSlideSteps =  TIMELAPSE_SLIDE_STEPS_DEFAULT;
}

void sliderLoop() {


    if(getIOBoard()->getBatteryLevel() < BATTERY_MIN && sliderState != STATE_BETTERY_EMPTY) {
        sliderState = STATE_BETTERY_EMPTY;
        getPanStepper()->stop();
        getPanStepper()->setSleepState(SLEEP);
        getSlideStepper()->stop();
        getSlideStepper()->setSleepState(SLEEP);
        debugMessage(">> State: Battery Empty (" + secondsToTimeString(getTime()) + ")");
    }

    switch (sliderState)
    {
        case STATE_IDLE:
            break;
            
        case STATE_BETTERY_EMPTY:
            if(getIOBoard()->getBatteryLevel() > BATTERY_RECOVER) {
                sliderState = STATE_IDLE;
                debugMessage(">> State: Idle (Bat. Recover) (" + secondsToTimeString(getTime()) + ")");
            }

            break;

        case STATE_TIMELAPSE:
            timelapseRoutine();

            if(timelapseState == TIMELAPSE_STATE_FINISHED && getIOBoard()->isCameraReadyForPhoto()) {
                sliderState = STATE_IDLE;
                debugMessage(">> State: Idle (Timelapse Finished) (" + secondsToTimeString(getTime()) + ")");
            }
            break;

        default:
            break;
    }
}

void timelapseRoutine() 
{
    float maxStepperSpeedForPreview = 2.0;
    float previewSpeed = 0;
    float theoreticalSlideSpeed = 0;
    float theoreticalPanSpeed = 0;
    float speedSlideToPanRatio = 0;

    float slideSpeed = 0;
    float panSpeed = 0;

    switch (timelapseState)
    {
        case TIMELAPSE_STATE_INIT:
            waitUntil = 0;
            timelapseState = TIMELAPSE_STATE_WAIT;
            debugMessage("\tTimelapseState: Wait (" + secondsToTimeString(getTime()) + ")");
            break;
        
        case TIMELAPSE_STATE_SET_MOVE_GOAL:
            getSlideStepper()->incPhysicalValue(timelapseSlideSteps);
            getPanStepper()->incPhysicalValue(timelapsePanSteps);

            timelapseState = TIMELAPSE_STATE_MOVE;
            debugMessage("\tTimelapseState: Move (" + secondsToTimeString(getTime()) + ")");

            break;

        case TIMELAPSE_STATE_MOVE:
            if( getSlideStepper()->isGoalReached() &&
                getPanStepper()->isGoalReached()) {
                    timelapseState = TIMELAPSE_STATE_WAIT;
                    debugMessage("\tTimelapseState: Wait (" + secondsToTimeString(getTime()) + ")");
                }
            break;
        
        case TIMELAPSE_STATE_WAIT:
            if(timelapsePhotoCount <= 0) {
                timelapseState = TIMELAPSE_STATE_FINISHED;
                debugMessage("\tTimelapseState: Finished (" + secondsToTimeString(getTime()) + ")");
            }

            if(waitUntil <= getTime()) {

                timelapseState = TIMELAPSE_STATE_TRIGGER_SHOOT;
                debugMessage("\tTimelapseState: Trigger Shoot (" + secondsToTimeString(getTime()) + ")");
            }
            break;
       
        case TIMELAPSE_STATE_TRIGGER_SHOOT:
            //getIOBoard()->cameraTakePhoto();
            timelapsePhotoCount--;
            
            waitUntil = getTime() + timelapseIntervalTime;

            timelapseState = TIMELAPSE_STATE_SHOOT;
            debugMessage("\tTimelapseState: Shoot (" + secondsToTimeString(getTime()) + " - Wait Until: " + secondsToTimeString(waitUntil) + ")");

        case TIMELAPSE_STATE_SHOOT:

            if (getIOBoard()->isCameraReadyForPhoto()) {
                timelapseState = TIMELAPSE_STATE_SET_MOVE_GOAL;
                debugMessage("\tTimelapseState: set Move-Goal (" + secondsToTimeString(getTime()) + ")");
            }

            break;

        case TIMELAPSE_STATE_FINISHED:
            getIOBoard()->setOutput(PIN_LED_2_GREEN, getIOBoard()->getBlinkHalfSecond());
            break;

        case TIMELAPSE_STATE_PREVIEW_START:
            timelapseSlideSpeedStorage = getSlideStepper()->getTargetSpeed();
            timelapsePanSpeedStorage = getPanStepper()->getTargetSpeed();

            timelapseSlideStart = getSlideStepper()->getPhysicalValue();
            timelapsePanStart = getPanStepper()->getPhysicalValue();

            theoreticalSlideSpeed = fabs(getSlideStepper()->physicalValueToSteps(getTimelapseSlideSteps()) / getTimelapseInterval());
            theoreticalPanSpeed = fabs(getPanStepper()->physicalValueToSteps(getTimelapsePanSteps()) / getTimelapseInterval());
            
            //previewSpeed = min(max(theoreticalSlideSpeed, theoreticalPanSpeed), maxStepperSpeedForPreview);
            previewSpeed = maxStepperSpeedForPreview;
            setTimelapsePreviewIsInRealSpeed(previewSpeed < maxStepperSpeedForPreview);
            
            speedSlideToPanRatio = fabs(theoreticalSlideSpeed / theoreticalPanSpeed);

            slideSpeed = theoreticalSlideSpeed > theoreticalPanSpeed ? previewSpeed : previewSpeed * speedSlideToPanRatio;
            panSpeed = theoreticalSlideSpeed > theoreticalPanSpeed ? previewSpeed / speedSlideToPanRatio : previewSpeed;

            getSlideStepper()->setTargetSpeed(slideSpeed);
            getPanStepper()->setTargetSpeed(panSpeed);

            debugMessage("Slide speed: " + String(slideSpeed));
            debugMessage("Pan speed: " + String(panSpeed));

            timelapseState = TIMELAPSE_STATE_PREVIEW_SET_MOVE_GOAL;
            debugMessage("\tTimelapseState: Prev. set Move-Goal (" + secondsToTimeString(getTime()) + ")");

            break;

        case TIMELAPSE_STATE_PREVIEW_MOVE:
            if( getSlideStepper()->isGoalReached() &&
                getPanStepper()->isGoalReached()) {
                waitUntil = getTime() + 5;
                timelapseState = TIMELAPSE_STATE_PREVIEW_WAIT_AT_END;
                debugMessage("\tTimelapseState: Prev. wait @ end (" + secondsToTimeString(getTime()) + ")");
            }

            break;

        case TIMELAPSE_STATE_PREVIEW_SET_MOVE_GOAL:
            getSlideStepper()->incPhysicalValue(getTimelapsePictureCount() * getTimelapseSlideSteps());
            getPanStepper()->incPhysicalValue(getTimelapsePictureCount() * getTimelapsePanSteps());

            timelapseState = TIMELAPSE_STATE_PREVIEW_MOVE;
            debugMessage("\tTimelapseState: Prev. move (" + secondsToTimeString(getTime()) + ")");

            break;

        case TIMELAPSE_STATE_PREVIEW_WAIT_AT_END:
            if(waitUntil <= getTime()) {
                timelapseState = TIMELAPSE_STATE_PREVIEW_STOP;
                debugMessage("\tTimelapseState: Prev. Stop (" + secondsToTimeString(getTime()) + ")");

            }
            break;

        case TIMELAPSE_STATE_PREVIEW_STOP:
            getSlideStepper()->setTargetPhysicalValue(timelapseSlideStart);
            getPanStepper()->setTargetPhysicalValue(timelapsePanStart);

            if( getSlideStepper()->isGoalReached() &&
                getPanStepper()->isGoalReached()) {
                
                getSlideStepper()->setTargetSpeed(timelapseSlideSpeedStorage);
                getPanStepper()->setTargetSpeed(timelapsePanSpeedStorage);

                timelapseState = TIMELAPSE_STATE_FINISHED;
                debugMessage("\tTimelapseState: Finished (" + secondsToTimeString(getTime()) + ")");

            }

            break;
    
        default:
            timelapseState = TIMELAPSE_STATE_INIT;
            debugMessage("\tTimelapseState: (Default) Init (" + secondsToTimeString(getTime()) + ")");

            break;
    }
}


void startTimelapse() {
    if(sliderState != STATE_TIMELAPSE) {
        timelapseState = TIMELAPSE_STATE_INIT;
        sliderState = STATE_TIMELAPSE; 
        
        debugMessage("\tTimelapseState: Init (" + secondsToTimeString(getTime()) + ")");
        debugMessage(">> State: Timelapse (" + secondsToTimeString(getTime()) + ")");
    }
   
}

void stopTimelapse() {
    timelapseState = TIMELAPSE_STATE_FINISHED;

    getSlideStepper()->stop();
    getPanStepper()->stop();
}

void startTimelapsePreview() {
    if(sliderState != STATE_TIMELAPSE) {
        timelapseState = TIMELAPSE_STATE_PREVIEW_START;
        sliderState = STATE_TIMELAPSE;  

        getSlideStepper()->stop();
        getPanStepper()->stop();
    } 
}

void stopTimelapsePreview() {
    if(sliderState == STATE_TIMELAPSE && 
        timelapseState >= TIMELAPSE_STATE_PREVIEW_START &&
        timelapseState <= TIMELAPSE_STATE_PREVIEW_STOP) {

        timelapseState = TIMELAPSE_STATE_PREVIEW_STOP;
        sliderState = STATE_TIMELAPSE;  
    } 
}

void setTimelapsePanSteps(float setSteps) {
    timelapsePanSteps = setSteps;
}

void setTimelapseSlideSteps(float setSteps) {
    timelapseSlideSteps = setSteps;
}

void setTimelapseInterval(gTime_t setInterval) {
    timelapseIntervalTime = setInterval;
}

void setTimelapsePictureCount(unsigned int setPictureCount) {
    timelapsePhotoCount = setPictureCount;
}

float getTimelapsePanSteps() { return timelapsePanSteps; }

float getTimelapseSlideSteps() { return timelapseSlideSteps; }

clock_t getTimelapseInterval() { return timelapseIntervalTime; }

clock_t getTimelapsePictureCount() { return timelapsePhotoCount; }


float getTimelapseTotalSlideDistance() {
    return (float)getTimelapsePictureCount() * getTimelapseSlideSteps();
}

float getTimelapseTotalPanDistance() {
    return (float)getTimelapsePictureCount() * getTimelapsePanSteps();
}

gTime_t getTimelapseTotalTime() {
    return getTimelapsePictureCount() * getTimelapseInterval();
}

gTime_t getTimelapseEndTime() {
    return getTime() + getTimelapseTotalTime();
}

gTime_t getTimelapseVideoTimePrediction(unsigned int frameRate) {
    return getTimelapsePictureCount() / frameRate;
}

bool isTimelapseInNortherHemisphere() {
    return timelapseInNortherHemisphere;
}

void setTimelapseHemisphere(bool inNorthernHemisphere) {
    timelapseInNortherHemisphere = inNorthernHemisphere;
}