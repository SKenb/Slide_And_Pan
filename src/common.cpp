#include "common.h"

std::function<void (String)> streamDataWarpperFunction = nullptr;

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
