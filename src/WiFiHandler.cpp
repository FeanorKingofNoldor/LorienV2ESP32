#include "WiFiHandler.h"
#include <SPIFFS.h>
#include <functional>

#define CONNECTION_TIMEOUT 60000 // 60 seconds
#define DEBUG  // Uncomment to enable debugging

WebServer server(80);
String ssid = "";
String password = "";
Mode currentMode = UNDEFINED;
unsigned long connectionStartTime = 0;

void setupAPMode();
void setupStationMode();
void readWiFiCredentials();
void checkFilesExist();
void initializeSPIFFS();
void checkWiFiStatus();
void handleRoot();
void handleSet();
void writeFlagToFile(int flag);
void readFlagFileAndDecideMode();

void WiFiHandler::setup() {
  delay(100);
  checkFilesExist();
  
  readWiFiCredentials();
  readFlagFileAndDecideMode();

  server.on("/", HTTP_GET, std::bind(&WiFiHandler::handleRoot, this));
  server.on("/set", HTTP_POST, std::bind(&WiFiHandler::handleSet, this));
  
  server.begin();
}

void WiFiHandler::loop() {
  server.handleClient();
  checkWiFiStatus();
}

void WiFiHandler::checkWiFiStatus() {
  if (currentMode == STATION_MODE && WiFi.status() != WL_CONNECTED) {
    if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
#ifdef DEBUG
      Serial.println("Failed to connect. Switching to AP mode.");
#endif
      setupAPMode(); // Switch to AP mode
    }
  }
}

void WiFiHandler::initializeSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  } else {
    Serial.println("SPIFFS mounted successfully.");
  }
  delay(500); // Add a delay to ensure initialization
}

void WiFiHandler::checkFilesExist() {
  // Check if files exist
  if(SPIFFS.exists("/wifi.json")) {
    Serial.println("/wifi.json exists.");
  } else {
    Serial.println("/wifi.json does not exist.");
  }
  if(SPIFFS.exists("/flag.txt")) {
    Serial.println("/flag.txt exists.");
  } else {
    Serial.println("/flag.txt does not exist.");
  }
}

void WiFiHandler::readFlagFileAndDecideMode() {
  File flagFile = SPIFFS.open("/flag.txt", "r");
  if (flagFile) {
    String flagStr = flagFile.readString();
    int flagValue = flagStr.toInt();
    Serial.print("Read flag value as string: ");
    Serial.println(flagStr);
    Serial.print("Converted flag value to int: ");
    Serial.println(flagValue);

    flagFile.close();
    
    // Updated logic
    if (flagValue == 1 && ssid.length() > 0 && password.length() > 0) {
      setupStationMode();
    } else if (flagValue == 1) {
      setupAPMode();
    } else {
      setupStationMode();
    }
  } else {
    Serial.println("Failed to open flag file. Going to default AP mode.");
    setupAPMode();
  }
}

void WiFiHandler::setupAPMode() {
  #ifdef DEBUG
    Serial.println("Switching to AP mode.");
  #endif
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP("MyESP32AP", "password");
  writeFlagToFile(1);
  Serial.println("Flag value set to 1");
  currentMode = AP_MODE;
}

void WiFiHandler::setupStationMode() {
  #ifdef DEBUG
    Serial.println("Switching to Station mode.");

    // Validate credentials before trying to connect
    if (ssid.length() > 0 && password.length() > 0) {
      Serial.print("Valid credentials. Trying to connect to: ");
      Serial.print(ssid);
      Serial.print(" with password: ");
      Serial.println(password);
    } else {
      Serial.println("Invalid credentials. Switching to AP mode.");
      setupAPMode();
      return;
    }
  #endif

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  writeFlagToFile(0);
  currentMode = STATION_MODE;
  connectionStartTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    // Print the current WiFi status code
    Serial.print("Status: ");
    Serial.println(WiFi.status());
    delay(1000);
    if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
  #ifdef DEBUG
      Serial.println("Failed to connect. Switching to AP mode.");
  #endif
      setupAPMode();
      return;
    }
  }
  #ifdef DEBUG
    Serial.println("");
    Serial.println("Connected to WiFi.");
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
    Serial.print("Local IP Address: ");
    Serial.println(WiFi.localIP());
  #endif
}

void WiFiHandler::readWiFiCredentials() {
  File file = SPIFFS.open("/wifi.json", "r");
  if (file) {
  #ifdef DEBUG
    Serial.println("Opened wifi.json for reading");
  #endif
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, file);
    if (err) {
  #ifdef DEBUG
      Serial.print("deserializeJson() failed: ");
      Serial.println(err.c_str());
  #endif
    } else {
      ssid = doc["ssid"].as<String>();
      ssid.trim();  // Added trim here

      password = doc["password"].as<String>();
      password.trim();  // Added trim here

  #ifdef DEBUG
      Serial.print("Read SSID: ");
      Serial.println(ssid);
      Serial.print("Read password: ");
      Serial.println(password);
  #endif

      if (ssid.length() > 0 && password.length() > 0) {
        Serial.println("Successfully read valid WiFi credentials from file.");
      } else {
        Serial.println("Read invalid WiFi credentials from file.");
      }
    }
    file.close();
  } else {
  #ifdef DEBUG
    Serial.println("Failed to open wifi.json");
  #endif
  }
}

void WiFiHandler::handleRoot() {
  String html = "<html><body>";
  html += "<h1>Enter WiFi Credentials</h1>";
  html += "<form method='post' action='/set'>";
  html += "SSID:<br>";
  html += "<input type='text' name='ssid'><br>";
  html += "Password:<br>";
  html += "<input type='password' name='password'><br><br>";
  html += "<input type='submit' value='Submit'>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void WiFiHandler::handleSet() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    ssid = server.arg("ssid");
    password = server.arg("password");
    
    StaticJsonDocument<256> doc;
doc["ssid"] = ssid;
doc["password"] = password;

File file = SPIFFS.open("/wifi.json", "w");
if (file) {
  serializeJson(doc, file);
  file.close();
  
#ifdef DEBUG
  Serial.println("Settings saved to file");
  Serial.println("Written to file: " + String(ssid) + ", " + String(password)); // Added debug line
#endif
} else {
#ifdef DEBUG
  Serial.println("Failed to open file for writing");
#endif
  return;
}


    writeFlagToFile(0); // Set flag to zero for station mode
    server.send(200, "text/plain", "Settings saved. Rebooting.");
    
#ifdef DEBUG
    Serial.println("Sent response to client");
#endif

    delay(2000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void WiFiHandler::writeFlagToFile(int flag) {
  File flagFile = SPIFFS.open("/flag.txt", "w");
  if (flagFile) {
    flagFile.print(String(flag));
    flagFile.close();
  } else {
    Serial.println("Failed to write to flag file!");
  }

}