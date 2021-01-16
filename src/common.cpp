#include "common.h"

std::function<void (String)> streamDataWarpperFunction = nullptr;
gTime_t daySeconds = 0;
bool sliderDarkMode = true;

void jsonAddField(String& json, String field, String value, String tabString, boolean wrapValue, boolean lastElement) {
    if(wrapValue) value = "\"" + value + "\"";

    json += tabString + "\"" + field + "\": " + value + (lastElement ? "\n" : ", \n");
}

String getBasicDataJson(String tabString) {
    String json = "{\n";

    tabString = "\t";
    jsonAddField(json, "Name", SLIDEANDPAN_NAME, tabString, true);
    jsonAddField(json, "ID", SLIDEANDPAN_ID, tabString, true);
    jsonAddField(json, "Copyright", SLIDEANDPAN_COPYRIGHT, tabString, true);
    jsonAddField(json, "Version", SLIDEANDPAN_VERSION, tabString, true, true);
    
    return json + "}";
}

void setStreamDataFunction(std::function<void (String)> setFunction) {
    streamDataWarpperFunction = setFunction;
}

void tryStreamData(String data) {
    if(streamDataWarpperFunction != nullptr) {
        streamDataWarpperFunction(data);
    }
}


String getCommandHelp() {
    String help = "All commands: \\n(use a command as parameter in the URL like: .../device/command/?help&...)\\n\\n";

    for(String command : AVAILABLE_COMMANDS) {
        help += "\t -> " + command + "\\n";
    }

    return help;
}


String getCommandList(String tabString) {
    String listString = tabString + "[";
    String innerTabString = tabString + "\t";

    for(String command : AVAILABLE_COMMANDS) listString += innerTabString + command + ",\n";

    return listString + tabString + " help]\n";
}

String getTimeInfo(gTime_t* secondsPtr, unsigned int base, String unit, String forMore = "s", String unitSuffix = " ", String zeroTimeString = "", bool zeroLeading = false) {
    unsigned int time = (unsigned int)((*secondsPtr) / base);
    
    unsigned int rest = (unsigned int)(*secondsPtr);
    rest %= base;
    *secondsPtr = (gTime_t)rest;

    String zerosInFront = ((zeroLeading && time <= 9) ? "0" : ""); 
    if(time > 1) unit += forMore;
    return time > 0 ? zerosInFront + String(time) + unit + unitSuffix : zeroTimeString;
}

String secondsToString(gTime_t valueInSeconds) {
    //return "off";
    String days = getTimeInfo(&valueInSeconds, (24 * 60 * 60), " day");
    String hours = getTimeInfo(&valueInSeconds, (60 * 60), " hour");
    String minutes = getTimeInfo(&valueInSeconds, (60), " minute");
    String seconds = getTimeInfo(&valueInSeconds, 1, " second", "s", "", "0 seconds");

    return days + hours + minutes + seconds;
}

String secondsToTimeString(gTime_t valueInSeconds) {
    //return "off";
    String days = getTimeInfo(&valueInSeconds, (24 * 60 * 60), " day");
    String hours = getTimeInfo(&valueInSeconds, (60 * 60), ":", "", "", "00:", true);
    String minutes = getTimeInfo(&valueInSeconds, (60), ":", "", "", "00:", true);
    String seconds = getTimeInfo(&valueInSeconds, 1, "", "", "", "00", true);

    String dayOffset = ((days.length() > 0) ? " (+" + days + ")" : "");
    return hours + minutes + seconds + dayOffset;
}

gTime_t getTime() { return daySeconds; }

void incTimeByMinimumStep() { daySeconds+= TIME_RESOLUTION; }

void setTime(gTime_t currentDaySeconds) { daySeconds = currentDaySeconds; }

bool isSliderInDarkMode() { return sliderDarkMode; }

void setSliderDarkMode(bool value) { sliderDarkMode = value; }
