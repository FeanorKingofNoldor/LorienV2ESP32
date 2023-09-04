#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <WebSocketsClient.h>
#include "SensorInterface.h"

extern const int tempSensorPin1;
extern const int tempSensorPin2;

class TempSensorHandler; // Forward declaration

class WebSocketHandler {
public:
    WebSocketHandler();
    WebSocketHandler(TempSensorHandler& sensorHandler);
    WebSocketsClient& getWebSocket();
    void setup(TempSensorHandler& sensorHandler);
    void loop();
    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);

private:
    WebSocketsClient webSocket;
    TempSensorHandler& tempSensorHandler; // Add reference to TempSensorHandler
};


#endif

