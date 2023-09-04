#include "TempSensorHandler.h"
#include "WebSocketHandler.h"
#include "SensorInterface.h"

TempSensorHandler::TempSensorHandler(int pin1, int pin2)
    : webSocketConnected(false), sendInterval(5000), lastSendTime(0) {
    // Initialize your vectors and sensors here
    oneWires.push_back(OneWire(pin1));
    oneWires.push_back(OneWire(pin2));
    sensors.resize(oneWires.size(), DallasTemperature());
}

void TempSensorHandler::setup(WebSocketHandler& webSocketHandler) {
    for (auto& sensor : sensors) {
        sensor.begin();
    }
    // Other setup logic
}

void TempSensorHandler::loop() {
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= 60000) {
        lastSendTime = currentTime;

        if (webSocketConnected) {
            // Send stored data via WebSocket
            for (const auto& sensorData : storedTemperatures) {
                String temperatureJson = "{\"sensor\": " + String(sensorData.sensorNumber) +
                                        ", \"temperature\": " + String(sensorData.temperature) + "}";
                // Send data via WebSocket
            }

            storedTemperatures.clear(); // Clear the stored data
        }
    }

    // Loop through the sensors, read temperature, and handle data
    for (size_t i = 0; i < sensors.size(); ++i) {
        sensors[i].setOneWire(&oneWires[i]);
        sensors[i].requestTemperatures();
        
        float tempC = sensors[i].getTempCByIndex(0);
        
        if (webSocketConnected) {
            String temperatureJson = "{\"sensor\": " + String(i + 1) +
                                     ", \"temperature\": " + String(tempC) + "}";
            // Send data via WebSocket
        } else {
            storedTemperatures.push_back({static_cast<int>(i + 1), tempC}); // Store data if WebSocket is not connected
        }
    }

    // Other loop logic
}

TempSensorHandler& TempSensorHandler::defaultInstance(int pin1, int pin2) {
    static TempSensorHandler instance(pin1, pin2);
    return instance;
}

void TempSensorHandler::writeTemperatureToFile() {
    // Implementation of writing temperature data to a file
    // You can use SPIFFS or any other method to write the data
}

void TempSensorHandler::readTemperatureFromFile() {
    // Implementation of reading temperature data from a file
    // You can use SPIFFS or any other method to read the data
}

std::pair<float, float> TempSensorHandler::getTemperatures() {
    float temp1 = sensors[0].getTempCByIndex(0);
    float temp2 = sensors[1].getTempCByIndex(0);

    if (temp1 == -127.00f || temp2 == -127.00f) {
        // Handle the case where temperature reading is not available
        // For example, you can set them to a specific value indicating an error
        temp1 = -999.0f;
        temp2 = -999.0f;
    }

    return std::make_pair(temp1, temp2);
}

std::vector<SensorData> TempSensorHandler::getStoredTemperatures() {
    std::vector<SensorData> storedData;
    for (const SensorData& data : storedTemperatures) {
        storedData.push_back(data);
    }
    return storedData;
}


void TempSensorHandler::setWebSocketConnected(bool connected) {
    webSocketConnected = connected;
}
