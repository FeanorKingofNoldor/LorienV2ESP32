#ifndef TEMP_SENSOR_HANDLER_H
#define TEMP_SENSOR_HANDLER_H

#include "SensorInterface.h"
#include <vector>  // For handling multiple sensors
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FS.h> // For SPIFFS

extern const int tempSensorPin1;
extern const int tempSensorPin2;

class WebSocketHandler;

class TempSensorHandler : public SensorInterface {
public:
    TempSensorHandler(int pin1, int pin2);
    void setup(WebSocketHandler& webSocketHandler);
    void setup() override;
    void loop() override;
    void writeTemperatureToFile();
    void readTemperatureFromFile();
    void setWebSocketConnected(bool connected);
    std::pair<float, float> getTemperatures(); 
    std::vector<SensorData> getStoredTemperatures();
    virtual ~TempSensorHandler() {};
    static TempSensorHandler& defaultInstance(int pin1, int pin2);

private:
    std::vector<OneWire> oneWires;
    std::vector<DallasTemperature> sensors;
    std::vector<SensorData> storedTemperatures;
    bool webSocketConnected;
    unsigned long lastSendTime;
    unsigned long sendInterval;
    
    
    // Other members and methods as needed
};


#endif
