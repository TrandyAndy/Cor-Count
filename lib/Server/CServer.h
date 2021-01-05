/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-01-04 16:23:52
 * @Description: Voraussetzungen: Webseite im data Ordner auf dem ESP32 hochladen via Platformio "Upload Filesystem Image"
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
        byte receiveData(DataReceive & myReceivedData); // Rückgabe: 0 = keine Nachricht, 1 = Änderungen an der Webseite, 2 = Datum und Zeit wird geschickt nachdem eine Person durchgelaufen ist, 3 = Fehler
        //void run();
        void init();
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