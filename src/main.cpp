#include <Arduino.h>
#include <SPIFFS.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "SensorInterface.h"
#include "TempSensorHandler.h"


WiFiHandler wifiHandler;
WebSocketHandler webSocketHandler;

// Define the pins for the two temperature sensors
const int tempSensorPin1 = 15;  // Replace with the actual pin number
const int tempSensorPin2 = 35;  // Replace with the actual pin number

extern const int tempSensorPin1;
extern const int tempSensorPin2;


TempSensorHandler tempSensorHandler1(tempSensorPin1, 1);  // Pass sensor pins
TempSensorHandler tempSensorHandler2(tempSensorPin2, 2);  // Pass sensor pins


void setup() {
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS mounted successfully.");

  webSocketHandler.setup(tempSensorHandler1);
  tempSensorHandler1.setup(webSocketHandler);

  webSocketHandler.setup(tempSensorHandler2);
  tempSensorHandler2.setup(webSocketHandler);

  wifiHandler.setup();
}

void loop() {
  wifiHandler.loop();
  webSocketHandler.loop();
  tempSensorHandler1.loop();
  tempSensorHandler2.loop();
}
