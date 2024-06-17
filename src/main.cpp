#include "bmcc.h"
#include "WebSocketHandler.h"
#include "BluetoothHandler.h"
#include "ethernet.h"
#include "wifiinfo.h"
#include "Button2.h"
#include "OledDisplay.h"
#include "encoder.h"
#include "database.h"

#define RXD2 GPIO_NUM_27
#define TXD2 GPIO_NUM_14

const char *ssid = WIFINAME;
const char *password = WIFIPASS;

// Ethernet *eth;
// WebSocketHandler *wsHandler;
// BluetoothHandler *blHandler;

BMCC *bmcc;
encoder *rotary;
OledDisplay *Display;
Button2 btn1;
Database *db;

std::string BMCCKey = "BMCC";
std::string BLEKey = "BLE";
std::string WIFIKey = "WIFI";

void btn1_click(Button2 &btn);
void btn1_doubleclick(Button2 &btn);

// void onWsEvent(char* data) {
//   // Create a copy of the original data
//   char* dataCopy = strdup(data);

//   // Parse the JSON data
//   DynamicJsonDocument doc(1024);
//   deserializeJson(doc, dataCopy);

//   // Check if the JSON data contains the "BMCC" key
//   if (doc.containsKey("BMCC")) {

//     bmc->updateBMCCFromJSON(data);
//   } else if (doc.containsKey("ReconnectBLE")){
//     Serial.println("Reconnect BLE.");
//     blHandler->connect();
//   } else {
//     blHandler->writeToBLE(data);
//   }
// }

// void onBLeEvent(char* data){
//   //Serial.print("Got BLE Event: ");
//   //Serial.println(data);
//   wsHandler->sendJSONtoWeb(data);
// }
int oldCounter;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.print("\n\nStarting...\n");

  // eth = new Ethernet(ssid, password);
  // wsHandler = new WebSocketHandler(80, "/ws");

  // wsHandler->setCallback(onWsEvent);

  // blHandler = new BluetoothHandler();

  // blHandler->connect();

  // blHandler->setCallback(onBLeEvent);

  bmcc = new BMCC(&Serial1, RXD2, TXD2, true);

  // Display = new OledDisplay();

  rotary = new encoder(GPIO_NUM_32, GPIO_NUM_33);
  rotary->setCounter(0);

  // btn1.begin(GPIO_NUM_14);
  // btn1.setClickHandler(btn1_click);
  // btn1.setDoubleClickHandler(btn1_doubleclick);

  oldCounter = 0;

} // End of setup.

void btn1_click(Button2 &btn)
{
  bmcc->sendSBUSPacket();
}

void btn1_doubleclick(Button2 &btn)
{
  Serial.println("DoubleClick!");
}

void loop()
{

  if (oldCounter != rotary->getCounter() * 10)
  {

    bmcc->setFocus(rotary->getCounter() * 10);
    Serial.printf("Rotary: %d , Focus: %d\n", rotary->getCounter() * 10, bmcc->getFocus());
    bmcc->sendSBUSPacket();
  }

  oldCounter = rotary->getCounter() * 10;

  //btn1.loop();
}