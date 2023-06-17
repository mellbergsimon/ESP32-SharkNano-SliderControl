/**
 * Simmes controlprogge
 */

let serverURL = "http://sharknano-server.local";
let websocketURL = "ws://sharknano-server.local/ws";
var ws;

//Statemachine
let buttonState = "standby";
let backOriginCounter = 0;
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
  const data = JSON.parse(event.data);
  console.log(data);

  infos.forEach((v) => Object.assign(cs, data[v]));

  cs.Pan1 = cs.PanActualPos;
  // cs.Slider1 = cs.SliderActualPos;
  if (data.PositionInfo || data.AdjPosition) updatePositions();

  updateBtnBackgrounds();
  displayValues();
  updateBattery();
  //updatePanPositions();


  if ("BackOrigin" in data) {
    buttonState = "standby";
    backOriginCounter++;

    console.log("backOrigin received. buttonCounter is: " + backOriginCounter);
    if (buttonState === "standby" && backOriginCounter > 1) {
      changeStartButtonState("start");
      backOriginCounter = 0;
    }

  }

  if ("VideoStart" in data) {
    if (data.VideoStart.AllDone === 1) {

      console.log("Done");
      changeStartButtonState("standby");

    }
  }

  document.getElementById("whatevertext").innerText = "Buttonstate: " + buttonState;
}

function updatePanPositions() {
  console.log(cs.SliderAPos + " " +
    cs.PanAPos + " " +
    cs.SliderBPos + " " +
    cs.PanBPos + " loopCMD is: " + cs.LoopCmd);
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
  console.log("changeBtnState: " + state);
  if (state == "stop") {
    buttonState = "stop";
    showButton("stop");

  } else if (state == "standby") {
    buttonState == "standby";
    showButton("standby");

  } else if (state == "start") {
    buttonState == "start";
    showButton("start");
  }
}


function showButton(show) {
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
  gsapChange("btn-ctrl", cs.CtrlCmd);
  handleAB("btn-a", cs.SetA);
  handleAB("btn-b", cs.SetB);
  gsapChange("loop", cs.LoopCmd);
  gsapChange("direction", cs.RunDir);
}

function handleAB(ID, value) {
  if (value == 255) {
    gsapfunc(ID, "red", "rgb(40, 6, 6)");
  } else if (value == 1) {
    gsapfunc(ID, "rgb(15, 97, 163)", "rgb(5, 30, 51)");
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
      background: "red",
      color: "rgb(40, 6, 6)",
      // and special properties
      duration: 1,
    });
  } else {
    // Turn off
    gsap.to(document.getElementById(ID), {
      // this is the vars object
      // it contains properties to animate
      background: "rgb(15, 97, 163)",
      color: "rgb(5, 30, 51)",
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
  cs.CtrlCmd = 1 - cs.CtrlCmd;

  let obj = {
    TorqueCtrl: {
      CtrlCmd: cs.CtrlCmd,
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

function loopButton() {
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

function videoParam() {
  let obj = {
    VideoParam: {
      Delay: 0,
      LoopCmd: cs.LoopCmd,
      Shutter: -1,
      Speed: 1,
      Time: 20000,
      RetryCount: 0,
      Seril: 18,
    },
  };
  sendJSON(obj);
}

function standby() {
  obj = {
    BackOrigin: {
      Direction: cs.RunDir,
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
  // @TR{"VideoStart":{"Direction":1,"RepType":0,"VideoRun":3,"RetryCount":1,"Seril":19}}
  console.log("cs.Direction: " + cs.Direction);
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
  changeStartButtonState("standby");
}



function updateVelocity() {
  let obj = {
    AdjPosition: {
      P_Velocity: Math.round(panSpeed),
      PanPos: 0,
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
const speed = 20;

let lastSliderValueSent = 0;
let lastSliderSent = Date.now() - timeout;

let lastPanValueSent = 0;
let lastPanSent = Date.now() - timeout;

let sliderSpeed = 0;
let panSpeed = 0;

sliderFactory(sliderStickElement, (value) => {
  sliderSpeed = value * speed;
  if (!cs.CtrlCmd) return; // Don't run if CTRL btn is red.

  // if (sliderSpeed === 0 || (sliderSpeed != lastSliderValueSent && Date.now() - lastSliderSent > timeout)) {
  if (sliderSpeed === 0 || Date.now() - lastSliderSent > timeout) {
    lastSliderValueSent = sliderSpeed;
    lastSliderSent = Date.now();
    updateVelocity();
    changeStartButtonState("standby");
  }
});

sliderFactory(panStickElement, (value) => {
  panSpeed = value * speed;
  if (!cs.CtrlCmd) return; // Don't run if CTRL btn is red.

  // if (panSpeed === 0 || (panSpeed != lastPanValueSent && Date.now() - lastPanSent > timeout)) {
  if (panSpeed === 0 || Date.now() - lastPanSent > timeout) {
    lastPanValueSent = panSpeed;
    lastPanSent = Date.now();
    updateVelocity();
    changeStartButtonState("standby");
  }
});

function updatePositions() {
  const normalizedPos = (cs.Slider1 / 100 - 0.1) * 160;
  const sliderpan = document.getElementById("actual-panContainer");

  sliderpan.style.setProperty("--rot", cs.Pan1 + "deg");
  sliderpan.style.setProperty("--pos", normalizedPos + "px");
}