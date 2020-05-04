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

void handleRoot() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html","<html><head><title>Camera Pan and Slide</title></head><body style='background-color: #404040; color: whitesmoke; margin: 0px;'><h1>Camera Pan and Slide</h1><p>Please use the provided PWA APP available </p><a href='https://sebastianknoll.net/projects/CameraSlider'>here</a></body></html>");
}

void sliderRequestAll() {

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Headers", "x-requested-with");
    server.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
    
    String json = "{\n";
    String tabString = "\t";

    jsonAddField(json, "Information", getBasicDataJson(tabString), tabString);
    jsonAddField(json, "Slide", slideJsonData(tabString), tabString);
    jsonAddField(json, "Pan", panJsonData(tabString), tabString);
    jsonAddField(json, "IO", getIOJsonData(tabString), tabString, false, true);

    server.send(200, "application/json", json + "}");
}

void sliderRequestStream() {

    /*server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Headers", "x-requested-with");
    server.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
    
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Content-Type", "text/event-stream");*/

    server.setContentLength(CONTENT_LENGTH_UNKNOWN); // the payload can go on forever
    server.sendContent_P(PSTR("HTTP/1.1 200 OK\nContent-Type: text/event-stream;\nConnection: keep-alive\nCache-Control: no-cache\nAccess-Control-Allow-Origin: *\n\n"));
  
    server.client().setNoDelay(true);
    server.client().setSync(true);

    if (streamClients.size() >= MAX_STREAM_CLIENTS) return handleNotFound();  // We ran out of channels
    streamClients.push_back(server.client());  
    debugMessage("One new stream client");  
}

String handleRequestParameter(String paramName, String paramValue) {
    String result = "-";

    return result;
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
    String json = "{\n";
    String tabString = "\t";

    jsonAddField(json, "Information", getBasicDataJson(tabString), tabString);
    jsonAddField(json, "Slide", slideJsonData(tabString), tabString);
    jsonAddField(json, "Pan", panJsonData(tabString), tabString);
    jsonAddField(json, "IO", getIOJsonData(tabString), tabString, false, true);

    streamData(json + "}");
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
    server.begin();

    //streamTimer.attach_ms(500, keepAliveStream);
    setStreamDataFunction(streamData);
}