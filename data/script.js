/**
 * Simmes controlprogge
 */

let serverURL = "http://sharknano-server.local";
let websocketURL = "ws://sharknano-server.local/ws";
var ws;

//Statemachine
let buttonState = "standby";

let runState = false;


const standbyButton = document.getElementById("btn-standby");
const videoStartButton = document.getElementById("btn-videoStart");
const videoStopBtn = document.getElementById("btn-videoStop");

const directionBtn = document.getElementById("direction");


// Elements
const sliderElement = document.getElementById("slider");

const btnLCD = document.getElementById("btn-lcd");
const btnCtrl = document.getElementById("btn-ctrl");
const btnA = document.getElementById("btn-a");
const btnB = document.getElementById("btn-b");
const btnStart = document.getElementById("btn-start");
const btnStart2 = document.getElementById("btn-start2");


const cs = {};
properties.forEach((property) => (cs[property] = 0));
displayValues();

function connect() {
  ws = new WebSocket(websocketURL);

  ws.onopen = function () {
    // subscribe to some channels
    console.log("Connected.");
    reconnectBluetooth();
    getAll();
    platformType();
    mode();
  };

  ws.onmessage = function (e) {
    //console.log('Message:', e.data);
    newMessage(e);
  };

  ws.onclose = function (e) {
    console.log(
      "Socket is closed. Reconnect will be attempted in 1 second.",
      e.reason
    );
    setTimeout(function () {
      connect();
    }, 1000);
  };

  ws.onerror = function (err) {
    console.error("Socket encountered error: ", err.message, "Closing socket");
    ws.close();
  };


  changeStartButtonState("standby");
}

connect();

addEventListener("message", (event) => {});


function newMessage(event) {
  //Parse JSON string.
  const data = JSON.parse(event.data);

  //Log the data
  console.log(data);

  if ("Companion" in data) {
    if (data.Companion.RunCommand == 1) {
      if (buttonState == "standby") {
        standby();
        return;
      } else if (buttonState == "start") {
        videoStart();
        if (cs.LoopCmd == 0) videoLoop();
      }
    } else if (data.Companion.RunCommand == 0) { //RunCommand 0
      if (buttonState == "stop") videoStop();
    }
    if (data.Companion.Loop == 1) videoLoop();
    return;
  }

  //Save the data from input.
  Object.entries(data).forEach(([key, value]) => {
    Object.entries(value).forEach(([nestedKey, nestedValue]) => {


      // Do not collect PanPos or SliderPos if SetA or SetB is 255.
      if (value["SetA"] == 255 && nestedKey == "PanAPos") {
        return;
      }
      if (value["SetB"] == 255 && nestedKey == "PanBPos") {
        return;
      }
      if (value["SetA"] == 255 && nestedKey == "SliderAPos") {
        return;
      }
      if (value["SetB"] == 255 && nestedKey == "SliderBPos") {
        return;
      }


      // Collect values from json input.
      if (nestedValue !== 255 && nestedValue !== 65535) {
        cs[`${nestedKey}`] = nestedValue;
      }
    });
  });

  updateBtnBackgrounds();
  displayValues();
  updateBattery();

  //Below is what will need to happen at each different input.

  if ("AdjPosition" in data) {
    cs.Pan1 = cs.PanActualPos;
  }

  if ("PositionInfo" in data) {
    if (runState === false) {
      cs.OriginStatus === 1 ? changeStartButtonState("start") : changeStartButtonState("standby");
    }
  }

  //Set runState etc.
  if ("VideoStart" in data) {
    if (data.VideoStart.StartMode === 2) {
      changeStartButtonState("stop");
      runState = true;

    } else if (data.VideoStart.AllDone === 0 && data.VideoStart.StartMode !== 0) {
      runState = true;
      changeStartButtonState("stop");

    } else if (data.VideoStart.StartMode === 0) {
      runState = false;
      changeStartButtonState("standby");

    } else if (data.VideoStart.AllDone === 1 && runState == true) {
      console.log("Run completed.");
      runState = false;
      changeStartButtonState("standby");
      changeDirectionIfAorB();

    }
  }

  if ("BasicInfo" in data) {
    document.getElementById("onTime").innerText = convertSecondsToHMS(cs.SysTemTime)
  }

  if ("ExchangeDirection" in data) {
    cs.RunDir = 1 - cs.RunDir;
    callBack();
  }

  if ("ReConnect" in data) {
    cs.RunDir = cs.Direction;
    cs.SetA = cs.SetAStatus;
    cs.SetB = cs.SetBStatus;
  }

  if ("ManualCallBack" in data) {
    manualCallBack();
  }

  if ("SetPoint" in data) {
    speedChanged();
  }

  if (runState == false && ("PositionInfo" in data || "AdjPosition" in data)) {
    updatePositions();
  } else if (runState == true) {
    runningPositions(cs.Progress);
  }
}

