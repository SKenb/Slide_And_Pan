#include "wifi.h"
#include "slider.h"
#include "htmlFiles.h"

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

String getStreamData() {
    String stream = "{ ";

    stream += getSlideStepper()->streamData("Slide");
    stream += ", " + getPanStepper()->streamData("Pan");

    return stream + "}";
}

void handleRoot() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html","<!doctype html><html lang='en'>            <head>            <meta charset='utf-8'>            <meta charset='utf-8'>            <meta name='viewport' content='width=device-width, initial-scale=1.0'>            <title>Slide and Pan</title>            <meta name='description' content='Slide and Pan'>            <meta name='author' content='Sebastian Knoll'>            </head>          <body>              <style>            * {              font-family:courier,arial,helvetica;            }            body {              background-color: rgb(180, 180, 180);            }            li {              display:inline-block;              margin: 5px;            }            ul {              margin: 0 auto;              display: inherit;            }            h1, h2, h3 {              text-align: center;            }            #container {              width: 100%;              max-width: 960px;              margin: 0 auto;            }            .section {              background: whitesmoke;              border-radius: 20px;              padding: 20px;            }            input {              width: 50px;            }            .set_button {              width: 200px;            }            .block {              display: block;              width: 100%;              height: 50px;;            }          </style>                      <script>            function makeRequest(command) {              var url = '/device/command/?' + command;                            var request = new XMLHttpRequest();              request.onreadystatechange = function() {                if (this.readyState == 4 && this.status == 200) {                  location.reload();                }              };              request.open('GET',url);              request.send();            }            function getDaytimeInSeconds() {              let now = new Date();              return now.getSeconds() + 60 * now.getMinutes() + 60*60* now.getHours();             }            var updateTimeRequest = new XMLHttpRequest();            updateTimeRequest.open('GET', '/device/command/?set_time=' + getDaytimeInSeconds());            updateTimeRequest.send();          </script>            <div id='container'>    <h1>Manual control</h1>    <ul>      <li><a href='index'>Control</a></li>      <li><a href='timelapse'>Timelapse</a></li>      <li><a href='settings'>Settings</a></li>    </ul>    <br>    <div class='section'>      <h2>Control</h2>      <br>      <button class='block' onclick='makeRequest(\"slide_to=1000000\");'>Slide Left</button>      <br>      <button class='block' onclick='makeRequest(\"slide_to=-1000000\");'>Slide Right</button>      <br>      <button class='block' onclick='makeRequest(\"pan_to=1000000\");'>Pan Left</button>      <br>      <button class='block' onclick='makeRequest(\"pan_to=-1000000\");'>Pan Right</button>      <br><br>      <button class='block' onclick='makeRequest(\"slide_stop&pan_stop\");'>Stop</button>      <br>    </div>    <br><br>    <div class='section'>      <h2>Speed</h2>      <br>      <button class='block' onclick='makeRequest(\"slide_speed=1&pan_speed=2\");'>Fast</button>      <br>      <button class='block' onclick='makeRequest(\"slide_speed=0.5&pan_speed=1\");'>Middle</button>      <br>      <button class='block' onclick='makeRequest(\"slide_speed=0.25&pan_speed=0.5\");'>Slow</button>      <br>      <button class='block' onclick='makeRequest(\"slide_speed=0.05&pan_speed=0.05\");'>Extrem slow</button>      <br>    </div>    <br><br>    <div class='section'>      <h2>Power</h2>      <br>      <button class='block' onclick='makeRequest(\"slide_sleep=0&pan_sleep=0\");'>Sleep</button>      <br>      <button class='block' onclick='makeRequest(\"slide_sleep=1&pan_sleep=1\");'>Power ON</button>      <br>    </div>    <br>    <br>    <a style='text-align: center; display: block;' href='about'>About</a>    <br>    <br>  </div></body></html>");
}

void handleTimelapse() {
    //server.sendHeader("Location", "",true);   //Redirect to our html web page  
    server.send(200, "text/html", getTimelapsePage());
}

