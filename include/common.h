/* I/O */
// Die möglichen Pin-Nummern.
// D1 = 5     D2 = 4     D3 = 0    D4 = 2
// TX = 1     RX = 3     D0 = 16   D5 = 14
// D6 = 12    D7 = 13    D8 = 15
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

#define DISABLE     HIGH
#define ENABLE      LOW

#define LONG_MAX 2147483647L
#define LONG_MIN -2147483648L

#define PI 3.1415

#define EARTH_SECONDS_PER_DAY 86164L
#define MOON_SECONDS_PER_TURN 2360622L

#define WD1 5
#define WD2 4
#define WD3 0
#define WD4 2
#define WTX 1
#define WRX 3
#define WD0 16
#define WD5 14
#define WD6 12
#define WD7 13
#define WD8 15

#define TIME_RESOLUTION 0.1

#define SLIDEANDPAN_NAME "Slide & Pan"
#define SLIDEANDPAN_VERSION "1.1.0.0"
#define SLIDEANDPAN_VERSION_INFO "Open alpha - Bug fix"
#define SLIDEANDPAN_COPYRIGHT "Copyright (c) Sebastian Knoll 2020"
#define SLIDEANDPAN_ID "S&P0001"
#define SLIDEANDPAN_ID_INFO "Special ID Info - First Slide & Pan device :)"

#define SLIDEANDPAN_LINK_HANDBOOK "https://sebastianknoll.net/slide"

#define COMMAND_HELP "help"
#define COMMAND_LIST_COMMANDS "commands"
#define COMMAND_GET_DEVICE_NAME "device_name"
#define COMMAND_GET_DEVICE_VERSION "device_version"
#define COMMAND_GET_DEVICE_COPYRIGHT "device_copyright"
#define COMMAND_GET_DEVICE_ID "device_id"
#define COMMAND_GET_DEVICE_NAME "device_name"

#define COMMAND_SET_TIME "set_time"
#define COMMAND_SET_DARKMODE "set_darkmode"

#define COMMAND_TIMELAPSE_START "timelapse_start"
#define COMMAND_TIMELAPSE_STOP "timelapse_stop"
#define COMMAND_TIMELAPSE_PREVIEW_START "timelapse_preview_start"
#define COMMAND_TIMELAPSE_PREVIEW_STOP "timelapse_preview_stop"
#define COMMAND_TIMELAPSE_PAN "timelapse_pan"
#define COMMAND_TIMELAPSE_SLIDE "timelapse_slide"
#define COMMAND_TIMELAPSE_COUNT "timelapse_count"
#define COMMAND_TIMELAPSE_PHOTO "timelapse_photo"
#define COMMAND_TIMELAPSE_INTERVAL "timelapse_interval"
#define COMMAND_TIMELAPSE_MILKYWAY_SETTINGS "timelapse_milkyway_settings"
#define COMMAND_TIMELAPSE_SET_HEMISPHERE "timelapse_set_hemisphere"

// For pan and slide equal - after '_' important for parsing...
#define COMMAND_SLIDE_SLEEP "slide_sleep"
#define COMMAND_SLIDE_SET_RESOLUTION "slide_res"
#define COMMAND_SLIDE_TO "slide_to"
#define COMMAND_SLIDE_SET_SPEED "slide_speed"
#define COMMAND_SLIDE_SET_ACC "slide_acc"
#define COMMAND_SLIDE_STOP "slide_stop"
#define COMMAND_SLIDE_LAZY "slide_lazy"
#define COMMAND_SLIDE_RESET "slide_reset"
#define COMMAND_SLIDE_PHYSICAL_VALUE "slide_physical_value"

#define PIN_LATCH WD5
#define PIN_CLOCK WD6
#define PIN_DATAOUT WD0
#define PIN_DATAIN WD7
#define PIN_LATCHIN WD8

#define ENCODER_PIN_CLK 5
#define ENCODER_PIN_DIRECTION 6
#define ENCODER_PIN_SWITCH 7

#define PIN_BATTERY A0
#define BATTERY_GREEN_MIN 400
#define BATTERY_ORANGE_MIN 350
#define BATTERY_RED_MIN 325
#define BATTERY_RED_BLINK_MIN 300
#define BATTERY_MIN 259
#define BATTERY_RECOVER 350

