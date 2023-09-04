#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>

enum Mode {
  AP_MODE,
  STATION_MODE,
  UNDEFINED
};

class WiFiHandler {
public:
  void setup();
  void loop();
private:
  void setupAPMode();
  void setupStationMode();
  void readWiFiCredentials();
  void handleRoot();
  void handleSet();
  void writeFlagToFile(int flag);
  void readFlagFileAndDecideMode();
  void checkFilesExist();
  void initializeSPIFFS();
  void checkWiFiStatus();
};

#endif // WIFIHANDLER_H
