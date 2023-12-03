#ifndef BluetoothHandler_H
#define BluetoothHandler_H

#include "NimBLEDevice.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000FFF0-0000-1000-8000-00805F9B34FB");
static BLEUUID writeUUID("0000FFF2-0000-1000-8000-00805F9B34FB");
static BLEUUID readUUID("0000FFF1-0000-1000-8000-00805F9B34FB");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pWChr;
static BLERemoteCharacteristic* pRChr;
static BLEAdvertisedDevice* myDevice;

class BluetoothHandler {
  public:
    BluetoothHandler();
    void connect();
    void BluetoothEvent(  BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    void writeToBLE(String packet);

    //Callback;
    void setCallback(std::function<void(char*)> callback);

  private:
    bool connectToServer();
    bool isJsonComplete(const char* data, size_t length);
    void removeWhitespace(char* str);


    void handleJson(char *jsonstr);

    std::function<void(char*)> dataCallback;
};



class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("Ble disconnected");
  }
};

// /**
//  * Scan for BLE servers and find the first one that advertises the service we are looking for.
//  */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice* advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice->toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
/********************************************************************************
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
********************************************************************************/
    if (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(serviceUUID)) {
      Serial.println("BLE has our UUID");
      BLEDevice::getScan()->stop();
/*******************************************************************
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
*******************************************************************/
      myDevice = advertisedDevice; /** Just save the reference now, no need to copy the object */
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks





#endif