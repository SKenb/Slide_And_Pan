from os import listdir
from os.path import isfile, join


def getFiles():
    extensions = ["html", "js", "css"]
    return [f for f in listdir(".") if isfile(f) and 
            "." in  f and 
            "_min" not in f and
            f.split(".")[-1] in extensions]

TO_REPLACE = {  "%HEAD%": """
                        <head>
                        <meta charset='utf-8'>

                        <meta charset='utf-8'>
                        <meta name='viewport' content='width=device-width, initial-scale=1.0'>

                        <title>Slide and Pan</title>
                        <meta name='description' content='Slide and Pan'>
                        <meta name='author' content='Sebastian Knoll'>

                        </head>
                    """,
                    "%STYLE%": """
                    <style>
                        * {
                            font-family:courier,arial,helvetica;
                        }

                        body {
                            background-color: rgb(180, 180, 180);
                        }

                        li {
                            display:inline-block;
                            margin: 5px;
                        }

                        ul {
                            margin: 0 auto;
                            display: inherit;
                        }

                        h1, h2, h3 {
                            text-align: center;
                        }

                        #container {
                            width: 100%;
                            max-width: 960px;
                            margin: 0 auto;
                        }

                        .section {

                            background: whitesmoke;
                            border-radius: 20px;

                            padding: 20px;
                        }

                        input {
                            width: 50px;
                        }

                        .set_button {
                            width: 200px;
                        }

                        .block {
                            display: block;
                            width: 100%;
                            height: 50px;;
                        }

                    </style>
                    """,
                    "%SCRIPT%": """
                    <script>

                        function makeRequest(command) {
                            var url = '/device/command/?' + command;
                            
                            var request = new XMLHttpRequest();

                            request.onreadystatechange = function() {
                                if (this.readyState == 4 && this.status == 200) {
                                    location.reload();
                                }
                            };

                            request.open('GET',url);
                            request.send();
                        }

                        function getDaytimeInSeconds() {
                            let now = new Date();
                            return now.getSeconds() + 60 * now.getMinutes() + 60*60* now.getHours(); 
                        }

                        var updateTimeRequest = new XMLHttpRequest();
                        updateTimeRequest.open('GET', '/device/command/?set_time=' + getDaytimeInSeconds());
                        updateTimeRequest.send();

                    </script>
                    """,
                    "%PAN%": "\" + String(getTimelapsePanSteps()) + \"",
                    "%SLIDE%": "\" + String(getTimelapseSlideSteps()) + \"",
                    "%PC%": "\" + String(getTimelapsePictureCount()) + \"",
                    "%EXP%": "\" + String(getIOBoard()->getCameraExposureTime()) + \"",
                    "%INT%": "\" + String(getTimelapseInterval()) + \"",
                    "%SLIDE%": "\" + String(getTimelapseSlideSteps()) + \"",
                    "%SAP_NAME%": "\" + String(SLIDEANDPAN_NAME) + \"",
                    "%SAP_VERSION%": "\" + String(SLIDEANDPAN_VERSION) + \"",
                    "%SAP_VERSION_INFO%": "\" + String(SLIDEANDPAN_VERSION_INFO) + \"",
                    "%SAP_CR%": "\" + String(SLIDEANDPAN_COPYRIGHT) + \"",
                    "%SAP_ID%": "\" + String(SLIDEANDPAN_ID) + \"",
                    "%SLIDEANDPAN_ID_INFO%": "\" + String(SLIDEANDPAN_ID_INFO) + \"",
                    "%HANDBOOK%": "\" + String(SLIDEANDPAN_LINK_HANDBOOK) + \"",
                    "%S_SLEEP%": "\" + String(getSlideStepper()->isSleeping() ? \"Sleeping\" : \"Awake\") + \"",
                    "%S_NSLEEP%": "\" + String(getSlideStepper()->isSleeping() ? \"Awake\" : \"Sleep\") + \"",
                    "%S_INSLEEP%": "\" + String(getSlideStepper()->isSleeping() ? 1 : 0) + \"",
                    "%S_STEPS%": "\" + String(getSlideStepper()->getSteps()) + \"",
                    "%S_TSTEPS%": "\" + String(getSlideStepper()->getTargetSteps()) + \"",
                    "%S_SPEED%": "\" + String(getSlideStepper()->getSpeed()) + \"",
                    "%S_TSPEED%": "\" + String(getSlideStepper()->getTargetSpeed()) + \"",
                    "%S_RES%": "\" + String(getSlideStepper()->getResolution()) + \"",
                    "%S_LAZY%": "\" + String(getSlideStepper()->isInLazyMode() ? \"Yes\" : \"No\") + \"",
                    "%S_NLAZY%": "\" + String(getSlideStepper()->isInLazyMode() ? \"Go Strong\" : \"Go Lazy\") + \"",
                    "%S_INLAZY%": "\" + String(getSlideStepper()->isInLazyMode() ? 0 : 1) + \"",
                    "%S_PHY_VALUE_DES%": "\" + (getSlideStepper()->getPhysicalValueDescription()) + \"",
                    "%S_PHY_VALUE%": "\" + (getSlideStepper()->getPhysicalValueInfo()) + \"",
                    "%S_PHY_VALUE_UNIT%": "\" + (getSlideStepper()->getPhysicalValueUnit()) + \"",
                    "%S_DIR_PIN%": "\" + String(SLIDE_STEPPER_PIN_DIRECTION) + \"",
                    "%S_STEP_PIN%": "\" + String(SLIDE_STEPPER_PIN_STEP) + \"",
                    "%S_SLEEP_PIN%": "\" + String(SLIDE_STEPPER_PIN_SLEEP) + \"",
                    "%S_STEPS_PER_TURN%": "\" + String(STEPPER_STEPS_PER_TURN) + \"",
                    "%S_CAN_CHANGE_RES%": "\" + String(getSlideStepper()->hasChangeResolutionMethod() ? \"Yes\" : \"No\") + \"",
                    "%S_DRIVER_M1%": "\" + String(SLIDE_STEPPER_DRIVER_M1) + \"",
                    "%S_DRIVER_M2%": "\" + String(SLIDE_STEPPER_DRIVER_M2) + \"",
                    "%S_DRIVER_M3%": "\" + String(SLIDE_STEPPER_DRIVER_M3) + \"",
                    "%P_SLEEP%": "\" + String(getPanStepper()->isSleeping() ? \"Sleeping\" : \"Awake\") + \"",
                    "%P_NSLEEP%": "\" + String(getPanStepper()->isSleeping() ? \"Awake\" : \"Sleep\") + \"",
                    "%P_INSLEEP%": "\" + String(getPanStepper()->isSleeping() ? 1 : 0) + \"",
                    "%P_STEPS%": "\" + String(getPanStepper()->getSteps()) + \"",
                    "%P_TSTEPS%": "\" + String(getPanStepper()->getTargetSteps()) + \"",
                    "%P_SPEED%": "\" + String(getPanStepper()->getSpeed()) + \"",
                    "%P_TSPEED%": "\" + String(getPanStepper()->getTargetSpeed()) + \"",
                    "%P_RES%": "\" + String(getPanStepper()->getResolution()) + \"",
                    "%P_LAZY%": "\" + String(getPanStepper()->isInLazyMode() ? \"Yes\" : \"No\") + \"",
                    "%P_NLAZY%": "\" + String(getPanStepper()->isInLazyMode() ? \"Go Strong\" : \"Go Lazy\") + \"",
                    "%P_INLAZY%": "\" + String(getPanStepper()->isInLazyMode() ? 0 : 1) + \"",
                    "%P_PHY_VALUE_DES%": "\" + (getPanStepper()->getPhysicalValueDescription()) + \"",
                    "%P_PHY_VALUE%": "\" + (getPanStepper()->getPhysicalValueInfo()) + \"",
                    "%P_PHY_VALUE_UNIT%": "\" + (getPanStepper()->getPhysicalValueUnit()) + \"",
                    "%P_DIR_PIN%": "\" + String(PAN_STEPPER_PIN_DIRECTION) + \"",
                    "%P_STEP_PIN%": "\" + String(PAN_STEPPER_PIN_STEP) + \"",
                    "%P_SLEEP_PIN%": "\" + String(PAN_STEPPER_PIN_SLEEP) + \"",
                    "%P_STEPS_PER_TURN%": "\" + String(STEPPER_STEPS_PER_TURN) + \"",
                    "%P_CAN_CHANGE_RES%": "\" + String(getPanStepper()->hasChangeResolutionMethod() ? \"Yes\" : \"No\") + \"",
                    "%P_DRIVER_M1%": "\" + String(PAN_STEPPER_DRIVER_M1) + \"",
                    "%P_DRIVER_M2%": "\" + String(PAN_STEPPER_DRIVER_M2) + \"",
                    "%P_DRIVER_M3%": "\" + String(PAN_STEPPER_DRIVER_M3) + \"",  
                    "%SLIDE_GEAR_DIAMETER%": "\" + String(SLIDE_GEAR_DIAMETER) + \"",
                    "%PAN_GEAR_SMALL_COG_NUMBER%": "\" + String(PAN_GEAR_SMALL_COG_NUMBER) + \"",
                    "%PAN_GEAR_LARGE_COG_NUMBER%": "\" + String(PAN_GEAR_LARGE_COG_NUMBER) + \"",
                    "%PIN_LATCH%": "\" + String(PIN_LATCH) + \"",
                    "%PIN_CLOCK%": "\" + String(PIN_CLOCK) + \"",
                    "%PIN_DATAOUT%": "\" + String(PIN_DATAOUT) + \"",
                    "%PIN_DATAIN%": "\" + String(PIN_DATAIN) + \"",
                    "%PIN_LATCHIN%": "\" + String(PIN_LATCHIN) + \"",
                    "%ENCODER_PIN_CLK%": "\" + String(ENCODER_PIN_CLK) + \"",
                    "%ENCODER_PIN_DIRECTION%": "\" + String(ENCODER_PIN_DIRECTION) + \"",
                    "%ENCODER_PIN_SWITCH%": "\" + String(ENCODER_PIN_SWITCH) + \"",
                    "%PIN_BATTERY%": "\" + String(PIN_BATTERY) + \"",
                    "%EARTH_TIME_PER_DAY%": "\" + secondsToString(EARTH_SECONDS_PER_DAY) + \"",
                    "%MOON_TIME_PER_TURN%": "\" + secondsToString(MOON_SECONDS_PER_TURN) + \"",
                    "%TIMELAPSE_TOTAL_TIME%": "\" + secondsToString(getTimelapseTotalTime()) + \"",
                    "%TIMELAPSE_END_TIME%": "\" + secondsToTimeString(getTimelapseEndTime()) + \"",
                    "%TIMELAPSE_VIDEO_LENGTH_24FPS%": "\" + secondsToString(getTimelapseVideoTimePrediction(24)) + \"",
                    "%TIMELAPSE_VIDEO_LENGTH_25FPS%": "\" + secondsToString(getTimelapseVideoTimePrediction(25)) + \"",
                    "%TIMELAPSE_VIDEO_LENGTH_30FPS%": "\" + secondsToString(getTimelapseVideoTimePrediction(30)) + \"",
                    "%TIMELAPSE_VIDEO_LENGTH_60FPS%": "\" + secondsToString(getTimelapseVideoTimePrediction(60)) + \"",
                    "%TIMELAPSE_TOTAL_SLIDE%": "\" + String(getTimelapseTotalSlideDistance()) + \"",
                    "%TIMELAPSE_TOTAL_PAN%": "\" + String(getTimelapseTotalPanDistance()) + \"",
                    "%TIME%": "\" + secondsToTimeString(getTime()) + \"",
                    "%DARK_MODE_NV%": "\" + String(isSliderInDarkMode() ? 0 : 1) + \"", 
                    "%DARK_MODE%": "\" + (isSliderInDarkMode() ? \"is On\" : \"is Off\") + \"",
                    "%LOGS%": "\" + getRingBufferForWeb() + \"",
                    "%BATTERY_LVL%": "\" + String(getIOBoard()->getBatteryLevel()) + \"",
                    "%TIMELAPSE_HEMISPHERE%": "\" + (isTimelapseInNortherHemisphere() ? \"Northern\" : \"South\") + \"",
                    "%TIMELAPSE_HEMISPHERE_NV%": "\" + String(!isTimelapseInNortherHemisphere() ? 1 : 0) +  \""
                }