//#define PIN_LED_0_GREEN 0
#define PIN_LED_0_RED 9
#define PIN_LED_1_RED 10
#define PIN_LED_1_GREEN 11
#define PIN_LED_2_RED 12
#define PIN_LED_2_GREEN 13
#define PIN_LED_3_RED 14
#define PIN_LED_3_GREEN 15

#define PIN_CAM 8
#define CAM_STATE_IDLE 0
#define CAM_STATE_PREPARE 1
#define CAM_STATE_PHOTO 2
#define CAM_STATE_REST 3
#define CAM_TIME_EXPOSURE_DEFAULT 2.0
#define CAM_TIME_PREPARE 2.0
#define CAM_TIME_REST 2.0

#define STATE_IDLE 1
#define STATE_BETTERY_EMPTY 100
#define STATE_TIMELAPSE 2

#define TIMELAPSE_STATE_INIT 1
#define TIMELAPSE_STATE_SET_MOVE_GOAL 2
#define TIMELAPSE_STATE_MOVE 3
#define TIMELAPSE_STATE_TRIGGER_SHOOT 4
#define TIMELAPSE_STATE_SHOOT 5
#define TIMELAPSE_STATE_WAIT 6
#define TIMELAPSE_STATE_FINISHED 7
#define TIMELAPSE_STATE_PREVIEW_START 8
#define TIMELAPSE_STATE_PREVIEW_MOVE 9
#define TIMELAPSE_STATE_PREVIEW_SET_MOVE_GOAL 10
#define TIMELAPSE_STATE_PREVIEW_WAIT_AT_END 11
#define TIMELAPSE_STATE_PREVIEW_STOP 12

#define TIMELAPSE_INTERVAL_TIME_DEFAULT 60
#define TIMELAPSE_PHOT_COUNT_DEFAULT 300;
#define TIMELAPSE_PAN_STEPS_DEFAULT 0.25;
#define TIMELAPSE_SLIDE_STEPS_DEFAULT 20;

const String AVAILABLE_COMMANDS[] = {
        COMMAND_HELP,
        COMMAND_LIST_COMMANDS,
        COMMAND_GET_DEVICE_NAME, 
        COMMAND_GET_DEVICE_VERSION,
        COMMAND_GET_DEVICE_COPYRIGHT,
        COMMAND_GET_DEVICE_ID,
        COMMAND_GET_DEVICE_NAME,
        COMMAND_SET_TIME,
        COMMAND_SET_DARKMODE,
        COMMAND_SLIDE_SLEEP,
        COMMAND_SLIDE_SET_RESOLUTION,
        COMMAND_SLIDE_TO,
        COMMAND_SLIDE_SET_SPEED,
        COMMAND_SLIDE_SET_ACC,
        COMMAND_SLIDE_STOP,
        COMMAND_SLIDE_LAZY,
        COMMAND_SLIDE_RESET,
        COMMAND_SLIDE_PHYSICAL_VALUE,
        COMMAND_TIMELAPSE_STOP,
        COMMAND_TIMELAPSE_START,
        COMMAND_TIMELAPSE_PREVIEW_START,
        COMMAND_TIMELAPSE_PREVIEW_STOP,
        COMMAND_TIMELAPSE_MILKYWAY_SETTINGS,
        COMMAND_TIMELAPSE_PHOTO,
        COMMAND_TIMELAPSE_SLIDE,
        COMMAND_TIMELAPSE_PAN,
        COMMAND_TIMELAPSE_INTERVAL,
        COMMAND_TIMELAPSE_COUNT
    };


typedef unsigned int pin_t;
typedef float gTime_t;

void jsonAddField(String& json, String field, String value, String tabString, boolean wrapValue = false, boolean lastElement = false);

String getBasicDataJson(String tabString);


void setStreamDataFunction(std::function<void (String)> setFunction);
void tryStreamData(String data);

String getCommandHelp();
String getCommandList(String tabString);

String secondsToString(gTime_t valueInSecons);
String secondsToTimeString(gTime_t valueInSecons);

gTime_t getTime();
void incTimeByMinimumStep();
void setTime(gTime_t currentDaySeconds);

bool isSliderInDarkMode();
void setSliderDarkMode(bool value);

#endif
