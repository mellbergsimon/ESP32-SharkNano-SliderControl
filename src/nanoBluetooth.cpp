#include "nanoBluetooth.h"

/**
 * Global variables.
 */
const size_t BUFFER_SIZE = 1024;
char *bluetoothBuffer = new char[BUFFER_SIZE];
char *inputBuffer = new char[BUFFER_SIZE];

/**
 * @brief Checks if a jsonstring input is complete or not.
 *
 * @param data string to check
 * @param length length of data
 * @return true if the package is complete
 * @return false if the package is not complete
 */
bool isJsonComplete(const char *data, size_t length)
{
  // Check if the data starts with '{' and ends with '}' to indicate a complete JSON object
  return length > 0 && data[0] == '{' && data[length - 1] == '}';
}

/**
 * @brief Remove spaces, line breaks and tabs from the string
 *
 * @param str String to change.
 */
void removeWhitespace(char *str)
{
  int length = strlen(str);
  int count = 0;

  // Loop through the entire string
  for (int i = 0; i < length; i++)
  {
    // Only save characters that are not the ones below
    if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
    {
      str[count++] = str[i];
    }
  }

  str[count] = '\0'; // Null-terminate the modified string
}

/**
 * @brief Receives a string with one or more json objects.
 *        It will handle each one and send them to the websocket.
 *
 * @param jsonstr Json string with one or more objects.
 */
void handleJson(char *jsonstr)
{
  char buffer[BUFFER_SIZE];
  strncpy(buffer, jsonstr, BUFFER_SIZE); // Copy the message to a local buffer
  buffer[BUFFER_SIZE - 1] = '\0';        // Ensure null-termination

  // Iterate through the buffer and process JSON objects
  int length = strlen(buffer);
  int startIndex = 0;
  int endIndex = 0;

  while (startIndex < length)
  {
    // Find the start of the next JSON object
    if (buffer[startIndex] != '{')
    {
      startIndex++;
      continue;
    }

    // Find the end of the current JSON object
    endIndex = startIndex;
    int braceCount = 0;

    while (endIndex < length)
    {
      if (buffer[endIndex] == '{')
      {
        braceCount++;
      }
      else if (buffer[endIndex] == '}')
      {
        braceCount--;

        if (braceCount == 0)
        {
          break; // Found the end of the JSON object
        }
      }

      endIndex++;
    }

    // Extract the JSON object from the buffer
    char jsonObject[BUFFER_SIZE];
    strncpy(jsonObject, buffer + startIndex, endIndex - startIndex + 1);
    jsonObject[endIndex - startIndex + 1] = '\0'; // Ensure null-termination

    sendJsontoWeb(jsonObject, strlen(jsonObject));

    startIndex = endIndex + 1;
  }
}

/**
 * @brief Async receive of a Json package from the slider.
 *
 * @param pBLERemoteCharacteristic
 * @param pData
 * @param length
 * @param isNotify
 */
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
  // Serial.println("------------------- NEW MESSAGE -------------------");

  // End pData buffer at correct length.
  pData[length] = '\0';

  /* Debugging */
  // Serial.print("pData: ");
  // Serial.println((char *)pData);

  // Only copy the right length.
  strcpy(inputBuffer, (char *)pData);

  // Remove unneccesary characters.
  removeWhitespace(inputBuffer);

  // Save new length.
  length = strlen(inputBuffer);

  if (!isJsonComplete(inputBuffer, length))
  { // Input is not complete

    // Concatenate input to buffer
    strcat(bluetoothBuffer, inputBuffer);

    // Check if it is complete again
    if (isJsonComplete(bluetoothBuffer, strlen(bluetoothBuffer)))
      handleJson(bluetoothBuffer);
  }
  else
  { // Input is complete
    handleJson(inputBuffer);

    // Reset bluetoothbuffer
    bluetoothBuffer[0] = '\0';
  }
}

/**
 * @brief Form a connection to the bluetooth server
 *
 * @return true
 * @return false
 */
bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pWChr = pRemoteService->getCharacteristic(writeUUID);
  if (pWChr == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(writeUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our write characteristic");

  //---------- READ UUID ----------//
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRChr = pRemoteService->getCharacteristic(readUUID);
  if (pRChr == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(readUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our read characteristic");

  // Read the value of the characteristic.
  if (pRChr->canRead())
  {
    std::string value = pRChr->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  // If it can notify. Then subscribe.
  if (pRChr->canNotify())
    pRChr->subscribe(true, notifyCallback);

  connected = true;
  return true;
}

void writeToBLE(String packet)
{
  // Wait for small moment
  delayMicroseconds(100000);
  if (!connected)
  {
    Serial.println("Not connected to bluetooth");
    // BLEDevice::getScan()->stop();
    // BLEDevice::getScan()->start(0, false);
    return;
  }
  if (pWChr->canWrite())
  {

    if (pWChr->writeValue(packet))
    {
      Serial.print("Writing: ");
      Serial.print(packet);
      Serial.print(" to: ");
      Serial.println(pWChr->getUUID().toString().c_str());
    }
    else
    {
      Serial.println("Could not write");
    }
  }
  else
  {
    Serial.println("Does not support writing...");
  }
}

void bluetoothsetup()
{
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  BLEDevice::setMTU(512);

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void bluetoothLoop()
{
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true)
  {
    if (connectToServer())
    {
      Serial.println("We are now connected to the BLE Server.");
    }
    else
    {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected)
  {
    // do nothing
  }
  else if (doScan)
  {
    Serial.println("Lost connection to BLE");
    BLEDevice::getScan()->stop();
    BLEDevice::getScan()->clearResults();
    BLEDevice::getScan()->start(0, false); // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
}
