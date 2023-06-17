
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

void websetup()
{
  // Start serial if not available
  if (!Serial.available())
  {
    Serial.begin(115200);
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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

  Serial.println("Initializing MDNS 'sharkNano-Server'");
  MDNS.begin("sharkNano-server");

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  websock.onEvent(onWsEvent);

  server.addHandler(&websock);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.begin();
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
  if (websock.availableForWriteAll())
  {
    Serial.print("Sending to websocket: ");
    Serial.println(jsonInput);
    websock.textAll(jsonInput, length);
  }
}