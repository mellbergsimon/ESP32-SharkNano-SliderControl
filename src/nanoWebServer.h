#ifndef nanoWebServer_H
#define nanoWebServer_H

// Librarys
#include <WiFi.h>
#include <wifiinfo.h>
#include "nanoBluetooth.h"
#include <ETH.h>

#include "ESPAsyncWebServer.h"
#include "ESPmDNS.h"
#include "FS.h"
#include "SPIFFS.h"

void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void websetup();
void sendJsontoWeb(char *jsonInput, int length);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);



#endif