void handleAbout() {
    server.send(200, "text/html", "<!doctype html><html lang='en'>            <head>            <meta charset='utf-8'>            <meta charset='utf-8'>            <meta name='viewport' content='width=device-width, initial-scale=1.0'>            <title>Slide and Pan</title>            <meta name='description' content='Slide and Pan'>            <meta name='author' content='Sebastian Knoll'>            </head>          <body>            <style>            * {              font-family:courier,arial,helvetica;            }            body {              background-color: rgb(180, 180, 180);            }            li {              display:inline-block;              margin: 5px;            }            ul {              margin: 0 auto;              display: inherit;            }            h1, h2, h3 {              text-align: center;            }            #container {              width: 100%;              max-width: 960px;              margin: 0 auto;            }            .section {              background: whitesmoke;              border-radius: 20px;              padding: 20px;            }            input {              width: 50px;            }            .set_button {              width: 200px;            }            .block {              display: block;              width: 100%;              height: 50px;;            }          </style>                      <script>            function makeRequest(command) {              var url = '/device/command/?' + command;                            var request = new XMLHttpRequest();              request.onreadystatechange = function() {                if (this.readyState == 4 && this.status == 200) {                  location.reload();                }              };              request.open('GET',url);              request.send();            }            function getDaytimeInSeconds() {              let now = new Date();              return now.getSeconds() + 60 * now.getMinutes() + 60*60* now.getHours();             }            var updateTimeRequest = new XMLHttpRequest();            updateTimeRequest.open('GET', '/device/command/?set_time=' + getDaytimeInSeconds());            updateTimeRequest.send();          </script>            <div id='container'>    <h1>About</h1>    <ul>      <li><a href='index'>Control</a></li>      <li><a href='timelapse'>Timelapse</a></li>      <li><a href='settings'>Settings</a></li>    </ul>    <br>    <div class='section'>      <h2>Slider Inforamtion</h2>      <br>      <p>Name: <i>" + String(SLIDEANDPAN_NAME) + "</i></p>      <p>Version: <i>" + String(SLIDEANDPAN_VERSION) + "</i></p>      <p>Copyright: <i>" + String(SLIDEANDPAN_COPYRIGHT) + "</i></p>      <p>ID: <i>" + String(SLIDEANDPAN_ID) + "</i></p>      <br>      <a style='text-align: center; display: block;' href='detail'>More details</a>      <br>    </div>    <br><br>    <div class='section'>      <h2>Author</h2>      <br>      <p>        My name is <i>Sebastian Knoll</i> and I am the inventor and author of this project.         If you have any question regarding me or the app do not hesitate to         <a href='https://sebastianknoll.net/me/contact.php'>contact me</a> or visit my         <a href='https://sebastianknoll.net/'>homepage</a>.         <br>        <br>        If you have any legal concerns, note that this is an private application         which you can use at your own risk. Furthermore you can checkout         <a href='https://sebastianknoll.net/gtc/'>legal stuff</a> and         <a href='https://sebastianknoll.net/gtc/TODO'>legal stuff for Slide and Pan</a>.      </p>      <br>    </div>    <br><br>    <div class='section'>      <h2>More</h2>      <br>      <p>      Use the Hashtag <a href='https://www.instagram.com/explore/tags/slideandpan/'>#slideandpan</a> so that we can see your amazing work.      </p>      <br>      <p>Use the <a href='" + String(SLIDEANDPAN_LINK_HANDBOOK) + "'>Handbook</a> for more information and tutorials.</p>      <br>    </div>  </div>  <br>  <br>  </body></html>");
}

