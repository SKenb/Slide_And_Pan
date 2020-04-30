#include "wifi.h"

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

const char *ssid = APSSID;
const char *password = APPPSW;

void handleRoot() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html","<html><head><title>Camera Pan and Slide</title></head><body style='background-color: #404040; color: whitesmoke; margin: 0px;'><h1>Camera Pan and Slide</h1><p>Please use the provided PWA APP available </p><a href='https://sebastianknoll.net/projects/CameraSlider'>here</a></body></html>");
}

void serverRoutine() {
    server.handleClient();
}

void sliderRequest() {

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Headers", "x-requested-with");
    server.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
    
    String json;
    String tabString = "";

    jsonAddField(json, "Information", getBasicDataJson(tabString), tabString);
    jsonAddField(json, "Slide", slideJsonData(tabString), tabString);
    jsonAddField(json, "Pan", panJsonData(tabString), tabString);
    jsonAddField(json, "IO", getIOJsonData(tabString), tabString, false, true);

    server.send(200, "application/json", json);
}

String handleRequestParameter(String paramName, String paramValue) {
    String result = "-";

    return result;
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

    
    server.on("/slider/", sliderRequest);
    server.begin();
}
