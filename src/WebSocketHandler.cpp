#include "WebSocketHandler.h"

WebSocketHandler::WebSocketHandler(uint16_t port, const char* path) : server(port), webSocket(path){
  Serial.println("Initializing WebSocketHandler.");

  webSocket.onEvent(std::bind(&WebSocketHandler::handleWebSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));


  server.addHandler(&webSocket);
  Serial.println("WebSocketHandler initialized.");
  server.begin();
}

void WebSocketHandler::handleWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("WebSocket client connected");
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("WebSocket client disconnected");
    } else if (type == WS_EVT_ERROR){
      Serial.println("WebSocket Error");
    } else if (type == WS_EVT_DATA) {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        // Handle WebSocket data
        handleWebSocketData((char*) data, len);
      }
    }
}

void WebSocketHandler::setCallback(std::function<void(char*)> callback) {
    dataCallback = callback;
}

void WebSocketHandler::handleWebSocketData(char* data, size_t len) {

  data[len] = '\0';

  // Check if a callback function is set and invoke it
  if (dataCallback) {
      dataCallback(data);
  }
}

void WebSocketHandler::sendJSONtoWeb(char* data){
  webSocket.textAll(data);
}









// WebSocketHandler.cpp
// #include "WebSocketHandler.h"


// WebSocketHandler::WebSocketHandler(const char* websock, AsyncWebServer server) : webSocket(websock){
    
//   Serial.println("Initializing websocket");

//   buffer.resize(400);

//   if (!Serial.available()) {
//     Serial.begin(115200);
//   }

//   webSocket.onEvent(&WebSocketHandler::onWsEvent);

//   server.addHandler(&this->webSocket);

//   Serial.println("Websocket Initialized.");
// }

// AsyncWebSocket& WebSocketHandler::getWebSocket(){
//   return this->webSocket;
// }


// void WebSocketHandler::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
//     // Handle WebSocket events here
//     Serial.println("onWsEvent:");

//     // Null-terminate the data
//     data[len] = '\0';

//     Serial.write(data, len);
//     Serial.print("\n");

//     if (type == WS_EVT_DISCONNECT) {
//         Serial.println("WebSocket Disconnected");
//     }
//     if (type == WS_EVT_CONNECT) {
//         Serial.println("WebSocket Connected!");
//     }
//     if (type == WS_EVT_ERROR) {
//         Serial.println("WebSocket Error!");
//     }
//     if (type == WS_EVT_PONG) {
//       Serial.println("Ping Pong");
//     }
//     if (type == WS_EVT_DATA) {
//         // data packet
//         AwsFrameInfo *info = (AwsFrameInfo *)arg;

//         if (info->final && info->index == 0 && info->len == len) {
//             // the whole message is in a single frame and we got all of its data
//             if (info->opcode == WS_TEXT) {
//                 data[len] = 0;
//                 // if (handler->receivedJSONCallback) {
//                 //   handler->receivedJSONCallback((char *)data);
//                 // }
//                 // Serial.println("Received some data in a single frame:");
//                 // Serial.println((char *)data);

//                 // Process the complete message (single frame) here
//                 // Note: You can use 'this->buffer' to access the complete message
//                 // and then reset or free the buffer to avoid memory leaks.
//             }
//         } else {
//             // message is comprised of multiple frames or the frame is split into multiple packets
//             if (info->index == 0) {
//                 // Allocate new buffer at the start of a new message
//                 buffer.resize(info->len);
//             }
//             // Append the data to the buffer
//             memcpy(buffer.data() + info->index, data, len);

//             if (info->final && info->message_opcode == WS_TEXT) {
//                 // if (handler->receivedJSONCallback) {
//                 //   handler->receivedJSONCallback((char *)data);
//                 // }
//                 // Serial.println("Received some data in multiple frames");
//                 // Serial.println((char *)data);

//                 // Process the complete message (multiple frames) using 'buffer'
//             Serial.println("Complete message received.");
//             }
//         }
//     }
// }


// unsigned long lastAttemptTime = 0;
// const int delayInterval = 10;

// void WebSocketHandler::sendJsonToWebSocket(const char *jsonInput, int length) {
//     while (!webSocket.availableForWriteAll() && (millis() - lastAttemptTime) < delayInterval) {
//         // Wait for WebSocket availability or timeout
//     }

//     if (webSocket.availableForWriteAll()) {
//         webSocket.textAll(jsonInput, length);
//         lastAttemptTime = millis();
//     }
// }

// void WebSocketHandler::printStatus(){
//   Serial.println("Websocket Clients: " + webSocket.count() + ".");
// }


// void WebSocketHandler::handleRequest(WebSocketHandler *handler, AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
//   if (request->contentType() == "application/json")
//   {
//     // Handle the incoming data here
//     // You can accumulate the data and process it later

//     // To send a response immediately, you can use the following code:
//     // request->send(200, "text/plain", "Data received");

//     // To send a response after processing the complete request body, you can check if index + len equals total
//     if (index + len == total)
//     {
//       // Complete request body received, process the data here
//       // String requestBody = String(reinterpret_cast<char*>(data), len);
//       // Process the request body
//       // ...
//       handler->sendJsonToWebSocket((char *)data, len);

//       data[0] = '\0';
//       //  Send a response
//       request->send(200, "text/plain", "Data received and processed");
//     }
//   }
//   else
//   {
//     Serial.println("Did not receive Companion.");
//     request->send(400, "text/plain", "Data received but not processed");
//   }
// }










// -- OLD PROGRAM HERE --



// //#include "nanoWebserver.h"

// // Replace with your network credentials
// const char *ssid = WIFINAME;
// const char *password = WIFIPASS;

