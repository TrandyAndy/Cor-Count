/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-01-09 08:23:18
 * @Description: Voraussetzungen: Webseite im data Ordner auf dem ESP32 hochladen via Platformio "Upload Filesystem Image"
 */

#include "CServer.h"

DataReceive globalReceivedData;
AsyncWebSocketClient * CServer::globalClient = NULL;
byte messageFlag = 0;

CServer::CServer(char* pSSID, char* pPassword, char* pDomain): ssid(pSSID), password(pPassword), domain(pDomain), server(80), ws("/ws")
{
    
}

void CServer::init()
{
    if(!SPIFFS.begin())                       // Dateien vom Flashspeicher des ESP32 lesen, inkl. der Webseite
    {
        Serial.println("An Error has occurred while mounting SPIFFS");  // Zum Debuggen
        Serial.println("Sie muessen eine Webseite hochladen!"); // Zum Debuggen
        //return;                               // Wenn keine Webseite hochgeladen wurde, wird kein WiFi eröffnet
    }
    // Ende Dateien vom Speicher laden

    // WiFi
    Serial.print("Starten AP (Access Point)…");
    WiFi.softAP(ssid, password,1,0,4);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
    IP = WiFi.softAPIP();                     // IP-Adresse auslesen
    if(MDNS.begin(domain))
    {
      Serial.println("DNS gestartet, erreichbar unter: ");
      Serial.println("http://" + String(domain)+ ".local/");
    }
    Serial.print("AP IP-Adresse: ");
    Serial.println(IP);                       // IP-Adresse ausgeben
    ws.onEvent(onWsEvent);                    // Funktion bekannt machen, die aufgerufen werden soll bei einem WS Events
    server.addHandler(&ws);                   // Handler hinzufügen
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/" zugreift
    request->send(SPIFFS, "/index.htm", "text/html"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/script.js" zugreift
    request->send(SPIFFS, "/script.js", "text/script"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/style.css" zugreift
    request->send(SPIFFS, "/style.css", "text/css"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    /*
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){   // wenn der Client auf die Adresse: "192.168.4.1/favicon.ico" zugreift
    request->send(SPIFFS, "/favicon.ico", "image/ico"); // wird das Favicon vom Flash Speicher geladen an an den Client geschickt.
    });
    */
    server.begin();                           // Server wird gestarten
    // Ende WiFi
}
void CServer::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
    // wird aufgerufen wenn ein WebSocket Event auftritt:
    if(type == WS_EVT_CONNECT)                // was soll bei einer Verbindung passieren?
    {
        
        Serial.println("Websocket Verbindung hergestellt.");  // Zum Debuggen
        globalClient = client;                  // client speichern
        Serial.printf("Die ID des Client ist: %d \n",globalClient->id()); // Zum Debuggen
        //ausgabe();                              // Webseite aktualisieren
        
    } 
    else if(type == WS_EVT_DISCONNECT)        // was soll bei einem Verbindungsabbruch passieren?
    {
        Serial.println("Client disconnected");
        Serial.println("-----------------------");
        globalClient = NULL;                    // client Pointer leer machen
    } 
    else if(type == WS_EVT_DATA)              // was soll bei empfangenen Daten passieren passieren?
    {
        char nachricht[len];                    // Ein Chararray für das Empfangen des Datenstrings im JSON-Format erstelle, len ist die Größe der empfangen Daten
        Serial.print("Data received: ");        // Zum Debuggen
        for(int i=0; i < len; i++)              // Komplette Nachricht auslesen und in das Char-Array "nachricht" schreiben
        {
            nachricht[i] = ((char) data[i]);  // Datenstring füllen, int-Array data in ein char-Array konvertieren
            Serial.print(((char) data[i]));   // Zum Debuggen
        }
        DynamicJsonDocument empfangenJSON(1024); // JSON Dokument erstellen, um empfangen Daten zu verstehen, erforderliche Größe kann auf "https://arduinojson.org/v6/assistant/" berechnet werden
        
        DeserializationError fehlerJSON = deserializeJson(empfangenJSON, nachricht); // Nachricht aus dem Chararray "nachricht" ist das JSON Dokument "empfangenJSON" übertragen
        Serial.println(fehlerJSON.c_str());     // Fehler bei Lesen der JSON Nachricht 
        
        if (empfangenJSON["energiesparmodus"].as<String>() == " ")  // wenn initiale Message von der Webseite kommt
        {
            messageFlag = 3;
        }
        else if(empfangenJSON["dateTime"].as<String>() == " ")       // wenn, etwas in den Einstellungen auf der Webseite geändert wurde
        {
            globalReceivedData.personenzahlMax = empfangenJSON["personenzahlMax"]; // Daten in den Strukt schreiben
            globalReceivedData.personenzahlAktuell = empfangenJSON["personenzahlAktuell"];
            globalReceivedData.energiesparmodus = empfangenJSON["energiesparmodus"];
            messageFlag = 1; // Flag setzten, dass eine Änderung von der Webseite kommt.  
        }
        else    // wenn nur die Uhrzeit geschickt wurde, nachdem eine Person durchgelaufen ist
        {
            globalReceivedData.dateTime = empfangenJSON["dateTime"].as<String>();   // .as<String>(), ansonsten Fehler "This line is ambiguous because the compiler cannot tell which constructor of String to call. Is it the one taking a const char*, an int, or a float?"
            messageFlag = 2; // Flag setzten, dass ein Datum geschickt wird
        }
        
    }
    // Ende Websocket Funktion

}
void CServer::transmitData(DataSend mySendData)
{
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED)    // wenn die Verbindung zur Webseite besteht
    {
        DynamicJsonDocument sendenJSON(1024);
        sendenJSON["personenzahlMax"] = mySendData.personenzahlMax;
        sendenJSON["personenzahlAktuell"] = mySendData.personenzahlAktuell;
        sendenJSON["akkustand"] = mySendData.akkustand;
        sendenJSON["energiesparmodus"] = mySendData.energiesparmodus;
        sendenJSON["flagGetTime"] = mySendData.flagGetTime;
        String JSONmessage;
        serializeJson(sendenJSON, JSONmessage);
        globalClient->text(JSONmessage);    // nur ein Client wird behandelt
        //ws.textAll(JSONmessage);  // alternative mehrere Clients bekommen selbe anzeige
        // Serial.print("Gesendete Daten sind: "); // Zum Debuggen
        // Serial.println(JSONmessage);  // Zum Debuggen
    }
    else
    {
        Serial.println("Fehler beim Schicken, kein Client verbunden");
    }
}

byte CServer::receiveData(DataReceive & myReceivedData)
{
    byte tempMessageFlag = messageFlag;
    messageFlag = 0;    // Mesage Flag resetten
    if (tempMessageFlag == 0)
    {
        return tempMessageFlag;
    }
    else if (tempMessageFlag == 1)
    {
        myReceivedData.personenzahlAktuell = globalReceivedData.personenzahlAktuell;
        myReceivedData.personenzahlMax = globalReceivedData.personenzahlMax;
        myReceivedData.energiesparmodus = globalReceivedData.energiesparmodus;
        return tempMessageFlag;
    }
    else if (tempMessageFlag == 2)
    {
        myReceivedData.dateTime = globalReceivedData.dateTime;
        return tempMessageFlag;
    }
    else if (tempMessageFlag == 3)
    {
        return tempMessageFlag;
    }
    else
    {
        return 4;
    }
    
}
void CServer::close()
{
    Serial.println("Verbindung wird geschlossen");
    ws.cleanupClients();
    ws.closeAll();
}