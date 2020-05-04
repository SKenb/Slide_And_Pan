#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>

#include "common.h"
#include "io.h"
#include "stepper.h"
#include "debug.h"

#define APSSID SLIDEANDPAN_NAME
#define APPPSW SLIDEANDPAN_NAME

void serverSetup();
void handleRoot();
void sliderRequest();
void serverRoutine();
String handleRequestParameter(String paramName, String paramValue);

#endif