function convertSecondsToHMS(seconds) {
  const hours = Math.floor(seconds / 3600);
  const minutes = Math.floor((seconds % 3600) / 60);
  const remainingSeconds = seconds % 60;

  const formattedHours = String(hours).padStart(2, "0");
  const formattedMinutes = String(minutes).padStart(2, "0");
  const formattedSeconds = String(remainingSeconds).padStart(2, "0");

  return formattedHours + ":" + formattedMinutes + ":" + formattedSeconds;
}

function updateBattery() {
  const batLogo = document.getElementById("batteryLogo");
  const batLevel = document.getElementById("batteryLevel");

  let state = "dead";
  if (cs.Battery > 20) state = "half";
  if (cs.Battery > 70) state = "full";
  if (cs.ChargeStatus == 1) state = "charging";

  batLogo.src = `./icons/bat/bat-${state}.svg`;

  batLevel.innerText = cs.Battery + "%";
}



function changeStartButtonState(state) {
  if (state == "stop") {
    buttonState = "stop";
    showButtons("stop");

  } else if (state == "standby") {
    buttonState = "standby";
    showButtons("standby");

  } else if (state == "start") {
    buttonState = "start";
    showButtons("start");
  }
}

function showButtons(show) {
  standbyButton.style.display = "none";
  videoStartButton.style.display = "none";
  videoStopBtn.style.display = "none";

  if (show == "standby") {
    standbyButton.style.display = "block";
    btnCtrl.style.display = "block";
    directionBtn.style.display = "block";

  } else if (show == "stop") {
    videoStopBtn.style.display = "block";
    btnCtrl.style.display = "none";
    directionBtn.style.display = "none";


  } else if (show == "start") {
    videoStartButton.style.display = "block";
    directionBtn.style.display = "block";
    btnCtrl.style.display = "block";


  }
}

function displayValues() {
  const propertiesElement = document.getElementById("properties");
  // propertiesElement.innerHTML = properties
  //   .map((v) => `<p id="${v}">${v}: <span>${cs[v]}</span></p>`)
  //   .join("");
  propertiesElement.innerText = '';
  for (const [key, value] of Object.entries(cs)) {
    propertiesElement.innerText += `${key}: ${value}\n`;
  }
}

function updateBtnBackgrounds() {
  gsapChange("btn-lcd", cs.LcdStatus);
  gsapChange("btn-ctrl", cs.TorqueStatus);
  handleAB("btn-a", cs.SetA);
  handleAB("btn-b", cs.SetB);
  gsapChange("loopButton", cs.LoopCmd);
  gsapChange("direction", cs.RunDir);
}

function handleAB(ID, value) {
  if (value == 255) {
    gsapfunc(ID, "#121212", "white");
  } else if (value == 1) {
    gsapfunc(ID, "red", "white");
  } else if (value == 0) {
    gsapfunc(ID, "#232323", "fff");
  }
}

function gsapfunc(ID, color, altcolor) {
  gsap.to(document.getElementById(ID), {
    // this is the vars object
    // it contains properties to animate
    background: color,
    color: altcolor,
    // and special properties
    duration: 1,
  });
}

/**
 * Change background color using gsap.
 */
function gsapChange(ID, toggle) {
  if (!toggle) {
    // Turn on
    gsap.to(document.getElementById(ID), {
      // this is the vars object
      // it contains properties to animate
      background: "#121212",
      color: "white",
      // and special properties
      duration: 1,
    });
  } else {
    // Turn off
    gsap.to(document.getElementById(ID), {
      // this is the vars object
      // it contains properties to animate
      background: "red",
      color: "white",
      // and special properties
      duration: 1,
    });
  }
}

