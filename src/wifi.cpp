#include "wifi.h"
#include "slider.h"
#include "htmlFiles.h"
#include "htmlAppFiles.h"

#define MAX_STREAM_CLIENTS 2
#define KEEP_ALIVE_STREAM_COUNTER 5000

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

const char *ssid = APSSID;
const char *password = APPPSW;

unsigned int keepAliveStreamCounter = 0;

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

String getStreamData() {
    String stream = " { ";

    stream += getSlideStepper()->streamData("slide");
    stream += ", " + getPanStepper()->streamData("pan");

    stream += ", " + getStatusStreamData();

    stream += ", " + getIOBoard()->getBatteryStream();

    return stream + "}";
}

void handleRoot() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html", getIndexPage());
}

void handleTimelapse() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html", getTimelapsePage());
}

void handleAbout() {
    server.send(200, "text/html", getAboutPage());
}

void handleSliderDetail() {
    server.send(200, "text/html", getSliderdetailPage());
}

void handleSettings() {
    server.send(200, "text/html", getSettingsPage());
}

void handleMainCss() {
    server.send(200, "text/css", getMaincssPage());
}

void handleMainJS() {
    server.send(200, "application/javascript", getMainjsPage());
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
    
    if(paramName == COMMAND_TIMELAPSE_START) {
        startTimelapse();
        return "Start Timelapse";
    }

    if(paramName == COMMAND_TIMELAPSE_STOP) {
        stopTimelapse();
        stopTimelapsePreview();
        return "Stopped Timelapse (+Preview)";
    }

    if(paramName == COMMAND_TIMELAPSE_PREVIEW_START) {
        startTimelapsePreview();
        return "Start timelapse preview";
    }

    if(paramName == COMMAND_TIMELAPSE_PREVIEW_STOP) {
        stopTimelapsePreview();
        return "Stop timelapse preview";
    }

    if(paramName == COMMAND_TIMELAPSE_PAN) {
        setTimelapsePanSteps(paramValue.toFloat());
        return "Timelapse pan steps set to: " + String(paramValue.toFloat());
    }

    if(paramName == COMMAND_TIMELAPSE_SLIDE) {
        setTimelapseSlideSteps(paramValue.toFloat());
        return "Timelapse slide steps set to: " + String(paramValue.toFloat());
    }

    if(paramName == COMMAND_TIMELAPSE_INTERVAL) {
        setTimelapseInterval(paramValue.toFloat());
        return "Timelapse interval set to: " + String(paramValue.toInt());
    }

    if(paramName == COMMAND_TIMELAPSE_PHOTO) {
        getIOBoard()->cameraSetExposureTime(paramValue.toFloat());
        return "Timelapse exposure time set to: " + String(paramValue.toInt());
    }

    if(paramName == COMMAND_TIMELAPSE_COUNT) {
        setTimelapsePictureCount(paramValue.toInt());
        return "Timelapse picture count set to: " + String(paramValue.toInt());
    }

    if(paramName == COMMAND_TIMELAPSE_MILKYWAY_SETTINGS) {

        getPanStepper()->setResolution(SIXTEENTH);
        getSlideStepper()->setResolution(QUARTER);

        float coeff = isTimelapseInNortherHemisphere() ? 1.0 : -1.0;
        setTimelapsePanSteps(coeff * ((float)getTimelapseInterval() * 360.0) / EARTH_SECONDS_PER_DAY);

        return "Timelapse milkyway settings made - Pan: " + String(getTimelapsePanSteps() + getPanStepper()->getPhysicalValueUnit() + "/TimelapseStep");
    }

    if(paramName == COMMAND_TIMELAPSE_SET_HEMISPHERE) {

        setTimelapseHemisphere(paramValue.toInt() > 0);
        return "Timelapse hemisphere set to: " + String((isTimelapseInNortherHemisphere() ? "Northern" : "South"));
    }

    if(paramName == COMMAND_SET_TIME) {

        setTime(paramValue.toDouble());
        return "Slider - time set to" + secondsToTimeString(getTime());
    }

    if(paramName == COMMAND_SET_DARKMODE) {

        setSliderDarkMode(paramValue.toInt() > 0);
        return "Slider - DarkMode: " + isSliderInDarkMode() ? "On" : "Off";
    }

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
    keepAliveStreamCounter++;

    if(keepAliveStreamCounter < KEEP_ALIVE_STREAM_COUNTER) return;

    keepAliveStreamCounter = 0;
    streamData(getStreamData());
}

void serverRoutine() {
    server.handleClient();
    keepAliveStream();
}

void handleGenericHTML(String page) {
    server.send(200, "text/html", page);
}

void handleGenericCSS(String page) {
    server.sendHeader("Cache-Control", "max-age=31536000");
    server.send(200, "text/css", page);
}

void handleGenericJS(String page) {
    server.sendHeader("Cache-Control", "max-age=31536000");
    server.send(200, "application/json", page);
}

void serverSetup() {
    delay(1000);
    
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(APSSID, APPPSW);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    IPAddress myIP = WiFi.softAPIP();
    debugMessage("serverSetup: IP = " + myIP.toString());
    
    server.on("/", handleRoot);
    server.on("/timelapse", handleTimelapse);
    server.on("/settings", handleSettings);
    server.on("/detail", handleSliderDetail);
    server.on("/about", handleAbout);
    server.on("/main.css", handleMainCss);
    server.on("/main.js", handleMainJS);
    server.onNotFound(handleRoot);

    server.on("/app/", [](){ handleGenericHTML(getIndexAppPage()); });
    server.on("/app/index.html", [](){ handleGenericHTML(getIndexAppPage()); });
    server.on("/app/control.html", []() { handleGenericHTML(getControlAppPage()); });
    server.on("/app/timelapse.html", []() { handleGenericHTML(getTimelapseAppPage()); });
    server.on("/app/milkyway.html", []() { handleGenericHTML(getMilkywayAppPage()); });
    server.on("/app/tracker.html", []() { handleGenericHTML(getTrackerAppPage()); });
    server.on("/app/plan.html", []() { handleGenericHTML(getPlanAppPage()); });
    server.on("/app/info.html", []() { handleGenericHTML(getInfoAppPage()); });
    server.on("/app/mainCss.css", []() { handleGenericCSS(getMaincssAppPage()); });
    server.on("/app/definesCss.css", []() { handleGenericCSS(getDefinescssAppPage()); });
    server.on("/app/mainJs.js", []() { handleGenericJS(getMainjsAppPage()); });

    server.on("/device/all/", sliderRequestAll);
    server.on("/device/stream/", sliderRequestStream);
    server.on("/device/command/", sliderRequestCommand);
    server.begin();

    //streamTimer.attach_ms(500, keepAliveStream);
    setStreamDataFunction(streamData);

}