void handleSliderDetail() {
    server.send(200, "text/html", "<!doctype html><html lang='en'>            <head>            <meta charset='utf-8'>            <meta charset='utf-8'>            <meta name='viewport' content='width=device-width, initial-scale=1.0'>            <title>Slide and Pan</title>            <meta name='description' content='Slide and Pan'>            <meta name='author' content='Sebastian Knoll'>            </head>          <body>            <style>            * {              font-family:courier,arial,helvetica;            }            body {              background-color: rgb(180, 180, 180);            }            li {              display:inline-block;              margin: 5px;            }            ul {              margin: 0 auto;              display: inherit;            }            h1, h2, h3 {              text-align: center;            }            #container {              width: 100%;              max-width: 960px;              margin: 0 auto;            }            .section {              background: whitesmoke;              border-radius: 20px;              padding: 20px;            }            input {              width: 50px;            }            .set_button {              width: 200px;            }            .block {              display: block;              width: 100%;              height: 50px;;            }          </style>                      <script>            function makeRequest(command) {              var url = '/device/command/?' + command;                            var request = new XMLHttpRequest();              request.onreadystatechange = function() {                if (this.readyState == 4 && this.status == 200) {                  location.reload();                }              };              request.open('GET',url);              request.send();            }            function getDaytimeInSeconds() {              let now = new Date();              return now.getSeconds() + 60 * now.getMinutes() + 60*60* now.getHours();             }            var updateTimeRequest = new XMLHttpRequest();            updateTimeRequest.open('GET', '/device/command/?set_time=' + getDaytimeInSeconds());            updateTimeRequest.send();          </script>            <div id='container'>    <h1>Slider in detail</h1>    <ul>      <li><a href='index'>Control</a></li>      <li><a href='timelapse'>Timelapse</a></li>      <li><a href='settings'>Settings</a></li>    </ul>    <br>    <div class='section'>      <h2>Slider Inforamtion</h2>      <br>      <p>Name: <i>" + String(SLIDEANDPAN_NAME) + "</i></p>      <p>Version: <i>" + String(SLIDEANDPAN_VERSION) + "</i></p>      <p>Copyright: <i>" + String(SLIDEANDPAN_COPYRIGHT) + "</i></p>      <p>ID: <i>" + String(SLIDEANDPAN_ID) + "</i></p>      <br>      <p>Note: <i>" + String(SLIDEANDPAN_ID_INFO) + "</i></p>      <p>Ver.-Info: <i>" + String(SLIDEANDPAN_VERSION_INFO) + "</i></p>      <br>    </div>    <br><br>    <div class='section'>      <h2>Hardware - Slide</h2>      <br>      <p>Direction Pin: <i>" + String(SLIDE_STEPPER_PIN_DIRECTION) + "</i></p>      <p>Step Pin <i>" + String(SLIDE_STEPPER_PIN_STEP) + "</i></p>      <p>Sleep Pin <i>" + String(SLIDE_STEPPER_PIN_SLEEP) + "</i></p>      <p>Steps / Turn: <i>" + String(STEPPER_STEPS_PER_TURN) + "</i></p>      <p>Can change res.: <i>" + (getSlideStepper()->hasChangeResolutionMethod() ? "Yes" : "No") + "</i></p>      <br>      <p>Driver M1 Pin <i>" + String(SLIDE_STEPPER_DRIVER_M1) + "</i></p>      <p>Driver M2 Pin <i>" + String(SLIDE_STEPPER_DRIVER_M2) + "</i></p>      <p>Driver M3 Pin <i>" + String(SLIDE_STEPPER_DRIVER_M3) + "</i></p>      <br>    </div>    <br><br>    <div class='section'>      <h2>Hardware - Pan</h2>      <br>      <p>Direction Pin: <i>" + String(PAN_STEPPER_PIN_DIRECTION) + "</i></p>      <p>Step Pin <i>" + String(PAN_STEPPER_PIN_STEP) + "</i></p>      <p>Sleep Pin <i>" + String(PAN_STEPPER_PIN_SLEEP) + "</i></p>      <p>Steps / Turn: <i>" + String(STEPPER_STEPS_PER_TURN) + "</i></p>      <p>Can change res.: <i>" + (getPanStepper()->hasChangeResolutionMethod() ? "Yes" : "No") + "</i></p>      <br>      <p>Driver M1 Pin <i>" + String(PAN_STEPPER_DRIVER_M1) + "</i></p>      <p>Driver M2 Pin <i>" + String(PAN_STEPPER_DRIVER_M2) + "</i></p>      <p>Driver M3 Pin <i>" + String(PAN_STEPPER_DRIVER_M3) + "</i></p>      <br>    </div>    <br><br>    <div class='section'>      <h2>IO Board</h2>      <br>      <p>Latch Pin: <i>" + String(PIN_LATCH) + "</i></p>      <p>Clock Pin: <i>" + String(PIN_CLOCK) + "</i></p>      <p>DataOut Pin: <i>" + String(PIN_DATAOUT) + "</i></p>      <p>DataIn Pin: <i>" + String(PIN_DATAIN) + "</i></p>      <p>LatchIn Pin: <i>" + String(PIN_LATCHIN) + "</i></p>      <br>      <p>Enc. CLK Pin: <i>" + String(ENCODER_PIN_CLK) + "</i></p>      <p>Enc. DIR Pin: <i>" + String(ENCODER_PIN_DIRECTION) + "</i></p>      <p>Enc. Switch Pin: <i>" + String(ENCODER_PIN_SWITCH) + "</i></p>      <br>      <p>Battery AI Pin: <i>" + String(PIN_BATTERY) + "</i></p>      <br>    </div>    <br><br>    <div class='section'>      <h2>Constants</h2>      <br>      <p>Gear diameter: <i>" + String(SLIDE_GEAR_DIAMETER) + "</i></p>      <p>Gear cog number: <i>" + String(PAN_GEAR_SMALL_COG_NUMBER) + "</i></p>      <p>Big gear cog number: <i>" + String(PAN_GEAR_LARGE_COG_NUMBER) + "</i></p>      <p>Earth rotation: <i>" + secondsToString(EARTH_SECONDS_PER_DAY) + "</i></p>      <p>Moon rotation: <i>" + secondsToString(MOON_SECONDS_PER_TURN) + "</i></p>            <br>    </div>    <br><br>    <div class='section'>      <h2>Time</h2>      <br>      <p>Slider Time: <i>" + secondsToTimeString(getTime()) + "</i></p>      <br>      <button class='block' onclick='makeRequest(\"\");'>Set time</button>      <br>    </div>  </div>  <br>  <br>  </body></html>");
}

