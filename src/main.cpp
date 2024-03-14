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



// const char *ssid = WIFINAME;
// const char *password = WIFIPASS;

// BMCC *bmc;
// Ethernet *eth;
// WebSocketHandler *wsHandler;
// BluetoothHandler *blHandler;

encoder *rotary;
OledDisplay *Display;
Button2 btn1;
Database * db;

std::string BMCCKey = "BMCC";
std::string BLEKey = "BLE";
std::string WIFIKey = "WIFI";

void btn1_click(Button2& btn);
void initDatabase();

unsigned int currentListPosition;

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


void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.print("\n\nStarting...\n");
  
  // eth = new Ethernet(ssid, password);
  // wsHandler = new WebSocketHandler(80, "/ws");

  // wsHandler->setCallback(onWsEvent);

  // blHandler = new BluetoothHandler();

  // blHandler->connect();

  // blHandler->setCallback(onBLeEvent);

  // bmc = new BMCC(&Serial1, RXD2, TXD2, true);


  Display = new OledDisplay();

  rotary = new encoder(GPIO_NUM_32, GPIO_NUM_33);
  rotary->setCounter(0);

  btn1.begin(GPIO_NUM_25);
  btn1.setTapHandler(btn1_click);

  db = new Database();
  initDatabase();

  currentListPosition = 0;

} // End of setup.


// Increment menu and update value of encoder.
void btn1_click(Button2& btn){

  Serial.println("Click!");
  db->incrementList(BMCCKey);
  rotary->setCounter(db->getSelectedItem(BMCCKey)->selectedParameter);

  Display->scrollList(db);
  // Serial.print("Current list position: ");

  // currentListPosition = (currentListPosition+1) % MenuItems->size();

  // Serial.print(currentListPosition);
  // Serial.println(" Menu list size: ");
  // Serial.println(MenuItems->size());
  // rotary->setCounter(MenuItems->get(currentListPosition).pos);


  // Display->scrollList(MenuItems, currentListPosition);
  
}




void initDatabase(){

  List * list_BLE = db->createList();

  db->addContent(BLEKey, list_BLE);
  
  List * list_WIFI = db->createList();

  db->addContent(WIFIKey, list_WIFI);

  List * list_BMCC = db->createList();

  const char * buffer_wb[] = {"2500K", "2800K", "3000K", "3200K", "3400K", "3600K", "4000K", "4500K", "4800K", "5000K", "5200K", "5400K", "5600K", "6000K", "6500K", "7000K", "7500K", "8000K", nullptr};

  list_BMCC->items->push(db->createItem("WB", buffer_wb));
  // Initialize buffer for "ISO" item
  const char * buffer_iso[] = {"200", "400", "800", "1600", nullptr};
  list_BMCC->items->push(db->createItem("ISO", buffer_iso));

  const char * buffer_ShutterAngle[] = {"11,25", "15", "22,5", "30", "37,5", "45", "60", "72", "75", "90", "108", "120", "144", "150", "172,8", "180", "216", "270", "324", "360", nullptr};
  list_BMCC->items->push(db->createItem("ShutterAngle", buffer_ShutterAngle));

  const char * buffer_Percent[] = {"10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%", nullptr};

  list_BMCC->items->push(db->createItem("Aperture", buffer_Percent));
  db->addContent(BMCCKey, list_BMCC);

  

  // MenuItems->push(ListItem(0, {"hej"}, "AutoFocus"));
  // MenuItems->push(ListItem(0, {"hej"}, "Zoom"));
  // MenuItems->push(ListItem(0, {"hej"}, "ShutterAngle"));
  // MenuItems->push(ListItem(0, {"hej"}, "WhiteBalance"));
  // MenuItems->push(ListItem(0, {"hej"}, "FrameRate"));
  // MenuItems->push(ListItem(0, {"hej"}, "Codec"));

}


item * currentItems;
void loop() {
  
  currentItems = db->getSelectedItem(BMCCKey);
  
  Serial.print("SelectedItem: ");
  Serial.print(db->getSelectedItem(BMCCKey)->name);
  Serial.print(" SelectedParameter: ");
  Serial.print(db->getSelectedParameter(BMCCKey));
  Serial.print(" Rotary: ");
  Serial.print(rotary->getCounter());

  Serial.println("");

  //Serial.println(buffer);
  //Display->print(buffer);
  db->updateSelectedParameter(std::string(db->selectedKey), rotary->getCounter());


  Display->printMenuWithDatabase(db);

  // Serial.print("Rotary: ");
  // Serial.print(rotary->getCounter());
  // Serial.print(" selectedParameter: ");
  // Serial.print(db->getSelectedParameter(BMCCKey));
  // Serial.println(" ");

  //Display->printList(MenuItems, currentListPosition);

  Serial.print(" Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.print(" MaxAllocHeap: ");
  Serial.print(ESP.getMaxAllocHeap());
  Serial.print(" MinAllocHeap: ");
  Serial.print(ESP.getMinFreeHeap());
  Serial.print(" - ");
  btn1.loop();

}