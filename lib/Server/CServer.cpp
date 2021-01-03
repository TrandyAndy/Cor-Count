#include "CServer.h"

CServer::CServer(char* pSSID, char* pPassword)
{
    ssid = pSSID;
    password = pPassword;
}

void CServer::setup()
{
    AsyncWebServer server(80);                  // Set web server port number to 80
    AsyncWebSocket ws("/ws");                   // AyncWebSocket wird erstellt
    AsyncWebSocketClient * globalClient = NULL; // Pointer auf den Client, für Überprüfung der Verbindung
    // Dateien vom Speicher laden
    if(!SPIFFS.begin())                       // Dateien vom Flashspeicher des ESP32 lesen, inkl. der Webseite
    {
        Serial.println("An Error has occurred while mounting SPIFFS");  // Zum Debuggen
        Serial.println("Sie muessen eine Webseite hochladen!"); // Zum Debuggen
        //return;                               // Wenn keine Webseite hochgeladen wurde, wird kein WiFi eröffnet
    }
    // Ende Dateien vom Speicher laden

    // WiFi
    Serial.print("Starten AP (Access Point)…");
    WiFi.softAP(ssid, password,1,0,1);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
    IP = WiFi.softAPIP();                     // IP-Adresse auslesen
    // if(MDNS.begin(dnsName))
    // {
    //   Serial.println("DNS gestartet, erreichbar unter: ");
    //   Serial.println("http://" + String(dnsName)+ ".local/");
    // }
    Serial.print("AP IP-Adresse: ");
    Serial.println(IP);                       // IP-Adresse ausgeben
    ws.onEvent(onWsEvent);                    // Funktion bekannt machen, die aufgerufen werden soll bei einem WS Events
    server.addHandler(&ws);                   // Handler hinzufügen
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/" zugreift
    request->send(SPIFFS, "/index.htm", "text/html"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
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
    Serial.println("Internet ist neuland");
}
