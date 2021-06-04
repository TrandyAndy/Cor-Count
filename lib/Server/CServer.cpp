/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-06-04 22:05:36
 * @Description: Voraussetzungen: Webseite im data Ordner auf dem ESP32 hochladen via Platformio "Upload Filesystem Image"
 */
//#include "Global.h"
#include "CServer.h"
#define waitForConnection 10000

DataReceive globalReceivedData;
AsyncWebSocketClient * CServer::globalClient = NULL;
byte messageFlag = 0;

CServer::CServer(String pSSID, String pPassword, String pDomain): 
    ssidSTA(""), 
    passwordSTA(""),
    ssidAP(pSSID), 
    passwordAP(pPassword),
    domain(pDomain), 
    server(80), 
    ws("/ws"), 
    flagAP(true),
    numberOfScannedNetworks(0)
{
    
}

void CServer::setNewConnectionSTA(String pSSID, String pPassword)
{
    ssidSTA = pSSID;
    passwordSTA = pPassword;
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_APSTA);
    bool flagError = false;
    unsigned long startZeit = millis();
    WiFi.begin(ssidSTA.c_str(), passwordSTA.c_str());
    while (WiFi.status() != WL_CONNECTED) 
    {
        if ( (millis() - startZeit) > waitForConnection) // 3 Sekunden Zeit für Verbindung mit dem Router
        {
            Serial.println("STA WLAN Verbindung fehlgeschlagen");
            flagError = true;
            break;
        }
    }
    if (flagError == false)
    {
        Serial.println("");
        Serial.println("Mit Router verbunden");
        IP = WiFi.localIP();
        Serial.print("STA IP ist: ");
        Serial.println(IP);
    }
    else 
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_MODE_AP);
    }
    WiFi.softAP(ssidAP.c_str(), passwordAP.c_str());
    myFlash.saveWlanAccessDataSta(ssidSTA, passwordSTA);
}
void CServer::setNewConnectionAP(String pSSID, String pPassword)
{
    ssidAP = pSSID;
    passwordAP = pPassword;
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(ssidAP.c_str(), passwordAP.c_str());
    IP = WiFi.softAPIP();                     // IP-Adresse auslesen
    Serial.print("AP IP ist: ");
    Serial.println(IP);
    myFlash.saveWlanAccessDataAp(ssidAP, passwordAP);
}

