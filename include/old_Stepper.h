#include <Arduino.h>

#define CLKW        0
#define CONTERCLKW  1

#define STEPPER_A 0
#define STEPPER_B 1
#define STEPPER_COUNT 2

#define STEPPER_MODUS_DRIVER_BOARD 1

#define STEPPER_ACCELARATION_MODE_NONE 0
#define STEPPER_ACCELARATION_MODE_RISE 1
#define STEPPER_ACCELARATION_MODE_FALL 2
#define STEPPER_ACCELARATION_MODE_CONST 3

#define STEPPER_A_DIR_PIN 5
#define STEPPER_A_STEP_PIN 4
#define STEPPER_A_SLEEP_PIN 0

#define STEPPER_B_DIR_PIN 13
#define STEPPER_B_STEP_PIN 15
#define STEPPER_B_SLEEP_PIN 12

#define STEPPER_A_DEGREE_PER_STEP 112500 // u°/Step
#define STEPPER_B_DEGREE_PER_STEP 112500 // u°/Step

#define STEPPER_A_SPEED_INDEX_MAX 8008 // Slowest
#define STEPPER_A_SPEED_INDEX_MIN 80 // Fastest

#define STEPPER_DEFAULT_ACCELERATION_WINDOW 1000

#define TIMER_STEPPER_PERIOD 1L // [µsec] time to next interrupt

/* Stepper */
typedef struct {
    String id;

    int pin_dir;
    int pin_step;
    int pin_sleep;

    long position;
    bool sleep;
    int direction;

    unsigned int speed_index;
    unsigned int speed_target_index;
    unsigned int speed_index_max;
    unsigned int speed_index_min; 

    float acceleration;
    unsigned int acceleration_start_speed_index;
    float acceleration_pitch;
    unsigned int acceleration_window;
    unsigned int acceleration_mode;

    int udegree_per_step;
    unsigned long steps_to_drive;

    long point_start;
    bool point_start_valid;

    long point_end;
    bool point_end_valid;
} STEPPER;

extern STEPPER steppers[2];

void initSteppers();
void eStop();
void stepperGoTo(int stepperIndex, long position);
void stepperDoStep(int stepperIndex);
void stepperDoSteps(int stepperIndex, long steps);
void stepperRotate(int stepperIndex, long degree);
void stepperSleep(bool stepperIndex, bool sleep);
void stepperSetDirection(int stepperIndex, int dir);
void stepperSetSpeed(int stepperIndex, unsigned int speed_index);
void stepperSetTargetSpeed(int stepperIndex, unsigned int speed_index);
unsigned int getSpeedPercentage(int stepperIndex, unsigned int speed_index);
void stepperAccelerationInit(int stepperIndex);
void stepperUpdateAcceleration(int stepperIndex);
void stepperAccelerationStop(int stepperIndex);
void stepperSetAccWindow(int stepperIndex, float accWindow);
String stepperSetAccModeToString(int accMode);
String stepperJsonStatus(int stepperIndex);
String stepperControl(int stepperIndex, String paramName, String paramValue);