def convert(file):
    global fileContent

    if not "." in file:
        print("Convert {} failed (Ext not found) ".center(80, "-"))
        return 

    print(" Convert {} ".format(file).center(80, "-"))
    content = open(file, "r").read()

    content = content.replace("'", "**R**").replace("\"", "'").replace("**R**", "\\\"")
    for tore in TO_REPLACE: content = content.replace(tore, TO_REPLACE[tore])
    content = content.replace("\t", " ").replace("\n", "\");\n\tserver.sendContent(\"").replace("  ", " ")


    filename = file.split(".")[0:-1]
    fileContent += "\n\nvoid " + filename[0].capitalize() + "PageHandler(ESP8266WebServer& server) {\n"
    fileContent += "\tserver.sendContent(\"" + content
    fileContent = fileContent.replace("\"\"", "\"\\n\"")
    fileContent += "\");\n}"

def initFileContent():
    global fileContent

    fileContent = """
#ifndef HTML_FILES_H
#define HTML_FILES_H

#include "common.h"
#include "io.h"
#include "stepper.h"
#include "debug.h"
    """

def writeFile():
    global fileContent

    fileContent += "\n\n#endif"
    
    newFile = open("htmlFiles.h", "w+")
    newFile.write(fileContent)
    newFile.close()

def main():
    initFileContent()

    files = getFiles()

    print(" Files: ".center(80, "-"))
    print(files)

    for file in files: convert(file)

    writeFile()


if __name__ == "__main__":
    main()