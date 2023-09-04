#include "WebSocketHandler.h"
#include "TempSensorHandler.h"
#include <WebSocketsClient.h>

WebSocketHandler::WebSocketHandler() : tempSensorHandler(TempSensorHandler::defaultInstance(tempSensorPin1, tempSensorPin2)) {
    // Constructor implementation
}


WebSocketHandler::WebSocketHandler(TempSensorHandler& sensorHandler)
    : tempSensorHandler(sensorHandler) {
    // Parameterized constructor implementation
}

void WebSocketHandler::setup(TempSensorHandler& sensorHandler) {
    // WebSocket setup code here
    webSocket.begin("172.19.96.1", 6379, "/temperature");
    webSocket.onEvent([this](WStype_t type, uint8_t * payload, size_t length) {
        this->webSocketEvent(type, payload, length);
    });
}

void WebSocketHandler::loop() {
    webSocket.loop();
}

void WebSocketHandler::webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    if (type == WStype_CONNECTED) {
        // When connected, you can start sending data
        std::pair<float, float> temperatures = tempSensorHandler.getTemperatures();
        String temperatureJson = "{\"temperature1\": " + String(temperatures.first) + 
                                 ", \"temperature2\": " + String(temperatures.second) + "}";
        webSocket.sendTXT(temperatureJson);
    } else if (type == WStype_DISCONNECTED) {
        // Handle disconnection event
        // You might want to set the WebSocket connection flag to false here
        tempSensorHandler.setWebSocketConnected(false);
    } else if (type == WStype_TEXT) {
        // Handle text data reception if needed
        // ...
    }
    // Handle other WebSocket event types if necessary
}

WebSocketsClient& WebSocketHandler::getWebSocket() {
    return webSocket;
}
