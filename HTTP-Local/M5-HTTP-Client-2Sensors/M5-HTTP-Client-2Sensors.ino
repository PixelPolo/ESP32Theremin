#include <M5StickC.h>
#include <Wire.h>
#include <TCA9548.h>
#include <Unit_Sonic.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "linkedQueue.h"

// WiFI Settings
const char* ssid = "...";
const char* password = "...";

// Server Settings
const char* serverAddress = "...";
const int serverPort = 8000;

// Wifi
WiFiClient client;

// HttpClient
HTTPClient http;

// Sensor
SONIC_I2C sensor;

// Multiplexer on I2C address 0x70
TCA9548 multiplexer(0x70);

// Queue pointer
LinkedQueue *distanceQueue;

// Extern variables
float sensor1;
float sensor2;

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
// Send distances to the server
// **********************************************

void sendData() {
  // Configure server and port
  http.begin(client, serverAddress, serverPort, "/");  
  // Content-type header
  http.addHeader("Content-Type", "application/json");  
  // Data construction
  String distanceJsonPayload = "{\"sensor1\": " + String(sensor1) + ", \"sensor2\": " + String(sensor2) + "}";
  // Send data with Post
  int httpResponseCode = http.POST(distanceJsonPayload);  
  // handleResponse(httpResponseCode);
  // Free http client
  http.end();
}

// FOR DEBUGGING
void handleResponse(int httpResponseCode) {
    // Response OK
    if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
  // Response Not OK
  } else {
    // Print Response
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
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
    // Configure server and port
    http.begin(client, serverAddress, serverPort, "/save");  
    // Content-type header
    http.addHeader("Content-Type", "application/json");
    // Queue iteration
    struct Node *iterator = distanceQueue->first;
    while (iterator != nullptr)
    {
        // Send distances
        float d1 = iterator->sensor1;
        float d2 = iterator->sensor2;
        String distanceJsonPayload = "{\"sensor1\": " + String(d1) + ", \"sensor2\": " + String(d2) + "}";
        int httpResponseCode = http.POST(distanceJsonPayload);
        // handleResponse(httpResponseCode);
        iterator = iterator->next;
    }
    // Reset the Queue
    freeQueue(distanceQueue);
    distanceQueue = createQueue();
    // Free http client
    http.end();
  }
}

// **********************************************
// Loop of the Atom Lite
// **********************************************

void loop() {
  // Read Sensors
  readSensors();
  // Send Data to Server
  sendData();
  // Save Data inside a LinkedQueue
  saveDistances();
  // Send 10 saved distances
  sendSavedDistances(10); 
  // Manage Heap Memory
  manageHeapMemory();
}
