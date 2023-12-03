#include "Ethernet.h"



Ethernet::Ethernet(const char* WIFINAME, const char* WIFIPASS){
  // Connect to WIFI.
  
  this->initWifi(WIFINAME, WIFIPASS);

  //this->initSpiffs();

  this->initMDNS();

  //this->initHTTPHTML();

  //server.begin();
  
}

void Ethernet::initWifi(const char* WIFINAME, const char* WIFIPASS) {
  Serial.print("Connecting to ");
  Serial.println(WIFINAME);

  int attempts = 0;
  const int maxAttempts = 20;

  while (attempts < maxAttempts) {
    WiFi.begin(WIFINAME, WIFIPASS);

    int status = WiFi.waitForConnectResult();
    if (status == WL_CONNECTED) {
      Serial.println("\nWi-Fi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return; // Successful connection, exit the loop
    } else {
      Serial.print("Failed to connect to Wi-Fi. Status: ");
      Serial.println(status);
    }

    delay(500);
    attempts++;
  }

  Serial.println("Exceeded maximum attempts. Unable to connect to Wi-Fi.");
  // Handle connection failure, e.g., implement a retry mechanism or take appropriate action
}


// void Ethernet::initSpiffs(){
//   // Start SPIFFS
//   if (!SPIFFS.begin()) {
//       Serial.println("An Error has occurred while mounting SPIFFS");
//       return;
//   } else {
//       Serial.println("SPIFFS started successfully.");
//   }
// }

void Ethernet::initMDNS(){
  Serial.println("Initializing MDNS 'sliderserver'");
  if(MDNS.begin("sliderserver")){
    Serial.println("MDNS started successfully.");
  } else {
    Serial.println("MDNS failed to start.");
  }
}

// void Ethernet::initHTTPHTML(AsyncWebServer *server){
//   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
//   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT");
//   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

//   server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//     request->send(SPIFFS, "/index.html", "text/html");
//   });
//   server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
// }



void Ethernet::checkStatus() {
    static wl_status_t prevStatus = WL_IDLE_STATUS;

    wl_status_t status = WiFi.status();

    if (status != prevStatus) {
        switch (status) {
            case WL_IDLE_STATUS:
                Serial.println("Idle status");
                break;
            case WL_NO_SSID_AVAIL:
                Serial.println("No SSID available");
                break;
            case WL_SCAN_COMPLETED:
                Serial.println("Scan completed");
                break;
            case WL_CONNECTED:
                Serial.print("Connected to: " + WiFi.SSID() + " at: ");
                Serial.println(WiFi.localIP());
                break;
            case WL_CONNECT_FAILED:
                Serial.println("Connection failed");
                break;
            case WL_CONNECTION_LOST:
                Serial.println("Connection lost");
                break;
            case WL_DISCONNECTED:
                Serial.println("Disconnected");
                break;
            default:
                Serial.println("Unknown status");
                break;
        }

        prevStatus = status;
    }
}
