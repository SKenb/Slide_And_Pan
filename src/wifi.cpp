
#include "wifi.h"
#include "stepper.h"
#include "debug.h"

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

const char *ssid = APSSID;
const char *password = APPSK;

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
    
    String json = "{ \"version\": \"1.0.0.0\", \n";
    json += "\"steppers\": \"" + String(STEPPER_COUNT) + "\",";
    
    json += "\n \"arg_count\": \"";
    json += (String)server.args() + "\", ";
    json += "\n \"args\": { ";
    for (int i = 0; i < server.args(); i++) {
        String handleResult = handleRequestParameter(server.argName(i), server.arg(i));
        
        json += "\n \"" + server.argName(i) + "\": { \"value\": \"" + server.arg(i) + "\", \n \"result\": " + handleResult + "}";
        if(i + 1 <  server.args()) json += ", ";
    }
    json += "\n }";
        
    json += "\n }";
    server.send(200, "application/json", json);
}

String handleRequestParameter(String paramName, String paramValue) {
    String result = "-";

    // Stepper - a<COMMAND> for stepper a or b<COMMAND> for stepper b
    result = stepperControl(paramName.startsWith("a") ? STEPPER_A : STEPPER_B, paramName.substring(1), paramValue);

    
    if(paramName == "led") 
    {
        digitalWrite(LED_BUILTIN, paramValue != "0" ? LOW : HIGH);
        result = "\" LED switched ";
        result += (paramValue != "0" ? "ON" : "OFF");
        result += "\"";
    }

    if(paramName == "stop") 
    {
        eStop();
        result = "\"Emergency stop\"";
    }

    if(paramName == "status") 
    {
        result = "{ \"Mode\": \"Implementing\", \n";
        result += "\"Stepper A\": {" + stepperJsonStatus(STEPPER_A) + "}, \n";
        result += "\"Stepper B\": {" + stepperJsonStatus(STEPPER_B) + "} \n }";
    }
    
    return result;
}

void serverSetup() {
    delay(1000);
    
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    IPAddress myIP = WiFi.softAPIP();
    debugMessage("serverSetup: IP = " + myIP.toString());
    
    server.on("/", handleRoot);
    server.onNotFound(handleRoot);
    
    server.on("/slider/", sliderRequest);
    
    server.begin();
}
