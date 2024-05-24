#include <M5StickC.h>
#include <Unit_Sonic.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <TCA9548.h>
#include "linkedQueue.h"

// Wifi setup
const char *ssid = "...";
const char *password = "...";

// Ultrasonic Distance
// Unit I2C RCWL-9620
SONIC_I2C sensor;

// Multiplexer on I2C address 0x70
TCA9548 multiplexer(0x70);

// Distances for sensors
float sensor1 = 0;
float sensor2 = 1;

// Websocket Server PORT = 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Queue pointer
LinkedQueue *distanceQueue;

// **********************************************
// Setup the Wifi
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
// WebSocket Event handler
// **********************************************

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

  switch (type) {

    // ***** Disconnected *****
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // ***** Connected *****
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        // send message to client
        // webSocket.sendTXT(num, "Connected");
      }
      break;

    // ***** Message Text received *****
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      // Send message to client
      // webSocket.sendTXT(num, "message here");
      // Send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;

    // ***** Binary data received *****
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      // send message to client
      // webSocket.sendBIN(num, payload, length);
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
  // LinkedQueue setup
  distanceQueue = createQueue();
  // Wifi setup
  setupWifi();
  // Websocket setup
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  // Delay
  delay(3000);
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
// Send distances to the client
// **********************************************

// Prepare a JSON string format to boradcast
String convertDistanceToJSON() {
    return "{\"sensor1\": " + String(sensor1) + ", \"sensor2\": " + String(sensor2) + "}";
}

// **********************************************
// Save distances in flash memory
// **********************************************

void saveDistances() {
  // Enqueue Distances inside the LinkedQueue
  enqueueDistances(distanceQueue, sensor1, sensor2);
}

void manageHeapMemory() {
  // If the memory is full, reset the queue
  uint32_t freeHeap = ESP.getFreeHeap();
  // Print for tests and debug
  // Serial.println("Queue size : " + String(distanceQueue->size));
  // Serial.println("Free Heap : " + String(freeHeap) + " bytes");
  // Reset the Queue and send distances
  if (freeHeap < 100) {
    freeQueue(distanceQueue);
    distanceQueue = createQueue();
  }
}

void sendSavedDistances(int quantity) {
  // Check if we have enough distances
  if (distanceQueue->size > quantity) {
    // Send a special message to start saving
    String specialMsg = "{\"specialMsg\": \"SAVE_START\"}";
    webSocket.broadcastTXT(specialMsg);
    // Queue iteration
    struct Node *iterator = distanceQueue->first;
    while (iterator != nullptr)
    {
        // Send distances
        float d1 = iterator->sensor1;
        float d2 = iterator->sensor2;
        String distanceJsonPayload = "{\"sensor1\": " + String(d1) + ", \"sensor2\": " + String(d2) + "}";
        webSocket.broadcastTXT(distanceJsonPayload);
        iterator = iterator->next;
    }
    // Send a special message to stop saving
    specialMsg = "{\"specialMsg\": \"SAVE_END\"}";
    webSocket.broadcastTXT(specialMsg);
    // Reset the Queue
    freeQueue(distanceQueue);
    distanceQueue = createQueue();
  }
}

// **********************************************
// Loop of the Atom Lite
// **********************************************

void loop() {
  // Read Sensors
  readSensors();
  // Convert distance and send to all clients !
  String jsonDistance = convertDistanceToJSON();
  webSocket.broadcastTXT(jsonDistance);
  // Save Data inside a LinkedQueue
  saveDistances();
  // Send 10 saved distances
  sendSavedDistances(10); 
  // Manage Heap Memory
  manageHeapMemory();
  // Web Socket loop
  webSocket.loop();
}
