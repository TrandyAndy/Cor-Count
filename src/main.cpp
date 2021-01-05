/* Corona-Ampel
Ein System zum Zählen von Personenfluss durch Türen. 
Insbensondere um die Corona-Beschränkungen erfüllen zu können. 
Als Steuerung wird ein ESP32 der Firma Espressif eingesetzt.

HIT Projekt WS20/21
Version: 0.1  Datum: 05.12.20
*/
#include "Global.h"   //Global wird überall inkludiert.
#include "CorCount.h"
#include "CSensor.h"
#include "CServer.h"  // Webserver auf dem ESP32
#include "CCamera.h"  // Klasse für die Anbindung der Kamera

// Webserver erstellen, mit SSID, Passwort und domain:
CServer myServer((char*)"Cor-Count",(char*)"COVID-19", (char*)"cor-count");    
DataSend mySendData;            // Strukt mit den zu sendenden Daten
DataReceive myReceivedData;     // Strukt mit den zu empfangenen Daten


// Kamera Objekt erstellen:
CCamera myCamera(pinCamereaEventEntry, pinCameraEventExit, pinCameraWakeUp);

CSchlafen ESP_schlaf(1); //Oder in Global?
//CSchlafen ESP();
CSensor ToF_innen(24);
CSignalLicht L(5);
CSignalLicht R(18);

void checkIfNewMessageFromServer();
int8_t updateZaehler(int8_t cameraEvent, int8_t tofEvent);   // Rückgabe: Änderung des Zählers, Parameter 1: Event der Kamera, Parameter 2: Event des TOF-Sensors

void setup()
{
  Serial.begin(115200);
  // Webserver starten:
  myServer.init();     // Server wird gestartet
  // hier müssen noch die gespeicherten Daten geschickt werden
  //myServer.transmitData(mySendData);
  // Kamera starten:
  myCamera.init();     // Pins der Kamera werden aktiviert

  if(aufwachZaehler>0){
  Serial.println("Zum "+ String(aufwachZaehler)+" mal Aufgewacht");
  print_wakeup_reason();
  }
  else{
  Serial.println("Setup Abgeschlossen. Neustart");
  }
  aufwachZaehler++;
  //Daten Rücklesen
  EEPROM.begin(1); //Warum 1? Andy
  menschenImRaum=ESP_schlaf.getData();
}


void loop() //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi
{ 
  checkIfNewMessageFromServer();
  //myCamera.run();
  int8_t zaehlerAenderung = updateZaehler(myCamera.run(), 0);  // Sensor Fusion, mit Kamera und Sensor Ergebnis aufrufen
  menschenImRaum += zaehlerAenderung;
  if(zaehlerAenderung != 0)
  {
    mySendData.akkustand = menschenImRaum; // temp: // hier wird der aktuelle Akkustand geschickt
    mySendData.personenzahlAktuell = menschenImRaum; // temp: // hier wird die aktuelle Personenzahl geschickt
    mySendData.flagGetTime = false; // keine Zeit anforderun,
    myServer.transmitData(mySendData);  // Daten an Webseite schicken
  }

  /* Auskommentiert, weil der ESP schon schlafen geht, bevor man sich mit dem Wlan verbinden kann. 
  Serial.println("Hallo Team Cor-Count");
  Serial.println(menschenImRaum++);
  delay(800);
  L.LDR_pruefen();
  R.setLicht(true);
  delay(100);
  R.setLicht(false);
  R.LDR_pruefen();
  //ESP_schlaf.resetSleepTime();
  ESP_schlaf.energiesparen(); //Sende ESP in den Deepsleep

  Serial.println("Hallo Corona Team");
  */

} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende



void checkIfNewMessageFromServer()
{
  switch (myServer.receiveData(myReceivedData))
  {
  case 0:   // 0 = keine Nachricht,
    //Serial.println("Keine Nachrichten");  // debug
    break;
  case 1:   // 1 = Änderungen an der Webseite,
    // debug Ausgabe
    Serial.println("Empfangene Daten: "); //debug
    Serial.print("Enegiesparmodus: ");  //debug
    Serial.println(myReceivedData.energiesparmodus);  //debug
    Serial.print("Personenzahl aktuell: "); //debug
    Serial.println(myReceivedData.personenzahlAktuell);   //debug
    Serial.print("Personenzahl maximal: "); //debug
    Serial.println(myReceivedData.personenzahlMax); //debug
    // debug Ausgabe Ende

    // Empfangene Daten speichern
    menschenImRaum = myReceivedData.personenzahlAktuell;
    menschenImRaumMax = myReceivedData.personenzahlMax;
    energiesparmodus = myReceivedData.energiesparmodus;


    mySendData.akkustand = 95; // temp: // hier wird der aktuelle Akkustand geschickt
    mySendData.personenzahlAktuell = myReceivedData.personenzahlAktuell; // temp: // hier wird die aktuelle Personenzahl geschickt
    mySendData.personenzahlMax = myReceivedData.personenzahlMax; // temp: // maximale Personenzahl im Raum schicken
    mySendData.energiesparmodus = myReceivedData.energiesparmodus; // temp: // energiesparmodus schicken;
    mySendData.flagGetTime = false; // keine Zeit anforderun,
    myServer.transmitData(mySendData);  // Daten an Webseite schicken
    break;

  case 2:   // 2 = Datum und Zeit wird geschickt nachdem eine Person durchgelaufen ist
    // debug Ausgabe
    Serial.println("Empfangene Daten: "); //debug
    Serial.print("Das Datum: ");  //debug
    Serial.println(myReceivedData.dateTime);  //debug
    // debug Ausgabe Ende
    break;
  default:  // Fehler
    Serial.println("Fehler des Servers beim Empfangen der Nachrichten.");
    break;
  }
}

int8_t updateZaehler(int8_t cameraEvent, int8_t tofEvent)
{
  if (cameraEvent == 0 && tofEvent == 0)  // keine Änderung
  {
    return 0;
  }
  else // Sensor Fusion
  {
    // Bei verunden: speichert Millis vom ersten Sensor ab, zweite Sensor warten. 
    return cameraEvent; // temp
    // return tofEvent;  // temp
  }
}

int8_