void handleSettings() {
    server.send(200, "text/html", "<!doctype html><html lang='en'>            <head>            <meta charset='utf-8'>            <meta charset='utf-8'>            <meta name='viewport' content='width=device-width, initial-scale=1.0'>            <title>Slide and Pan</title>            <meta name='description' content='Slide and Pan'>            <meta name='author' content='Sebastian Knoll'>            </head>          <body>            <style>            * {              font-family:courier,arial,helvetica;            }            body {              background-color: rgb(180, 180, 180);            }            li {              display:inline-block;              margin: 5px;            }            ul {              margin: 0 auto;              display: inherit;            }            h1, h2, h3 {              text-align: center;            }            #container {              width: 100%;              max-width: 960px;              margin: 0 auto;            }            .section {              background: whitesmoke;              border-radius: 20px;              padding: 20px;            }            input {              width: 50px;            }            .set_button {              width: 200px;            }            .block {              display: block;              width: 100%;              height: 50px;;            }          </style>                      <script>            function makeRequest(command) {              var url = '/device/command/?' + command;                            var request = new XMLHttpRequest();              request.onreadystatechange = function() {                if (this.readyState == 4 && this.status == 200) {                  location.reload();                }              };              request.open('GET',url);              request.send();            }            function getDaytimeInSeconds() {              let now = new Date();              return now.getSeconds() + 60 * now.getMinutes() + 60*60* now.getHours();             }            var updateTimeRequest = new XMLHttpRequest();            updateTimeRequest.open('GET', '/device/command/?set_time=' + getDaytimeInSeconds());            updateTimeRequest.send();          </script>            <div id='container'>    <h1>Settings</h1>    <ul>      <li><a href='index'>Control</a></li>      <li><a href='timelapse'>Timelapse</a></li>      <li><a href='settings'>Settings</a></li>    </ul>    <br>    <div class='section'>      <h2>Slide</h2>      <br>      <p>" + (getSlideStepper()->getPhysicalValueDescription()) + ": <i>" + (getSlideStepper()->getPhysicalValueInfo()) + "</i> <button onclick='makeRequest(\"slide_reset\");'>Center</button></p>      <input type='number' value='" + (getSlideStepper()->getPhysicalValueInfo()) + "' step='0.001' min='-50000' max='50000' id='slide_pv'>      <button class='set_button' onclick='makeRequest(\"slide_physical_value=\" + document.getElementById(\"slide_pv\").value);'>Go to " + (getSlideStepper()->getPhysicalValueDescription()) + "</button>      <br>      <br>      <p>Sleep: <i>" + String(getSlideStepper()->isSleeping() ? "Sleeping" : "Awake") + "</i> <button onclick='makeRequest(\"slide_sleep=" + String(getSlideStepper()->isSleeping() ? 1 : 0) + "\");'>" + String(getSlideStepper()->isSleeping() ? "Awake" : "Sleep") + "</button></p>      <p>Steps: <i>" + String(getSlideStepper()->getSteps()) + "</i>/<i>" + String(getSlideStepper()->getTargetSteps()) + "</i></p>      <p>Speed: <i>" + String(getSlideStepper()->getSpeed()) + "</i>/<i>" + String(getSlideStepper()->getTargetSpeed()) + "</i></p>      <p>LazyMode: <i>" + (getSlideStepper()->isInLazyMode() ? "Yes" : "No") + "</i> <button onclick='makeRequest(\"slide_lazy=" + (getSlideStepper()->isInLazyMode() ? 0 : 1) + "\");'>" + (getSlideStepper()->isInLazyMode() ? "Go Strong" : "Go Lazy") + "</button></p>      <br>      <p>Resolution: <i>" + String(getSlideStepper()->getResolution()) + "</i></p>            <div>        <button onclick='makeRequest(\"slide_res=1\");'>1</button>        <button onclick='makeRequest(\"slide_res=2\");'>2</button>        <button onclick='makeRequest(\"slide_res=4\");'>4</button>        <button onclick='makeRequest(\"slide_res=8\");'>8</button>        <button onclick='makeRequest(\"slide_res=16\");'>16</button>      </div>      <br>    </div>    <br><br>    <div class='section'>      <h2>Pan</h2>      <br>      <p>" + (getPanStepper()->getPhysicalValueDescription()) + ": <i>" + (getPanStepper()->getPhysicalValueInfo()) + "</i> <button onclick='makeRequest(\"pan_reset\");'>Center</button></p>      <input type='number' value='" + (getPanStepper()->getPhysicalValueInfo()) + "' step='0.001' min='-50000' max='50000' id='pan_pv'>      <button class='set_button' onclick='makeRequest(\"pan_physical_value=\" + document.getElementById(\"pan_pv\").value);'>Go to " + (getPanStepper()->getPhysicalValueDescription()) + "</button>      <br>      <br>      <p>Sleep: <i>" + String(getPanStepper()->isSleeping() ? "Sleeping" : "Awake") + "</i> <button onclick='makeRequest(\"pan_sleep=" + String(getPanStepper()->isSleeping() ? 1 : 0) + "\");'>" + String(getPanStepper()->isSleeping() ? "Awake" : "Sleep") + "</button></p>      <p>Steps: <i>" + String(getPanStepper()->getSteps()) + "</i>/<i>" + String(getPanStepper()->getTargetSteps()) + "</i></p>      <p>Speed: <i>" + String(getPanStepper()->getSpeed()) + "</i>/<i>" + String(getPanStepper()->getTargetSpeed()) + "</i></p>      <p>LazyMode: <i>" + (getPanStepper()->isInLazyMode() ? "Yes" : "No") + "</i> <button onclick='makeRequest(\"pan_lazy=" + String(getPanStepper()->isInLazyMode() ? 0 : 1) + "\");'>" + (getPanStepper()->isInLazyMode() ? "Go Strong" : "Go Lazy") + "</button></p>      <br>      <p>Resolution: <i>" + String(getPanStepper()->getResolution()) + "</i></p>            <div>        <button onclick='makeRequest(\"pan_res=1\");'>1</button>        <button onclick='makeRequest(\"pan_res=2\");'>2</button>        <button onclick='makeRequest(\"pan_res=4\");'>4</button>        <button onclick='makeRequest(\"pan_res=8\");'>8</button>        <button onclick='makeRequest(\"pan_res=16\");'>16</button>      </div>      <br>    </div>    <br><br>    <div class='section'>      <h2>Additional</h2>      <br>      <p>Battery-Level: <i>" + String(getIOBoard()->getBatteryLevel()) + "</i></p>      <br>      <p>S/P LazyMode: <i>" + (getPanStepper()->isInLazyMode() ? "Yes" : "No") + "</i>/<i>" + (getPanStepper()->isInLazyMode() ? "Yes" : "No") + "</i></p>      <button onclick='makeRequest(\"pan_lazy=1&slide_lazy=1\");'>Set both Lazy</button>      <button onclick='makeRequest(\"pan_lazy=0&slide_lazy=0\");'>Set both Strong</button>      <br>      <br>      <br>      <p id='darkMode' name='darkMode'>DarkMode: <a href='#darkMode' onclick='makeRequest(\"set_darkmode=" + String(isSliderInDarkMode() ? 0 : 1) + "\");'><i>" + (isSliderInDarkMode() ? "is On" : "is Off") + "</i></a></p>      <br>    </div>    <br>    <br>    <div class='section'>      <h2>Logs</h2>      <br>      <div style='overflow-x: scroll; padding-bottom: 10px;'>        <p style='display: inline; white-space: nowrap; overflow-x: scroll;'>" + getRingBufferForWeb() + "</p>      </div>      <br>    </div>  </div>  <br>  <br>  <a style='text-align: center; display: block;' href='about'>About</a>  <br>  <br></body></html>");
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
    streamData(getStreamData());
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
    server.on("/timelapse", handleTimelapse);
    server.on("/settings", handleSettings);
    server.on("/detail", handleSliderDetail);
    server.on("/about", handleAbout);
    server.onNotFound(handleRoot);

    
    server.on("/device/all/", sliderRequestAll);
    server.on("/device/stream/", sliderRequestStream);
    server.on("/device/command/", sliderRequestCommand);
    server.begin();

    //streamTimer.attach_ms(500, keepAliveStream);
    setStreamDataFunction(streamData);

}