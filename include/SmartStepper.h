#ifndef SmartStepper_H
#define SmartStepper_H

#include <Ticker.h>
#include <Arduino.h>
#include "common.h"
#include "debug.h"

typedef float steps_t;
typedef float turns_t;
typedef float angle_t;
typedef float speed_t;
typedef float acceleration_t;
typedef uint32_t time_count_t;

enum SLEEPSTATE { SLEEP, AWAKE };
enum DIRECTION { NONE, FORWARD, BACKWARD };
enum MICROSTEPRESOLUTION { FULL, HALF, QUARTER, EIGHTH, SIXTEENTH };

class SmartStepper
{
    private:

        String name;

        pin_t pinDirection;
        pin_t pinStep;
        pin_t pinSleep;

        SLEEPSTATE sleepState;

        DIRECTION direction;

        MICROSTEPRESOLUTION resolution;
        std::function<void (MICROSTEPRESOLUTION)> changeResolutionMethod;
        
        steps_t stepsPerTurn;
        steps_t steps;
        steps_t targetSteps;

        time_count_t speedModulo;

        speed_t speed;
        speed_t speedMin;
        speed_t targetSpeed;

        acceleration_t acceleration;
        acceleration_t targetAcceleration;

        static std::vector<SmartStepper*> steppers;
        static void INIT_TIMER(void);
        static void TICK(void);
        static const uint32_t TICK_INTERRUPT_FREQ = 10000;
        static time_count_t tickDeltaCount;

        time_count_t timerCount;

        void internTick();
        void setSpeedModulo();
        void doStep();
        void setAcceleration();
        void setSpeed(speed_t setSpeed);
        void setIdealResolutionIfPossible();
        MICROSTEPRESOLUTION calculateIdealResolutionFromSpeed();

        String hardwareJson(String tabString);
        String postionJson(String tabString);
        String speedJson(String tabString);
        String accJson(String tabString);

        bool lazyMode;

        float physicalValuePerStep;
        String physicalValueUnit;
        String physicalValue;
        
    public:

        SmartStepper(pin_t setDirectionPin, pin_t setStepPin, pin_t setSleepPin);
        ~SmartStepper();

        bool isSleeping() { return sleepState == SLEEPSTATE::SLEEP; }
        void setSleepState(SLEEPSTATE setState);

        void setTargetSteps(steps_t setTargetSteps);

        DIRECTION getDirection() { return direction; }
        void setDirection(DIRECTION setDir);
        
        MICROSTEPRESOLUTION getResolution() { return resolution; }
        steps_t resolutionToSteps();
        void setResolution(MICROSTEPRESOLUTION setRes);
        void setResolutionFromInteger(int setRes);

        bool hasChangeResolutionMethod() { return changeResolutionMethod != nullptr; }
        void setChangeResolutionMethod(std::function<void (MICROSTEPRESOLUTION)> setChangeResolutionMethod);

        steps_t getSteps() { return steps; }
        steps_t getTargetSteps() { return targetSteps; }
        bool isGoalReached() { return steps == targetSteps; }

        void incTargetSteps(steps_t deltaSteps) { setTargetSteps(steps + deltaSteps); }

        steps_t getStepsPerTurn() { return stepsPerTurn; }
        void setStepsPerTurn(steps_t setSPT);

        void stop() { targetSteps = steps; }

        speed_t getTargetSpeed() { return targetSpeed; }
        void setTargetSpeed(speed_t setTargetSpeed);
        speed_t getSpeed() { return speed; }
        speed_t getMaxSpeed();

        acceleration_t getTargetAcceleration() { return targetAcceleration; }
        void setTargetAcceleration(acceleration_t setAcc);

        turns_t getTurns() { return steps / stepsPerTurn; }

        angle_t getAngle() { return getTurns() * 360; }

        void rotate(angle_t angle);
        void rotateToAbsoluteAngle(angle_t angle);

        String getDebugMessage();

        void waitUntilTargetReached();

        String toJson(String myName, String tabString);
        String streamData(String myName);

        void setName(String setName) { name = setName; }
        String getName() { return name; }

        bool isInLazyMode() { return lazyMode; }
        void setLazyMode(bool setMode) { lazyMode = setMode; }

        void setPhysicalValueInfos(String value, String unit);
        String getPhysicalValueUnit() { return physicalValueUnit; }
        String getPhysicalValueInfo(bool withValuePraefix = false);
        void setPhysicalValueRatio(float ratioPhysicalValuePerStep) { physicalValuePerStep = ratioPhysicalValuePerStep; }
        float getPhysicalValue() { return physicalValuePerStep * getSteps(); }
        float physicalValueToSteps(float value) { return value / physicalValuePerStep; }
        void setTargetPhysicalValue(float value) { setTargetSteps(physicalValueToSteps(value)); }
        String getPhysicalValueDescription() { return physicalValue; }
        void incPhysicalValue(steps_t deltaValue) { setTargetPhysicalValue(getPhysicalValue() + deltaValue); }


        void resetStepsToZero() { targetSteps = 0; steps = 0;}
};

#endif