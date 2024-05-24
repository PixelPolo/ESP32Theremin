#include <M5StickC.h>
#include <Unit_Sonic.h>
#include <TCA9548.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

// Wifi setup
const char *ssid = "...";
const char *password = "...";

// Websocket
WebSocketsClient webSocket;
const char *authToken = "...";
const char *websocketUrl = "...";
const int websocketPort = 443;

// Ultrasonic Distance
// Unit I2C RCWL-9620
SONIC_I2C sensor;

// Multiplexer on I2C address 0x70
TCA9548 multiplexer(0x70);

// Distances for sensors
float sensor1 = 0;
float sensor2 = 1;

// **********************************************
// Setup the Wifi and the Websocket Server
// **********************************************

void setupWifi() {

  // Connect to wifi with animation
  WiFi.begin(ssid, password);
  for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  // Print wifi setup
  // to get IP address assigned to ESP
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
}

// **********************************************
// Reading from 2 sensors
// **********************************************

void readSensors() {
  // Sonic sensor on Channel 0
  multiplexer.selectChannel(0);
  delay(20);
  sensor1 = sensor.getDistance();
  multiplexer.selectChannel(1);
  delay(20);
  sensor2 = sensor.getDistance();
  // Serial.printf("Distance0 : %.2fcm \t Distance1 : %.2fcm\r\n", distance0, distance1);
}

// **********************************************
// WebSocket Event handler
// **********************************************

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {

  switch (type) {

    // ***** Disconnected *****
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;

    // ***** Connected *****
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        // send message to server when Connected
        webSocket.sendTXT("{\"message\":\"Hello world\"}");
      }
      break;

    // ***** Message Text received *****
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      // send message to server
      // webSocket.sendTXT("message here");
      break;

    // ***** Binary data received *****
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      // send data to server
      // webSocket.sendBIN(payload, length);
      break;

    // ***** Others event ******
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

// **********************************************
// Send distances to the client
// **********************************************

// Send distance to the client in JSON string format
String convertDistanceToJSON() {
    return "{\"sensor1\": " + String(sensor1) + ", \"sensor2\": " + String(sensor2) + "}";
}
void sendDistances() {
  String jsonData = convertDistanceToJSON();
  webSocket.sendTXT(jsonData);
}

// **********************************************
// Setup of the Atom Lite
// **********************************************

void setup() {
  // Board setup
  M5.begin();
  // Multiplexer setup
  multiplexer.begin();
  // Sensor stup
  sensor.begin();
  // Serial setup
  Serial.begin(115200);
  // Wifi setup
  setupWifi();
  // Websocket begin

  String websocketPath = "/?token=" + String(authToken);
  webSocket.beginSSL(websocketUrl, websocketPort, websocketPath);
  webSocket.onEvent(webSocketEvent);
}

// **********************************************
// Loop of the Atom Lite
// **********************************************

void loop() {
  webSocket.loop();
  readSensors();
  sendDistances();
  // delay(1000);
}
