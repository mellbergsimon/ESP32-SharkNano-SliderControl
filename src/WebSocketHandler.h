#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <functional>



class WebSocketHandler {
public:
    WebSocketHandler(uint16_t port, const char* path);
    
    void handleWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);
    void handleWebSocketData(char* data, size_t len);

    void sendJSONtoWeb(char* data);


    //Callback;
    void setCallback(std::function<void(char*)> callback);
    
private:
    AsyncWebServer server;
    AsyncWebSocket webSocket;

    std::function<void(char*)> dataCallback;
};

#endif //WEBSOCKETHANDLER_H







// // WebSocketHandler.h
// #ifndef WebSocketHandler_H
// #define WebSocketHandler_H


// #include "ESPAsyncWebServer.h"

// #include <vector>


// class WebSocketHandler {
// public:

//     WebSocketHandler(const char* websock, AsyncWebServer server);

//     void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
//     void handleRequest(WebSocketHandler *handler, AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

//     void sendJsonToWebSocket(const char *jsonInput, int length);


//     void printStatus();
//     AsyncWebSocket& getWebSocket();
// private:

//     AsyncWebSocket webSocket;
    
//     std::vector<char> buffer;

// };

// #endif


// #ifndef nanoWebServer_H
// #define nanoWebServer_H

// // Librarys
// #include <WiFi.h>
// #include <wifiinfo.h>
// #include "nanoBluetooth.h"
// #include <ETH.h>

// #include "bmcc.h"

// #include "ESPAsyncWebServer.h"
// #include "ESPmDNS.h"
// #include "FS.h"
// #include "SPIFFS.h"





// void handleData(char* data);
// void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
// void websetup();
// void sendJsontoWeb(char *jsonInput, int length);
// void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
// #endif