function toggleLCD() {
  cs.LcdStatus = 1 - cs.LcdStatus;

  let obj = {
    CtrlLcd: {
      LcdOnOff: cs.LcdStatus,
      RetryCount: 0,
      Seril: 2,
    },
  };

  sendJSON(obj);
}

function toggleCtrl() {

  let obj = {
    TorqueCtrl: {
      CtrlCmd: 1 - cs.TorqueStatus,
      RetryCount: 0,
      Seril: 5,
    },
  };

  sendJSON(obj);
}

function toggleA() {
  cs.SetA = 1 - cs.SetA;

  let obj = {
    SetPoint: {
      SetA: cs.SetA,
      SetB: 255,
      RetryCount: 0,
      Seril: 6,
    },
  };
  sendJSON(obj);
}

function toggleB() {
  cs.SetB = 1 - cs.SetB;

  let obj = {
    SetPoint: {
      SetA: 255,
      SetB: cs.SetB,
      RetryCount: 0,
      Seril: 6,
    },
  };
  sendJSON(obj);
}

function mode() {
  let obj = {
    "Mode": {
      "Platform": 1,
      "RunMode": 1,
      "RetryCount": 0,
      "Seril": 3
    }
  }
  sendJSON(obj);
}

function manualCallBack() {
  let obj = {
    "ManualCallBack": {
      "Code": cs.Code,
      "RetryCount": 0,
      "Seril": cs.Seril
    }
  }
  sendJSON(obj);
}

function platformType() {
  let obj = {
    "PlatformType": {
      "Platform": 161,
      "RetryCount": 0,
      "Seril": 0
    }
  }
  sendJSON(obj);
}

function videoLoop() {
  let obj = {
    Loop: {
      LoopCmd: 1 - cs.LoopCmd,
      RetryCount: 0,
      Seril: 8,
    }
  }
  sendJSON(obj);
}

function directionButton() {
  let obj = {
    ExchangeDirection: {
      Direction: 1 - cs.RunDir,
      RetryCount: 0,
      Seril: 8,
    }
  }
  sendJSON(obj);
}

function reconnectBluetooth() {
  let obj = {
    Reconnect: {
      Reqstatus: 1,
      RetryCount: 0,
      Seril: 33,
    },
  };
  sendJSON(obj);
}

function getAll() {
  let obj = {
    DeviceInfo: {
      ReqInfo: 1,
      RetryCount: 0,
      Seril: 1,
    }
  };

  sendJSON(obj);
}

function sendJSON(obj) {
  ws.send(JSON.stringify(obj));
}




const videoTime = document.getElementById("videoTime");
const videoSpeed = document.getElementById("videoSpeed");


function speedChanged() {
  if (videoSpeed.value > 44) {
    alert("Speed should not exceed 44%.");
  }
  videoTime.value = Math.round(450 * Math.abs(cs.SliderAPos - cs.SliderBPos) / videoSpeed.value / 100);
}

function timeChanged() {
  var oldSpeed = videoSpeed.value;
  videoSpeed.value = Math.round(14400 / videoTime.value / 100);
  if (videoSpeed.value > 44) {
    alert("Speed should not exceed 44%.");
    videoSpeed.value = oldSpeed;
  }
}



function videoParam() {
  let obj = {
    VideoParam: {
      Delay: 0,
      LoopCmd: cs.LoopCmd,
      Shutter: -1,
      Speed: videoSpeed.value * 10,
      Time: videoTime.value * 1000,
      RetryCount: 0,
      Seril: 18,
    },
  };
  sendJSON(obj);
}

function callBack() {
  let obj = {
    CallBack: {
      Code: cs.Code,
      RetryCount: 0,
      Seril: cs.Seril,
    },
  };
  sendJSON(obj);
}

function changeDirectionIfAorB() {
  //Make slider start from origin.
  if (cs.Slider1 == cs.SliderBPos) {
    directionButton();
  } else if (cs.Slider1 == cs.SliderAPos) {
    directionButton();
  }
}

