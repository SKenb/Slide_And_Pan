#include "wifi.h"

#define MAX_STREAM_CLIENTS 2

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

const char *ssid = APSSID;
const char *password = APPPSW;

std::vector<WiFiClient> streamClients;
Ticker streamTimer;

void handleNotFound() {
    handleRoot();
}

String jsonDeviceStatus() {
    String json = "{\n";
    String tabString = "\t";

    jsonAddField(json, "Information", getBasicDataJson(tabString), tabString);
    jsonAddField(json, "Slide", slideJsonData(tabString), tabString);
    jsonAddField(json, "Pan", panJsonData(tabString), tabString);
    jsonAddField(json, "IO", getIOJsonData(tabString), tabString, false, true);

    return json + "\n}";
}

void handleRoot() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html","<html><head><title>Camera Pan and Slide</title></head><body style='background-color: #404040; color: whitesmoke; margin: 0px;'><h1>Camera Pan and Slide</h1><p>Please use the provided PWA APP available </p><a href='https://sebastianknoll.net/projects/CameraSlider'>here</a></body></html>");
}

void sendCommonHeaders() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Headers", "x-requested-with");
    server.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
}

void sliderRequestAll() {
    sendCommonHeaders();
    server.send(200, "application/json", jsonDeviceStatus());
}

void sliderRequestStream() {

    server.setContentLength(CONTENT_LENGTH_UNKNOWN); // the payload can go on forever
    server.sendContent_P(PSTR("HTTP/1.1 200 OK\nContent-Type: text/event-stream;\nConnection: keep-alive\nCache-Control: no-cache\nAccess-Control-Allow-Origin: *\n\n"));
  
    server.client().setNoDelay(true);
    server.client().setSync(true);

    if (streamClients.size() >= MAX_STREAM_CLIENTS) return handleNotFound();  // We ran out of channels
    streamClients.push_back(server.client());  
    debugMessage("One new stream client");  
}

String handleRequestParameter(String paramName, String paramValue, String tabString = "") {
    String json = "";

    paramName.toLowerCase();

    if(paramName == COMMAND_HELP) return "\"" + getCommandHelp() + "\"";
    if(paramName == COMMAND_LIST_COMMANDS) return getCommandList(tabString);

    if(paramName == COMMAND_GET_DEVICE_ID) return "\"" + String(SLIDEANDPAN_ID) + "\"";
    if(paramName == COMMAND_GET_DEVICE_NAME) return "\"" + String(SLIDEANDPAN_NAME) + "\"";
    if(paramName == COMMAND_GET_DEVICE_VERSION) return "\"" + String(SLIDEANDPAN_VERSION) + "\"";
    if(paramName == COMMAND_GET_DEVICE_COPYRIGHT) return "\"" + String(SLIDEANDPAN_COPYRIGHT) + "\"";
    
    String stepperResult = stepperHandleCommand(paramName, paramValue, tabString);
    if(stepperResult != "") return stepperResult;
    
    return "\"Unknown command 0.o - use >> /?help for help\"";
}

void sliderRequestCommand() {
    sendCommonHeaders();

    String commandResponse;

    if(server.args() <= 0) {
        commandResponse = "<h1>How to <i>command</i><h1><br><a href='" + String(SLIDEANDPAN_LINK_HANDBOOK) + "'>Handbook</a>";
        server.send(200, "text/html", commandResponse);
        return;
    }

    commandResponse = "{\n";
    String tabString = "\t";
    String commandArrayString = tabString + "[\n";
    String innerTabString = tabString + "\t";

    for (int i = 0; i < server.args(); i++) {
        
        String commandResultJsonString = "{\n";
        String commandResult = handleRequestParameter(server.argName(i), server.arg(i), innerTabString);
        
        jsonAddField(commandResultJsonString, "Index", String(i), innerTabString, false);
        jsonAddField(commandResultJsonString, "Name", server.argName(i), innerTabString, true);
        jsonAddField(commandResultJsonString, "Value", server.arg(i), innerTabString, true);
        jsonAddField(commandResultJsonString, "Result", commandResult, innerTabString, false, true);
        commandResultJsonString += "\n" + tabString + "}";

        commandArrayString += tabString + commandResultJsonString;
        if(i + 1 < server.args()) commandArrayString += ",\n";
    }

    jsonAddField(commandResponse, "Commands", commandArrayString + "]", tabString, false, true);
    commandResponse += "\n}";
    server.send(200, "application/json", commandResponse);
}

void streamData(String data) {

    std::vector<WiFiClient>::iterator it = streamClients.begin();
    while(it != streamClients.end()) {

        if (it->connected()) {
            it->printf_P(PSTR("event: event\ndata: %s\n\n"), data.c_str());
            it++;
        }
        else {
            it = streamClients.erase(it);
            debugMessage("One stream client left");
        }
    }
}

void keepAliveStream() {
    streamData(jsonDeviceStatus());
}

void serverRoutine() {
    server.handleClient();
    keepAliveStream();
}

void serverSetup() {
    delay(1000);
    
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(APSSID, APPPSW);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    IPAddress myIP = WiFi.softAPIP();
    debugMessage("serverSetup: IP = " + myIP.toString());
    
    server.on("/", handleRoot);
    server.onNotFound(handleRoot);

    
    server.on("/device/all/", sliderRequestAll);
    server.on("/device/stream/", sliderRequestStream);
    server.on("/device/command/", sliderRequestCommand);
    server.begin();

    //streamTimer.attach_ms(500, keepAliveStream);
    setStreamDataFunction(streamData);
}