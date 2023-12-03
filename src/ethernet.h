// WebSocketHandler.h
#ifndef ethernet_H
#define ethernet_H

#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ETH.h>

#include "ESPmDNS.h"
#include "FS.h"
#include "SPIFFS.h"



class Ethernet {
public:

    Ethernet(const char* WIFINAME, const char* WIFIPASS);

    void checkStatus();


    // void initHTTPHTML(AsyncWebServer *server);
private:

    void initWifi(const char* WIFINAME, const char* WIFIPASS);
    // void initSpiffs();
    void initMDNS();
    
};

#endif