// let serverURL = "http://sliderserver.local";
let serverURL = "http://192.168.1.79";
// let websocketURL = "ws://sliderserver.local/ws";
let websocketURL = "ws://192.168.1.79/ws";
var ws;


function connect() {
  ws = new WebSocket(websocketURL);

  ws.onopen = function () {
    // subscribe to some channels
    console.log("Connected.");
  };

  ws.onmessage = function (e) {
    //console.log('Message:', e.data);
    //newMessage(e);
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
}


function sendJSON(obj) {
  ws.send(JSON.stringify(obj));
}

connect();