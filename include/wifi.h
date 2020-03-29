
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define APSSID "CameraSlider"
#define APPSK  "CameraSlider"

void serverSetup();
void handleRoot();
void sliderRequest();
void serverRoutine();
String handleRequestParameter(String paramName, String paramValue);