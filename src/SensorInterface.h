#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <ArduinoJson.h>
#include <vector>

struct SensorData {
  int sensorNumber;
  float temperature;
};

class SensorInterface {
public:
  virtual void setup() = 0;
  virtual void loop() = 0;
  virtual void writeTemperatureToFile() = 0;
  virtual void readTemperatureFromFile() = 0;
  virtual void setWebSocketConnected(bool connected) = 0;
  virtual std::vector<SensorData> getStoredTemperatures() = 0;

};

#endif
