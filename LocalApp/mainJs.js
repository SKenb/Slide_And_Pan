optionPageMap = {
    1: "./control.html",
    2: "./timelapse.html",
    3: "./milkyway.html",
    4: "./tracker.html",
    5: "./plan.html",
    6: "./info.html"
};

function select(optionNr) {

    iframe = document.getElementById('iframe');
    iframe.src = optionPageMap[optionNr];

    wheel = document.getElementById('select');

    [1, 2, 3, 4, 5, 6, 7, 8].forEach(element => {
        wheel.classList.remove("option" + element);
    });

    wheel.classList.add("option" + optionNr);
};

function changeSpeed(sender) {
    requestStrings = [
        '',
        'slide_speed=0.05&pan_speed=0.05',
        'slide_speed=0.25&pan_speed=0.5',
        'slide_speed=0.5&pan_speed=1',
        'slide_speed=1&pan_speed=2'
    ];

    makeRequest(requestStrings[sender.value], false);
    document.getElementById('speed-selection').innerHTML = (['Zero', 'Extremely Slow', 'Slow', 'Middle', 'Fast'])[sender.value];
};

function makeRequest(command, reload = false) {
    var url = '/device/command/?' + command;
    
    var request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            if(reload) location.reload();
        }
    };

    request.open('GET',url);
    request.send();
};

function getDaytimeInSeconds() {
    let now = new Date();
    return now.getSeconds() + 60 * now.getMinutes() + 60*60* now.getHours(); 
};

var streamData = null;
function startStreamListener() {
    var $event_source_name = new EventSource("../device/stream/");

    $event_source_name.addEventListener("event", function(e) { 
    var result = JSON.parse( e.data );
    
    if(e.lastEventId == "CLOSE") { console.log('Closed stream'); }
    else { 
        streamData = result;
        updateGUIFromStramData();
    }

    });

    $event_source_name.addEventListener("error", function(e) { $event_source_name.close(); });
}

classPathMap = {
    'insertSTATE': (data) => data['state']['state'],
    'insertSTATEDETAIL': (data) => data['state']['detail'],
    'insertSLIDEPOS': (data) => data['slide']['Position'],
    'insertSLIDEDIR': (data) => data['slide']['Direction'],
    'insertSLIDESLEEP': (data) => data['slide']['Sleeping'],
    'insertPANPOS': (data) => data['pan']['Position'],
    'insertPANDIR': (data) => data['pan']['Direction'],
    'insertPANSLEEP': (data) => data['pan']['Sleeping'],
};

function updateGUIFromStramData() {
    if(!streamData) return;

    heartBeat();
    updateBattery(streamData["battery"]);
    updatePowerState(streamData["slide"]["Sleeping"], streamData["pan"]["Sleeping"])

    for (var classOI in classPathMap) {
        if (classPathMap.hasOwnProperty(classOI)) {      
            updateGUIValue(classOI, classPathMap[classOI](streamData));
        }
    }
}

function updateGUIValue(classOI, value) {

    var elements = document.getElementsByClassName(classOI);

    ifr = document.getElementById('iframe');
    var iFrameElements = ifr 
        ? document.getElementById('iframe').contentWindow.document.getElementsByClassName(classOI)
        : [];

    [elements, iFrameElements].forEach(list => {

        for (var index = 0; index < list.length; index++) {
            try {
                list[index].innerHTML = value;
            } catch (error) {
                
            }
        }
    });
}
    
function updateBattery(batteryData) {
    let batEl = document.querySelector("#battery-fill");

    try {
        let level = batteryData["level"];
        let full = batteryData["full"];
        let middle = batteryData["middle"];
        let empty = batteryData["empty"];
        let critical = batteryData["critical"];

        batEl.style.width = Math.max(10, Math.min(100, (level-critical)/(full-critical)*100)) + "%";
    
        batEl.style.backgroundColor = "forestgreen";
        if(level <= middle) batEl.style.backgroundColor = "#F4AF00";
        if(level <= empty) batEl.style.backgroundColor = "#cc0000";
        if(level <= critical) batEl.style.backgroundColor = "red";

    } catch (error) {
        batEl.style.width = "50%";
        batEl.style.backgroundColor = "blue";
    }
}

function updateCameraState() {
    var inner = '&#128247; ?';
    
    var state = streamData ? streamData["state"]["camera"].toLowerCase() : "";
    
    switch(state) {
        case 'photo': 
            inner = '&#128248; Shoot';
            break;

        case 'idle': 
            inner = '&#128247;';
            break;
            
        case 'prepare': 
            inner = '&#128247; Prepare';
            break;
            
        case 'rest': 
            inner = '&#128247; Rest';
            break;
            
        case '?': 
            inner = '&#128247; ?! :0';
            break;
    }

    let el = document.getElementById('camera-state');
    if(el) el.innerHTML = inner;
}

var heartBeatCount = 0;
function heartBeat() {
    heartBeatCount = 6;
    setConnection(true);
    setInnerConnection(true);
}

function setConnection(visible) {
    try {
        document.querySelector("#connection").style.display = visible ? 'unset' : 'none';
    } catch (error) { }
}

function setInnerConnection(antenne) {
    try {
        document.querySelector("#connection").innerHTML = antenne ? '&#128225;' : '&#10060;';
    } catch (error) { }
}

function updatePowerState(slideState, panState) {
    mapState = (state) => (state.toLowerCase() == 'yes') ? '&#128164;' : '&#128170;';
    slideState = mapState(slideState);
    panState = mapState(panState);

    document.getElementById('power-state').innerHTML = slideState + " / " + panState;
}

setInterval(function() {
    updateCameraState()

    heartBeatCount--;
    if(heartBeatCount<=0) {
        streamData = null;

        if(heartBeatCount >= -10) {
            setConnection(heartBeatCount % 2);
        
            dotString = "...".substr(0, Math.abs(heartBeatCount % 4));
            updateGUIValue("insertSTATE", "Wait for Slider" + dotString);
            updateGUIValue("insertSTATEDETAIL", "Maybe we lost the connection :0");
        }
        else {
            setInnerConnection(false);
            setConnection(true);
            updateGUIValue("insertSTATE", "No Connection!");
            updateGUIValue("insertSTATEDETAIL", "Please try to reconnect.");
        }
    }
}, 500);


var updateTimeRequest = new XMLHttpRequest();
updateTimeRequest.open('GET', '../device/command/?set_time=' + getDaytimeInSeconds());
updateTimeRequest.send();