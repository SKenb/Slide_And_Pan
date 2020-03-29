#include "common.h"
#include "stepper.h"

STEPPER steppers[2] = {{     "A", 
                            STEPPER_A_DIR_PIN, STEPPER_A_STEP_PIN, 
                            STEPPER_A_SLEEP_PIN, 
                            0, 
                            false, 
                            CLKW, 
                            STEPPER_A_SPEED_INDEX_MAX, 
                            STEPPER_A_SPEED_INDEX_MIN, 
                            STEPPER_A_SPEED_INDEX_MAX, 
                            STEPPER_A_SPEED_INDEX_MIN, 
                            0, 0, 0,
                            STEPPER_DEFAULT_ACCELERATION_WINDOW,
                            STEPPER_ACCELARATION_MODE_NONE,
                            STEPPER_A_DEGREE_PER_STEP, 
                            0, 
                            0, 
                            false, 
                            0, 
                            false 
                        },
                        {   "B", 
                            STEPPER_B_DIR_PIN, 
                            STEPPER_B_STEP_PIN, 
                            STEPPER_B_SLEEP_PIN, 
                            0, 
                            false, 
                            CLKW, 
                            STEPPER_A_SPEED_INDEX_MAX, 
                            STEPPER_A_SPEED_INDEX_MIN, 
                            STEPPER_A_SPEED_INDEX_MAX, 
                            STEPPER_A_SPEED_INDEX_MIN, 
                            0, 0, 0,
                            STEPPER_DEFAULT_ACCELERATION_WINDOW,
                            STEPPER_ACCELARATION_MODE_NONE,
                            STEPPER_B_DEGREE_PER_STEP, 
                            0,
                            0, 
                            false, 
                            0, 
                            false 
                        }
                    };


void initSteppers() {
    for(int index = 0; index < STEPPER_COUNT; index++) {    
        pinMode(steppers[index].pin_dir, OUTPUT);
        pinMode(steppers[index].pin_step, OUTPUT);
        pinMode(steppers[index].pin_sleep, OUTPUT);

        stepperSleep(index, true);
        stepperSetDirection(index, CLKW);
        digitalWrite(steppers[index].pin_step, LOW);
    } 
}

void eStop() {
    for(int index = 0; index < STEPPER_COUNT; index++) {
        stepperDoSteps(index, 0);
        stepperSleep(index, false);
    }
}

void stepperGoTo(int stepperIndex, long position) {
    long distance = (steppers[stepperIndex].position - position);

    if(distance < 0) {
        distance *= -1;
        stepperSetDirection(stepperIndex, CLKW);
    }
    else {
        stepperSetDirection(stepperIndex, CONTERCLKW);
    }

    stepperDoSteps(stepperIndex, distance);
}

void stepperDoStep(int stepperIndex) {   
    stepperSleep(stepperIndex, false);

    steppers[stepperIndex].position += ((steppers[stepperIndex].direction == CLKW) ? 1 : -1);
    
    digitalWrite(steppers[stepperIndex].pin_step , LOW); 
    digitalWrite(steppers[stepperIndex].pin_step , HIGH);// perform one step

    stepperSetSpeed(stepperIndex, steppers[stepperIndex].speed_index + steppers[stepperIndex].acceleration);
    stepperUpdateAcceleration(stepperIndex);
}

void stepperDoSteps(int stepperIndex, long steps) {
    stepperAccelerationInit(stepperIndex);
    steppers[stepperIndex].steps_to_drive = steps;
}

void stepperRotate(int stepperIndex, long degree) {
    long steps = degree * 1000000 / steppers[stepperIndex].udegree_per_step;
    stepperDoSteps(stepperIndex, steps);
}

void stepperSleep(bool stepperIndex, bool sleep) {
    steppers[stepperIndex].sleep = sleep;
    digitalWrite(steppers[stepperIndex].pin_sleep, sleep ? LOW : HIGH);
}