function standby() {

  changeDirectionIfAorB();

  obj = {
    BackOrigin: {
      Direction: cs.RunDir,
      OriginCmd: cs.OriginStatus,
      OriginCmd: 1,
      RetryCount: 0,
      Seril: 7
    },
  };

  sendJSON(obj);
}

function videoStart() {

  videoParam();

  let obj = {
    VideoStart: {
      Direction: cs.RunDir,
      RepType: 0,
      VideoRun: 3,
      RetryCount: 0,
      Seril: 19,
    },
  };
  sendJSON(obj);
  changeStartButtonState("stop");
}

function videoStop() {
  obj = {
    VideoStart: {
      Direction: cs.RunDir,
      RepType: 0,
      VideoRun: 0,
      RetryCount: 0,
      Seril: 19,
    },
  };
  sendJSON(obj);
}

function updateVelocity() {
  let obj = {
    AdjPosition: {
      P_Velocity: Math.round(panSpeed),
      //PanPos: 0,
      S_Velocity: Math.round(sliderSpeed),
      //SliderPos: 0,
      //TurnDir: 0,
      //RetryCount: 0,
      Seril: 4,
    },
  };
  sendJSON(obj);
}



const sliderStickElement = document.getElementById("sliderstick");
const panStickElement = document.getElementById("panstick");

const timeout = 100;
let speed = document.getElementById("sliderpanSpeed").value;

let lastSliderValueSent = 0;
let lastSliderSent = Date.now() - timeout;

let lastPanValueSent = 0;
let lastPanSent = Date.now() - timeout;

let sliderSpeed = 0;
let panSpeed = 0;


sliderFactory(sliderStickElement, (value) => {
  sliderSpeed = value * speed;
  if (!cs.TorqueStatus) return; // Don't run if CTRL btn is red.

  if (sliderSpeed === 0 || Date.now() - lastSliderSent > timeout) {
    lastSliderValueSent = sliderSpeed;
    lastSliderSent = Date.now();
    updateVelocity();
  }
});

sliderFactory(panStickElement, (value) => {
  panSpeed = value * speed;
  if (!cs.TorqueStatus) return; // Don't run if CTRL btn is red.

  if (panSpeed === 0 || Date.now() - lastPanSent > timeout) {
    lastPanValueSent = panSpeed;
    lastPanSent = Date.now();
    updateVelocity();
  }
});

function sliderpanspeedChanged() {
  speed = document.getElementById("sliderpanSpeed").value;
  if (speed > 20) {
    alert("Please select a value between 1 and 20");
    document.getElementById("sliderpanSpeed").value = 15;
  }
}

function updatePositions() {
  moveElement("actual-panContainer", cs.Slider1, cs.Pan1);
  moveElement("A-panContainer", cs.SliderAPos, cs.PanAPos);
  moveElement("B-panContainer", cs.SliderBPos, cs.PanBPos);
}


function moveElement(ID, sliderVal, panVal) {
  const normalizedPos = (sliderVal / 100 - 0.1) * 160;
  const element = document.getElementById(ID);

  element.style.setProperty("--rot", panVal + "deg");
  element.style.setProperty("--pos", normalizedPos + "px");
}

function runningPositions(percentage) {
  const runningDirection = cs.RunDir === 1 ? -1 : 1;

  let sliderendValue = cs.SliderAPos;
  let sliderstartValue = cs.SliderBPos;
  let panendValue = cs.PanAPos;
  let panstartValue = cs.PanBPos;
  if (cs.RunDir === 0) {
    sliderendValue = cs.SliderBPos;
    sliderstartValue = cs.SliderAPos;
    panendValue = cs.PanBPos;
    panstartValue = cs.PanAPos;
  }

  // Calculate the difference between the start and end values
  var sliderdifference = sliderendValue - sliderstartValue;
  var pandifference = panendValue - panstartValue;

  // Calculate the new value based on the percentage
  var sliderVal = sliderstartValue + (sliderdifference * percentage / 100);
  var panVal = panstartValue + (pandifference * percentage / 100);

  const element = document.getElementById("actual-panContainer");

  //Move element accordingly
  moveElement("actual-panContainer", sliderVal, panVal);
}