// // Set web server port number to 80
// AsyncWebServer server(80);
// //AsyncWebSocket websock("/ws");

// // Inputbuffer for when the websocket message arrives in two packages.
// char *buffer = new char[400];

// // What will happen when a websocket event happens.
// void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
// {

//   if (type == WS_EVT_DISCONNECT)
//   {
//     Serial.println("WebSocket Disconnected");
//   }
//   if (type == WS_EVT_CONNECT)
//   {
//     Serial.println("WebSocket Connected!");
//   }
//   if (type == WS_EVT_DATA)
//   {
//     // data packet
//     AwsFrameInfo *info = (AwsFrameInfo *)arg;

//     if (info->final && info->index == 0 && info->len == len)
//     {
//       // the whole message is in a single frame and we got all of it's data
//       if (info->opcode == WS_TEXT)
//       {
//         data[len] = 0;
//         writeToBLE((char *)data);
//       }
//     }
//     else
//     {
//       // message is comprised of multiple frames or the frame is split into multiple packets
//       if (info->index == 0)
//         buffer = new char[info->len];
//       memcpy(buffer + info->index, data, len);

//       if ((info->index + len) == info->len && info->final && info->message_opcode == WS_TEXT)
//         writeToBLE((char *)data);
//     }
//   }
// }

// void handleData(char* data) {
//     DynamicJsonDocument doc(400); // Adjust the capacity based on your needs

//     // Deserialize the JSON data
//     DeserializationError error = deserializeJson(doc, data);
//     Serial.println("Recieved: " + doc[0]);

//     // Check if deserialization was successful and if "bmcc" is present in the JSON
//     if (error == DeserializationError::Ok && doc["command"].as<const char*>() == "BMCC") {
//         // If "bmcc" is present, call bmccdata
        
//         //bmccdata(data);
//     } else {
//         // If "bmcc" is not present, write to BLE
//         writeToBLE((char *)data);
//     }
// }

// /**
//  * @brief Handle incoming HTTP requests.
//  *
//  * @param request
//  * @param data
//  * @param len
//  * @param index
//  * @param total
//  */
// void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
// {
//   if (request->contentType() == "application/json")
//   {
//     // Handle the incoming data here
//     // You can accumulate the data and process it later

//     // To send a response immediately, you can use the following code:
//     // request->send(200, "text/plain", "Data received");

//     // To send a response after processing the complete request body, you can check if index + len equals total
//     if (index + len == total)
//     {
//       // Complete request body received, process the data here
//       // String requestBody = String(reinterpret_cast<char*>(data), len);
//       // Process the request body
//       // ...
//       sendJsontoWeb((char *)data, len);

//       data[0] = '\0';
//       //  Send a response
//       request->send(200, "text/plain", "Data received and processed");
//     }
//   }
//   else
//   {
//     Serial.println("Did not receive Companion.");
//     request->send(400, "text/plain", "Data received but not processed");
//   }
// }

// void websetup()
// {
//   // Start serial if not available
//   if (!Serial.available())
//   {
//     Serial.begin(115200);
//   }

//   // Check if Ethernet is available
//   // if (ETH.begin())
//   // {
//   //   if (ETH.linkUp())
//   //   {
//   //     Serial.println("Connecting to Ethernet");
//   //     // Enable DHCP for Ethernet
//   //     if (ETH.config(IPAddress(), IPAddress(), IPAddress(), IPAddress()))
//   //     {
//   //       Serial.println("DHCP enabled");
//   //       delay(2000); // Add a delay to allow DHCP negotiation

//   //       // Check if an IP address is obtained
//   //       if (ETH.localIP() == IPAddress(0, 0, 0, 0))
//   //       {
//   //         Serial.println("Failed to obtain IP address");
//   //       }
//   //       else
//   //       {
//   //         Serial.print("IP address: ");
//   //         Serial.println(ETH.localIP());
//   //       }
//   //     }
//   //     else
//   //     {
//   //       Serial.println("Failed to enable DHCP");
//   //     }
//   //   }
//   //   else
//   //   {
//   //     Serial.println("Ethernet cable is not connected, trying wifi.");

//       // Start the Wi-Fi connection
//       Serial.print("Connecting to ");
//       Serial.println(ssid);
//       WiFi.begin(ssid, password);

//       // Wait for Wi-Fi connection to be established
//       while (WiFi.status() != WL_CONNECTED)
//       {
//         delay(500);
//         Serial.print(".");
//       }

//       Serial.println("Wi-Fi connected");
//       Serial.print("IP address: ");
//       Serial.println(WiFi.localIP());
//     //} //If Eth phy exists.

//     // Start SPIFFS
//     if (!SPIFFS.begin())
//     {
//       Serial.println("An Error has occurred while mounting SPIFFS");
//       return;
//     }
//     else
//     {
//       Serial.println("SPIFFS started successfully.");
//     }

//     Serial.println("Initializing MDNS 'sharkNano-Server'");
//     MDNS.begin("sharkNano-server");

//     DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
//     DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT");
//     DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

//     server.onRequestBody(handleRequest);

//     websock.onEvent(onWsEvent);

//     server.addHandler(&websock);

//     server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

//     server.begin();
//   // }
// }

// /**
//  * @brief Send a json string to the websocket if the websocket is available.
//  *
//  * @param jsonInput pointer to a char array containing the json string
//  * @param length length of the array to send.
//  */
// void sendJsontoWeb(char *jsonInput, int length)
// {
//   // Send jsonInput to websock
//   while (websock.availableForWriteAll() == false)
//   {
//     delay(10); // Wait for a short duration (e.g., 10 milliseconds) before checking again
//   }
//   // Serial.print("Sending to websocket: ");
//   // Serial.println(jsonInput);
//   websock.textAll(jsonInput, length);
// }