void stepperSetDirection(int stepperIndex, int dir) {
    if(steppers[stepperIndex].direction != dir) {
        stepperAccelerationInit(stepperIndex);
    }

    steppers[stepperIndex].direction = dir;
    digitalWrite(steppers[stepperIndex].pin_dir, (dir == CLKW) ? LOW : HIGH);
}

void stepperSetSpeed(int stepperIndex, unsigned int speed_index) {
    if(speed_index > steppers[stepperIndex].speed_index_max) speed_index = steppers[stepperIndex].speed_index_max;
    if(speed_index < steppers[stepperIndex].speed_index_min) speed_index = steppers[stepperIndex].speed_index_min;
    
    steppers[stepperIndex].speed_index = speed_index;
}

void stepperSetTargetSpeed(int stepperIndex, unsigned int speed_index) {
    if(speed_index > steppers[stepperIndex].speed_index_max) speed_index = steppers[stepperIndex].speed_index_max;
    if(speed_index < steppers[stepperIndex].speed_index_min) speed_index = steppers[stepperIndex].speed_index_min;
    
    steppers[stepperIndex].speed_target_index = speed_index;
}

void stepperAccelerationInit(int stepperIndex) {
    steppers[stepperIndex].acceleration = 0;
    steppers[stepperIndex].acceleration_start_speed_index = steppers[stepperIndex].speed_index;
    steppers[stepperIndex].acceleration_mode = STEPPER_ACCELARATION_MODE_RISE;
}
    

void stepperUpdateAcceleration(int stepperIndex) {
    float acc = (float)steppers[stepperIndex].speed_index;
    acc /= steppers[stepperIndex].speed_index_max;

    int speed_delta = steppers[stepperIndex].speed_index - steppers[stepperIndex].speed_target_index;

    steppers[stepperIndex].acceleration_pitch = acc;
    steppers[stepperIndex].acceleration_window = speed_delta;

    if(speed_delta > 0) {
        steppers[stepperIndex].acceleration = -10 * acc;
    }
    else if(speed_delta < 0) {
        steppers[stepperIndex].acceleration = 10 * acc;
    }
    else {
        stepperAccelerationStop(stepperIndex);
    }
}

void stepperAccelerationStop(int stepperIndex) {
    steppers[stepperIndex].acceleration = 0;
    steppers[stepperIndex].acceleration_pitch = 0;
    
    steppers[stepperIndex].acceleration_mode = STEPPER_ACCELARATION_MODE_NONE;

    stepperSetSpeed(stepperIndex, steppers[stepperIndex].speed_target_index);
}

void stepperSetAccWindow(int stepperIndex, float accWindow) {
    steppers[stepperIndex].acceleration_window = accWindow;
}


String stepperSetAccModeToString(int accMode) {
    if(accMode == STEPPER_ACCELARATION_MODE_NONE) return "NONE";
    if(accMode == STEPPER_ACCELARATION_MODE_FALL) return "FALL";
    if(accMode == STEPPER_ACCELARATION_MODE_RISE) return "RISE";
    if(accMode == STEPPER_ACCELARATION_MODE_CONST) return "CONST";

    return "UNKOWN?!";
}

// 0% - 100%
unsigned int getSpeedPercentage(int stepperIndex, unsigned int speed_index) {
    unsigned int index = steppers[stepperIndex].speed_index_min;
    index = speed_index - index;

    unsigned int delta = steppers[stepperIndex].speed_index_max;
    delta -= steppers[stepperIndex].speed_index_min;

    return (unsigned int)(1000 - ((index * 1000) / delta));
}

