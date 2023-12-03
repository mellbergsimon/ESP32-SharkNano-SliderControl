#include "bmcc.h"
#include "WebSocketHandler.h"
#include "BluetoothHandler.h"
#include "ethernet.h"
#include "wifiinfo.h"

#define RXD2 GPIO_NUM_27
#define TXD2 GPIO_NUM_14



const char *ssid = WIFINAME;
const char *password = WIFIPASS;

BMCC *bmc;
Ethernet *eth;
WebSocketHandler *wsHandler;
BluetoothHandler *blHandler;

void onWsEvent(char* data) {
  // Create a copy of the original data
  char* dataCopy = strdup(data);

  // Parse the JSON data
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, dataCopy);

  // Check if the JSON data contains the "BMCC" key
  if (doc.containsKey("BMCC")) {

    bmc->updateBMCCFromJSON(data);
  } else if (doc.containsKey("ReconnectBLE")){
    Serial.println("Reconnect BLE.");
    blHandler->connect();
  } else {
    blHandler->writeToBLE(data);
  }
}

void onBLeEvent(char* data){
  //Serial.print("Got BLE Event: ");
  //Serial.println(data);
  wsHandler->sendJSONtoWeb(data);
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.print("\n\nStarting...\n");
  
  eth = new Ethernet(ssid, password);
  wsHandler = new WebSocketHandler(80, "/ws");

  wsHandler->setCallback(onWsEvent);

  blHandler = new BluetoothHandler();

  blHandler->connect();

  blHandler->setCallback(onBLeEvent);

  bmc = new BMCC(&Serial1, RXD2, TXD2, true);


} // End of setup.



void loop() {


  delay(1000); // Delay a second between loops.
} // End of loop
