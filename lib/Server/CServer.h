/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-06-04 13:23:16
 * @Description: Voraussetzungen: Webseite im data Ordner auf dem ESP32 hochladen via Platformio "Upload Filesystem Image"
 */

#pragma once

#include <Arduino.h>
//#include "Global.h"     // führt zu Fehler
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "ArduinoJson.h"
#include "SData.h"
#include "CSaveLoad.h"

extern CSaveLoad myFlash;   // Umweg weil Global nicht geht


//static DataReceive globalReceivedData;
// geht leider nicht anders :(
//static AsyncWebSocketClient * globalClient = NULL;

class CServer
{
    public:
        CServer(String pSSID, String pPassword, String pDomain);
        void setNewConnectionSTA(String pSSID, String pPassword);
        void setNewConnectionAP(String pSSID, String pPassword);
        void transmitData(DataSend mySendData);
        void transmitDataWIFI();
        byte receiveData(DataReceive & myReceivedData); // Rückgabe: 0 = keine Nachricht, 1 = Änderungen an der Webseite, 2 = Datum und Zeit wird geschickt nachdem eine Person durchgelaufen ist, 3 = initiale Nachricht, 4 = Fehler
        void sendDebugMessage(String msg);  // Debug Message schicken an Webclient
        void sendTOFData(float dataTOF1, float dataTOF2);
        bool checkIsWorkingSTA(bool send = false);
        //void run();
        void init();
        void close();
    private:
        void scanNetworks();
        IPAddress IP;        // Variable zum Speichern der IP-Adresse
        String ssidSTA;
        String passwordSTA; 
        String ssidAP;
        String passwordAP;
        String domain;
        AsyncWebServer server;
        AsyncWebSocket ws;
        //AsyncWebSocketClient * globalClient = NULL;
        // ohne static geht es nicht.
        static AsyncWebSocketClient * globalClient;
        static void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
        DataReceive dataReceived;
        DataSend dataSend;
        bool flagAP;
        bool isThereAnyClient();
        String scannedNetworks[20];
        byte numberOfScannedNetworks;
        bool isActiveSTA = true;
};