String stepperJsonStatus(int stepperIndex) {
    String state = "\"Stepper\": \"";
    state += steppers[stepperIndex].id;
    state += "\", \n";
    
    state += "\"Index\": \"";
    state += String(stepperIndex);
    state += "\", \n";
    
    state += "\"Position\": \"";
    state += String(steppers[stepperIndex].position);
    state += "\", \n";

    state += "\"Steps\": \"";
    state += String(steppers[stepperIndex].steps_to_drive);
    state += "\", \n";
    
    state += "\"Direction\": \"";
    state += (steppers[stepperIndex].direction == CLKW) ? "Clockwise" : "Counter clockwise";
    state += "\", \n";
    
    state += "\"Direction-Index\": \"";
    state += (steppers[stepperIndex].direction == CLKW) ? "0" : "1";
    state += "\", \n";

    state += "\"Speed-Index\": \"";
    state += String(steppers[stepperIndex].speed_index);
    state += "\", \n";

    state += "\"Target-Speed-Index\": \"";
    state += String(steppers[stepperIndex].speed_target_index);
    state += "\", \n";

    state += "\"Speed-Index Min\": \"";
    state += String(steppers[stepperIndex].speed_index_min);
    state += "\", \n";

    state += "\"Speed-Index Max\": \"";
    state += String(steppers[stepperIndex].speed_index_max);
    state += "\", \n";

    state += "\"Start\": \"";
    state += String(steppers[stepperIndex].point_start);
    state += "\", \n";

    state += "\"Start-Valid\": \"";
    state += steppers[stepperIndex].point_start_valid ? "true" : "false";
    state += "\", \n";

    state += "\"End\": \"";
    state += String(steppers[stepperIndex].point_end);
    state += "\", \n";

    state += "\"End-Valid\": \"";
    state += steppers[stepperIndex].point_end_valid ? "true" : "false";
    state += "\", \n";
    
    state += "\"Speed-Percentage\": \"";
    state += String(getSpeedPercentage(stepperIndex, steppers[stepperIndex].speed_index));
    state += "\", \n";

    state += "\"Acceleration\": \"";
    state += String(steppers[stepperIndex].acceleration);
    state += "\", \n";
 
    state += "\"Acceleration-Start-Speed-Index\": \"";
    state += String(steppers[stepperIndex].acceleration_start_speed_index);
    state += "\", \n";

    state += "\"Acceleration-Pitch\": \"";
    state += String(steppers[stepperIndex].acceleration_pitch);
    state += "\", \n";

    state += "\"Acceleration-Window\": \"";
    state += String(steppers[stepperIndex].acceleration_window);
    state += "\", \n";

    state += "\"Acceleration-Mode\": \"";
    state += stepperSetAccModeToString(steppers[stepperIndex].acceleration_mode);
    state += "\", \n";

    state += "\"Sleep\": \"";
    state += (steppers[stepperIndex].sleep ? "1" : "0");
    state += "\" \n";

    return state;
}