void CServer::init()
{
    if(!SPIFFS.begin())                       // Dateien vom Flashspeicher des ESP32 lesen, inkl. der Webseite
    {
        Serial.println("An Error has occurred while mounting SPIFFS");  // Zum Debuggen
        Serial.println("Sie muessen eine Webseite hochladen!"); // Zum Debuggen
        //return;                               // Wenn keine Webseite hochgeladen wurde, wird kein WiFi eröffnet
    }
    myFlash.loadScannedNetworks(scannedNetworks, numberOfScannedNetworks);
    if(numberOfScannedNetworks == 111)    // Achtung Bug wenn kein WLAN um einen rum ist
    {
        scanNetworks();
        ESP.restart();  // Der ESP spinnt irgendwie nach dem Scannen
    }
    String localSSID_AP, localPasswordAP, localSSID_STA, localPasswordSTA;
    myFlash.loadWlanAccessDataAp(localSSID_AP, localPasswordAP);
    if (localSSID_AP != "") // wenn es gespeicherte Werte gibt, die default Werte ersetzen
    {
        ssidAP = localSSID_AP;
        passwordAP = localPasswordAP;

    }
    myFlash.loadWlanAccessDataSta(localSSID_STA, localPasswordSTA);
    if (localSSID_STA != "") // wenn es gespeicherte Werte gibt, die default Werte ersetzen
    {
        ssidSTA = localSSID_STA;
        passwordSTA = localPasswordSTA;
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.begin(ssidSTA.c_str(), passwordSTA.c_str());
        Serial.println("STA gestartet mit: " + ssidSTA + " " + passwordSTA);
        
        bool flagError = false;
        unsigned long startZeit = millis();
        while (WiFi.status() != WL_CONNECTED) 
        {
            if ( (millis() - startZeit) > waitForConnection) // 3 Sekunden Zeit für Verbindung mit dem Router
            {
                Serial.println("STA WLAN Verbindung fehlgeschlagen");
                flagError = true;
                break;
            }
        }
        if (flagError == false)
        {
            Serial.println("");
            Serial.println("Mit Router verbunden");
            IP = WiFi.localIP();
            Serial.print("STA IP ist: ");
            Serial.println(IP);
        }
        else 
        {
            WiFi.disconnect();
            WiFi.mode(WIFI_MODE_AP);
        }
        Serial.println("Starten AP (Access Point)…");
        WiFi.softAP(ssidAP.c_str(), passwordAP.c_str(), 1, 0, 4);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
        Serial.println("AP gestartet mit: " + ssidAP + " " + passwordAP);
        //WiFi.softAP((char*)"PartyHart2",(char*)"9876543210");        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
        IP = WiFi.softAPIP();                     // IP-Adresse auslesen
        Serial.print("AP IP ist: ");
        Serial.println(IP);
    }
    else // wenn es keine gespeicherten STA Werte gibt, nur einen AP starten
    {
        WiFi.mode(WIFI_MODE_AP);
        Serial.println("Starten AP (Access Point)…");
        WiFi.softAP(ssidAP.c_str(), passwordAP.c_str(), 1, 0, 4);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
        Serial.println("AP gestartet mit: " + ssidAP + " " + passwordAP);
        //WiFi.softAP((char*)"PartyHart2",(char*)"9876543210");        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
        IP = WiFi.softAPIP();                     // IP-Adresse auslesen
        Serial.print("AP IP ist: ");
        Serial.println(IP);
    }
    
    // Ende Dateien vom Speicher laden

    //scanNetworks();

    // WiFi
    
    

    
    
    /*
    if (flagAP)
    {
        Serial.print("Starten AP (Access Point)…");
        WiFi.softAP(ssid, password,1,0,4);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
        IP = WiFi.softAPIP();                     // IP-Adresse auslesen
    }
    else
    {
        Serial.println("Verbinden mit WLAN Router...");
        WiFi.begin(ssid, password);
        bool flagError = false;
        unsigned long startZeit = millis();
        while (WiFi.status() != WL_CONNECTED) 
        {
            if ( (millis() - startZeit) > 2000)
            {
                Serial.println("STA WLAN Verbindung fehlgeschlagen, stattdessen AP starten ...");
                WiFi.disconnect();
                WiFi.softAP(ssid, password,1,0,4);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
                IP = WiFi.softAPIP();                     // IP-Adresse auslesen
                flagError = true;
                break;
            }
        }
        if (flagError == false)
        {
            Serial.println("");
            Serial.println("Mit Router verbunden");
            IP = WiFi.localIP();
        }
    }
    */
    if(MDNS.begin(domain.c_str()))
    {
      Serial.println("DNS gestartet, erreichbar unter: ");
      Serial.println("http://" + domain + ".local/");
    }
    //Serial.print("IP-Adresse: ");
    //Serial.println(IP);                       // IP-Adresse ausgeben
    
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
    server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/" zugreift
    request->send(SPIFFS, "/debug.htm", "text/html"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    server.on("/script_debug.js", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/script.js" zugreift
    request->send(SPIFFS, "/script_debug.js", "text/script"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    server.on("/style_debug.css", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/style.css" zugreift
    request->send(SPIFFS, "/style_debug.css", "text/css"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    server.on("/chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/script.js" zugreift
    request->send(SPIFFS, "/chart.min.js", "text/script"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest * request){
        DynamicJsonDocument sendenJSON(1024);
        String JSONmessage;
        if (ON_STA_FILTER(request))
        {
            sendenJSON["flagConnection"] = "STA";
            serializeJson(sendenJSON, JSONmessage);
            request->send(200, "application/json", JSONmessage);
            Serial.println("Aufruf von STA Client.");   // debug
        }
        else if (ON_AP_FILTER(request)) 
        {
            sendenJSON["flagConnection"] = "AP";
            serializeJson(sendenJSON, JSONmessage);
            request->send(200, "application/json", JSONmessage);
            Serial.println("Aufruf von AP Client.");    // debug
        }
        else
        {
            //request->send(200,"application/json","{\"message\":\"Welcome from Unknwon\"}");
            sendenJSON["flagConnection"] = "ERROR";
            serializeJson(sendenJSON, JSONmessage);
            request->send(200, "application/json", JSONmessage);
            Serial.println("Aufruf von unbekannten Client.");    // debug
        }
    });
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/config" zugreift
        request->send(SPIFFS, "/config.htm", "text/html"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
        if (ON_STA_FILTER(request))
        {
            request->send(200,"application/json","{\"message\":\"Welcome from STA\"}");
            Serial.println("Aufruf von STA Client.");   // debug
        }
        else if (ON_AP_FILTER(request)) 
        {
            request->send(200,"application/json","{\"message\":\"Welcome from AP\"}");
            Serial.println("Aufruf von AP Client.");    // debug
        }
        else
        {
            request->send(200,"application/json","{\"message\":\"Welcome from Unknwon\"}");
            Serial.println("Aufruf von unbekannten Client.");    // debug
        }
    });
    server.on("/style_config.css", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/config" zugreift
    request->send(SPIFFS, "/style_config.css", "text/css"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
    });
    
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){   // wenn der Client auf die Adresse: "192.168.4.1/favicon.ico" zugreift
    request->send(SPIFFS, "/favicon.ico", "image/ico"); // wird das Favicon vom Flash Speicher geladen an an den Client geschickt.
    });
    
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
        
        if (empfangenJSON.containsKey("ssidAP"))
        {
            globalReceivedData.ssidAP = empfangenJSON["ssidAP"].as<String>();
            globalReceivedData.passwordAP = empfangenJSON["passwordAP"].as<String>();
            Serial.println(globalReceivedData.passwordAP);
            Serial.println(globalReceivedData.ssidAP);
            messageFlag = 4;
        }
        else if (empfangenJSON.containsKey("ssidSTA"))
        {
            globalReceivedData.ssidSTA = empfangenJSON["ssidSTA"].as<String>();
            globalReceivedData.passwordSTA = empfangenJSON["passwordSTA"].as<String>();
            Serial.println(globalReceivedData.passwordSTA);
            Serial.println(globalReceivedData.ssidSTA);
            messageFlag = 5;
        }

        else if (empfangenJSON["energiesparmodus"].as<String>() == " ")  // wenn initiale Message von der Webseite kommt
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
    if(isThereAnyClient())      // wenn die Verbindung zur Webseite besteht
    {
        DynamicJsonDocument sendenJSON(1024);
        sendenJSON["personenzahlMax"] = mySendData.personenzahlMax;
        sendenJSON["personenzahlAktuell"] = mySendData.personenzahlAktuell;
        sendenJSON["akkustand"] = mySendData.akkustand;
        sendenJSON["energiesparmodus"] = mySendData.energiesparmodus;
        sendenJSON["flagGetTime"] = mySendData.flagGetTime;
        String JSONmessage;
        serializeJson(sendenJSON, JSONmessage);
        //globalClient->text(JSONmessage);    // nur ein Client wird behandelt
        ws.textAll(JSONmessage);  // alternative mehrere Clients bekommen selbe anzeige
        // Serial.print("Gesendete Daten sind: "); // Zum Debuggen
        // Serial.println(JSONmessage);  // Zum Debuggen
    }
    else
    {
        Serial.println("Fehler beim Schicken, kein Client verbunden");
    }
}
void CServer::transmitDataWIFI()
{
    if(isThereAnyClient())      // wenn die Verbindung zur Webseite besteht
    {
        
        DynamicJsonDocument sendenJSON(4048);
        JsonArray scannedNetworksJSON = sendenJSON.createNestedArray("scannedNetworks");
        for (byte i = 0; i < numberOfScannedNetworks; i++)
        {
            scannedNetworksJSON.add(scannedNetworks[i]);
        }
        sendenJSON["ssidAP"] = ssidAP;
        sendenJSON["passwordAP"] = passwordAP;
        sendenJSON["ssidSTA"] = ssidSTA;
        sendenJSON["passwordSTA"] = passwordSTA;
        IPAddress localIP = WiFi.softAPIP();
        sendenJSON["ipAP"] = WiFi.softAPIP().toString();
        sendenJSON["ipSTA"] = WiFi.localIP().toString();
    
        String JSONmessage;
        serializeJson(sendenJSON, JSONmessage);
        //globalClient->text(JSONmessage);    // nur ein Client wird behandelt
        ws.textAll(JSONmessage);  // alternative mehrere Clients bekommen selbe anzeige
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
    else if (tempMessageFlag == 4)
    {
        myReceivedData.ssidAP = globalReceivedData.ssidAP;
        myReceivedData.passwordAP = globalReceivedData.passwordAP;
        return tempMessageFlag;
    }
    else if (tempMessageFlag == 5)
    {
        myReceivedData.ssidSTA = globalReceivedData.ssidSTA;
        myReceivedData.passwordSTA = globalReceivedData.passwordSTA;
        return tempMessageFlag;
    }
    else 
    {
        return 100;
    }
    
}
void CServer::sendDebugMessage(String msg)
{
    if(isThereAnyClient()) // wenn eine Verbindung zur Webseite besteht
    {
        DynamicJsonDocument sendenJSON(1024);
        sendenJSON["debug_msg"] = msg;
        String JSONmessage;
        serializeJson(sendenJSON, JSONmessage);
        ws.textAll(JSONmessage);  // alternative mehrere Clients bekommen selbe anzeige
        // Serial.print("Gesendete Daten sind: "); // Zum Debuggen
        // Serial.println(JSONmessage);  // Zum Debuggen
    }
}
void CServer::sendTOFData(float dataTOF1, float dataTOF2)
{
    if(isThereAnyClient()) // wenn eine Verbindung zur Webseite besteht
    {
        DynamicJsonDocument sendenJSON(34);
        sendenJSON["tof1"] = dataTOF1;
        sendenJSON["tof2"] = dataTOF2;
        String JSONmessage;
        serializeJson(sendenJSON, JSONmessage);
        ws.textAll(JSONmessage);  // alternative mehrere Clients bekommen selbe anzeige
        // Serial.print("Gesendete Daten sind: "); // Zum Debuggen
        // Serial.println(JSONmessage);  // Zum Debuggen
    }
}
void CServer::close()
{
    Serial.println("Verbindung wird geschlossen");
    ws.cleanupClients();
    ws.closeAll();
}

bool CServer::isThereAnyClient()
{
    AsyncWebSocket::AsyncWebSocketClientLinkedList myClients =  ws.getClients();    // Liste aller Clients laden
    bool clientAvailable = ! myClients.isEmpty();
    //Serial.println(clientAvailable);     // Zum Debuggen
    return clientAvailable;        // gibt es kein Client --> false, gibt es Clients --> true
}

void CServer::scanNetworks()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    numberOfScannedNetworks = n;
    Serial.println("scan done");
    if (n == 0) 
    {
        Serial.println("no networks found");
    } 
    else 
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) 
        {
            scannedNetworks[i] = WiFi.SSID(i);
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    WiFi.disconnect();
    myFlash.saveScannedNetworks(scannedNetworks, numberOfScannedNetworks);
}

bool CServer::checkIsWorkingSTA(bool send)
{
    bool isActiveSTA_new = ( WiFi.status() == WL_CONNECTED ); 
    if ( (isActiveSTA != isActiveSTA_new) || send)
    {
        if(isThereAnyClient()) // wenn eine Verbindung zur Webseite besteht
        {
            DynamicJsonDocument sendenJSON(32);
            sendenJSON["isActiveSTA"] = isActiveSTA_new;
            String JSONmessage;
            serializeJson(sendenJSON, JSONmessage);
            ws.textAll(JSONmessage);  // alternative mehrere Clients bekommen selbe anzeige
        }
        Serial.print("Status STA: ");
        Serial.println(isActiveSTA_new);
    }
    isActiveSTA = isActiveSTA_new;
    return isActiveSTA;
    
}