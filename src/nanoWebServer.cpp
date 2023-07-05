
#include "nanoWebserver.h"

// Replace with your network credentials
const char *ssid = WIFINAME;
const char *password = WIFIPASS;

// Set web server port number to 80
AsyncWebServer server(80);
AsyncWebSocket websock("/ws");

// Inputbuffer for when the websocket message arrives in two packages.
char *buffer = new char[400];

// What will happen when a websocket event happens.
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

  if (type == WS_EVT_DISCONNECT)
  {
    Serial.println("WebSocket Disconnected");
  }
  if (type == WS_EVT_CONNECT)
  {
    Serial.println("WebSocket Connected!");
  }
  if (type == WS_EVT_DATA)
  {
    // data packet
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    if (info->final && info->index == 0 && info->len == len)
    {
      // the whole message is in a single frame and we got all of it's data
      if (info->opcode == WS_TEXT)
      {
        data[len] = 0;
        writeToBLE((char *)data);
      }
    }
    else
    {
      // message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
        buffer = new char[info->len];
      memcpy(buffer + info->index, data, len);

      if ((info->index + len) == info->len && info->final && info->message_opcode == WS_TEXT)
        writeToBLE((char *)data);
    }
  }
}

/**
 * @brief Handle incoming HTTP requests.
 *
 * @param request
 * @param data
 * @param len
 * @param index
 * @param total
 */
void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  if (request->contentType() == "application/json")
  {
    // Handle the incoming data here
    // You can accumulate the data and process it later

    // To send a response immediately, you can use the following code:
    // request->send(200, "text/plain", "Data received");

    // To send a response after processing the complete request body, you can check if index + len equals total
    if (index + len == total)
    {
      // Complete request body received, process the data here
      // String requestBody = String(reinterpret_cast<char*>(data), len);
      // Process the request body
      // ...
      sendJsontoWeb((char *)data, len);

      data[0] = '\0';
      //  Send a response
      request->send(200, "text/plain", "Data received and processed");
    }
  }
  else
  {
    Serial.println("Did not receive Companion.");
    request->send(400, "text/plain", "Data received but not processed");
  }
}

void websetup()
{
  // Start serial if not available
  if (!Serial.available())
  {
    Serial.begin(115200);
  }

  // Check if Ethernet is available
  if (ETH.begin())
  {
    if (ETH.linkUp())
    {
      Serial.println("Connecting to Ethernet");
      // Enable DHCP for Ethernet
      if (ETH.config(IPAddress(), IPAddress(), IPAddress(), IPAddress()))
      {
        Serial.println("DHCP enabled");
        delay(5000); // Add a delay to allow DHCP negotiation

        // Check if an IP address is obtained
        if (ETH.localIP() == IPAddress(0, 0, 0, 0))
        {
          Serial.println("Failed to obtain IP address");
        }
        else
        {
          Serial.print("IP address: ");
          Serial.println(ETH.localIP());
        }
      }
      else
      {
        Serial.println("Failed to enable DHCP");
      }
    }
    else
    {
      Serial.println("Ethernet cable is not connected, trying wifi.");

      // Start the Wi-Fi connection
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);

      // Wait for Wi-Fi connection to be established
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }

      Serial.println("Wi-Fi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }

    // Start SPIFFS
    if (!SPIFFS.begin())
    {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    else
    {
      Serial.println("SPIFFS started successfully.");
    }

    if (MDNS.begin("sharkNano-server"))
    {
      Serial.println("Initializing MDNS 'sharkNano-Server'");
    }
    else
    {
      Serial.println("MDNS failed to start.");
      return;
    }

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.onRequestBody(handleRequest);

    websock.onEvent(onWsEvent);

    server.addHandler(&websock);

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.begin();
  }
}

/**
 * @brief Send a json string to the websocket if the websocket is available.
 *
 * @param jsonInput pointer to a char array containing the json string
 * @param length length of the array to send.
 */
void sendJsontoWeb(char *jsonInput, int length)
{
  // Send jsonInput to websock
  while (websock.availableForWriteAll() == false)
  {
    delay(10); // Wait for a short duration (e.g., 10 milliseconds) before checking again
  }
  // Serial.print("Sending to websocket: ");
  // Serial.println(jsonInput);
  websock.textAll(jsonInput, length);
}