String stepperControl(int stepperIndex, String paramName, String paramValue) {
    String result = "\"not known command\"";
    String motorString = (stepperIndex == STEPPER_A) ? "Stepper A" : "Stepper B";
    
    if(paramName == "sleep") 
    {
        stepperSleep(stepperIndex, paramValue != "0");
        result = "\"" + motorString;
        result += (paramValue != "0" ? " put to bed" : " waked up");
        result += "\"";
    }

    if(paramName == "steps") 
    {
        stepperDoSteps(stepperIndex, paramValue.toInt());
        result = "\"" + motorString + " do " + paramValue + " steps" + "\"";
    }

    if(paramName == "degree") 
    {
        stepperRotate(stepperIndex, paramValue.toInt());
        result = "\"" + motorString + " rotate " + paramValue + " degree(s)" + "\"";
    }

    if(paramName == "accwindow") 
    {
        stepperSetAccWindow(stepperIndex, paramValue.toInt());
        result = "\"" + motorString + " Acceleration Window set to: " + paramValue + "\"";
    }
    
    if(paramName == "speed" || paramName == "realspeed") 
    {
        unsigned int value = 0;
        if(paramValue == "inc"){
            double t_value = (double)steppers[stepperIndex].speed_target_index;
            value = (unsigned int)(t_value * 0.9);
        }
        else if(paramValue == "dec") {
            double t_value = (double)steppers[stepperIndex].speed_target_index;
            value = (unsigned int)(t_value * 1.15);
        }
        else if(paramValue == "max") {
            value = steppers[stepperIndex].speed_index_min;
        }
        else if(paramValue == "min") {
            value = steppers[stepperIndex].speed_index_max;
        }
        else if(paramValue == "toggle") {
            unsigned int speed = steppers[stepperIndex].speed_target_index;

        if(speed >= steppers[stepperIndex].speed_index_max){
            value = steppers[stepperIndex].speed_index_min;
        }
        else if(speed <= steppers[stepperIndex].speed_index_min) {
            value = steppers[stepperIndex].speed_index_max;
        }
        else {
            value = ((steppers[stepperIndex].speed_index_max - speed) > (steppers[stepperIndex].speed_index_min - speed)) ? steppers[stepperIndex].speed_index_min : steppers[stepperIndex].speed_index_max;
        }
        }
        else {
            value = paramValue.toInt();
        }

        if(paramName == "speed") {
            stepperSetTargetSpeed(stepperIndex, value);
            result = "\"" + motorString + " set (real) speed to " + String(steppers[stepperIndex].speed_index) + " (in limits)" + "\"";
        }
        else {
            stepperSetSpeed(stepperIndex, value);
            result = "\"" + motorString + " set target speed to " + String(steppers[stepperIndex].speed_index) + " (in limits)" + "\"";
        }
    }

    if(paramName == "move") 
    {
        stepperDoSteps(stepperIndex, LONG_MAX);
        result = "\"" + motorString + " start moving \"";
    }

    if(paramName == "direction" || paramName == "dir") 
    {
        stepperSetDirection(stepperIndex, paramValue == "0");
        result = "\"" + motorString + " direction set to " + (paramValue == "0" ? "counterclockwise" : "clockwise") + "\"";
    }

    if(paramName == "reset") 
    {
        steppers[stepperIndex].position = 0;
        
        for(int index = 0; index < STEPPER_COUNT; index++) {
        steppers[index].point_start_valid = false;
        steppers[index].point_start = 0;
        steppers[index].point_end_valid = false;
        steppers[index].point_end = 0;
        }
        
        result = "\"" + motorString + " position(s) reseted\"";
    }

    if(paramName == "start") 
    {
        steppers[stepperIndex].point_start_valid = true;
        steppers[stepperIndex].point_start = steppers[stepperIndex].position;
        
        result = "\"" + motorString + " start set to " + String(steppers[stepperIndex].point_start);
        if((steppers[stepperIndex].steps_to_drive > 0)) result +=  " (WARNING: stepper still moving)";
        result += "\"";
    }

    if(paramName == "halt") 
    {
        stepperDoSteps(stepperIndex, 0);
        result = "\"" + motorString + " halted/stopped \"";
    }

    if(paramName == "tostart") 
    {
        if(steppers[stepperIndex].point_start_valid) {
            stepperGoTo(stepperIndex, steppers[stepperIndex].point_start);
            result = "\"" + motorString + " to start\"";
        }
        else
        {
            result = "\"" + motorString + " to start ignored - start not valid\"";
        }
    }

    if(paramName == "toend") 
    {
        if(steppers[stepperIndex].point_end_valid) {
            stepperGoTo(stepperIndex, steppers[stepperIndex].point_end);
            result = "\"" + motorString + " to end\"";
        }
        else
        {
            result = "\"" + motorString + " to end ignored - end not valid\"";
        }
    }

    if(paramName == "end") 
    {
        steppers[stepperIndex].point_end_valid = true;
        steppers[stepperIndex].point_end = steppers[stepperIndex].position;
        
        result = "\"" + motorString + " end set to " + String(steppers[stepperIndex].point_end);
        if((steppers[stepperIndex].steps_to_drive > 0)) result +=  " (WARNING: stepper still moving)";
        result += "\"";
    }

    return result;
}