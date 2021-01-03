/*
Autor: Julian Schweizerhof
Voraussetzungen: Webseite im data Ordner auf dem ESP32 hochladen via Platformio "Upload Filesystem Image"
*/

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "SData.h"

//static DataReceive globalReceivedData;
// geht leider nicht anders :(
//static AsyncWebSocketClient * globalClient = NULL;

class CServer
{
    public:
        CServer(char* pSSID, char* pPassword, char* pDomain);
        void transmitData(DataSend mySendData);
        void receiveData(DataReceive & myReceivedData);
        //void run();
        void setup();
    private:
        IPAddress IP;        // Variable zum Speichern der IP-Adresse
        char* ssid;
        char* password; 
        char* domain;
        AsyncWebServer server;
        AsyncWebSocket ws;
        //AsyncWebSocketClient * globalClient = NULL;
        // ohne static geht es nicht.
        static AsyncWebSocketClient * globalClient;
        static void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
        DataReceive dataReceived;
        DataSend dataSend;
};