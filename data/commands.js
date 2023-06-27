const commands = {
  Toogle: {
    SetPoint: {
      SetA: 255,
      SetB: 255,
      RetryCount: 0,
      Seril: 6,
    }
  },
  Mode: {
    Mode: {
      Platform: 1,
      RunMode: 1,
      RetryCount: 0,
      Seril: 3
    }
  }
};


function toggleA() {
  const obj = commands.Toogle;

  cs.SetA = 1 - cs.SetA;

  obj.SetPoint.SetA = cs.SetA;
  obj.SetPoint.SetB = 255;

  sendJSON(obj);
}

function toggleB() {
  const obj = commands.Toogle;

  cs.SetB = 1 - cs.SetB;

  obj.SetPoint.SetA = 255;
  obj.SetPoint.SetB = cs.SetB;

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