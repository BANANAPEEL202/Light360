#include <WiFi.h>             // or <ESP8266WiFi.h> for ESP8266
#include <ESPAsyncWebServer.h> // Library to manage the asynchronous server

// Replace these with your Wi-Fi credentials
const char* ssid = "GTother";
const char* password = "GeorgeP@1927";

AsyncWebServer server(80);    // Set up a server on port 80

// Global variables to hold the status
String deviceStatus = "Idle";
bool controlFlag = false;

void setup() {
  // Start Serial
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Route to return status
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", "{\"status\":\"" + deviceStatus + "\"}");
  });

  // Route to control device
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("action")) {
      String action = request->getParam("action")->value();
      
      if (action == "start") {
        controlFlag = true;
        deviceStatus = "Running";
        request->send(200, "application/json", "{\"message\":\"Device started\"}");
      } else if (action == "stop") {
        controlFlag = false;
        deviceStatus = "Stopped";
        request->send(200, "application/json", "{\"message\":\"Device stopped\"}");
      } else {
        request->send(400, "application/json", "{\"error\":\"Invalid action\"}");
      }
    } else {
      request->send(400, "application/json", "{\"error\":\"No action specified\"}");
    }
  });

  // Start the server
  server.begin();
}

void loop() {
  // Here, you could add code to run the device based on controlFlag
  if (controlFlag) {
    // Device is running
    Serial.println("Device is active...");
  } else {
    // Device is idle
    Serial.println("Device is idle...");
  }
  delay(2000);
}
