#ifndef SmartStepper_H
#define SmartStepper_H

#include <Ticker.h>
#include <Arduino.h>
#include "common.h"
#include "debug.h"

typedef unsigned int pin_t;
typedef float steps_t;
typedef float turns_t;
typedef float angle_t;
typedef float speed_t;
typedef long accelaration_t;

enum SLEEPSTATE { SLEEP, AWAKE };
enum DIRECTION { NONE, FORWARD, BACKWARD };
enum MICROSTEPRESOLUTION { FULL, HALF, QUARTER, EIGHTH, SIXTEENTH };

class SmartStepper
{
    private:

        pin_t pinDirection;
        pin_t pinStep;
        pin_t pinSleep;

        SLEEPSTATE sleepState;

        DIRECTION direction;

        MICROSTEPRESOLUTION resolution;

        steps_t stepsPerTurn;
        steps_t steps;
        steps_t target_steps;

        speed_t speed;
        speed_t speedMin;

        static std::vector<SmartStepper*> steppers;
        static void INIT_TIMER(void);
        static void TICK(void);
        static const uint16_t TICK_INTERRUPT_FREQ = 2*10000;
        
        steps_t resolutionToSteps();
        uint16_t timerCount;


        void internTick();
        void doStep();
        void setTargetSteps(steps_t setTargetSteps);
        void setSleepState(SLEEPSTATE setState);

        
    public:

        SmartStepper(pin_t setDirectionPin, pin_t setStepPin, pin_t setSleepPin);
        ~SmartStepper();

        bool isSleeping() { return sleepState == SLEEPSTATE::SLEEP; }

        void setDirection(DIRECTION setDir) { direction = setDir; }
        
        void setResolution(MICROSTEPRESOLUTION setRes);

        void setStepsPerTurn(steps_t setSPT);

        void setSpeed(speed_t setSpeed);
        speed_t getSpeed() { return speed; }
        speed_t getMaxSpeed();

        turns_t getTurns() { return steps / stepsPerTurn; }

        angle_t getAngle() { return getTurns() * 360; }

        void rotate(angle_t angle);

        String getDebugMessage();

        void waitUntilTargetReached();
};

#endif