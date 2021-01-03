#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// WiFi
/*
void sendToClient();              // Daten an die Webseite schicken
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
  // wird aufgerufen wenn ein WebSocket Event auftritt:
*/
void onWsEvent();
class CServer
{
    public:
        CServer(char* pSSID, char* pPassword);
        void transmitData();
        void receiveData();
        void run();
        void setup();
    private:
        IPAddress IP;        // Variable zum Speichern der IP-Adresse
        char* ssid;
        char* password; 
        // ohne static geht es nicht.
        static void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
        
};
