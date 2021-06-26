/* Corona-Ampel
Ein System zum Zählen von Personenfluss durch Türen. 
Insbensondere um die Corona-Beschränkungen erfüllen zu können. 
Als Steuerung wird ein ESP32 der Firma Espressif eingesetzt.

HIT Projekt WS20/21
Version: 0.1  Datum: 05.12.20
*/
#include <Arduino.h>
#include "Global.h"   //Global wird überall inkludiert.
#include <Wire.h>
#include "CSaveLoad.h"
#include "CorCount.h"
#include "CSensor.h"
#include "CServer.h"  // Webserver auf dem ESP32
#include "CCamera.h"  // Klasse für die Anbindung der Kamera
#include "Global.h"
#include "CTof.h"
#include "CBattery.h"
#include "CZaehler.h"



// Webserver erstellen, mit SSID, Passwort und domain:
// CServer ist global // CServer myServer((char*)"Cor-Count",(char*)"COVID-19", (char*)"cor-count");    
DataSend mySendData;            // Strukt mit den zu sendenden Daten
DataReceive myReceivedData;     // Strukt mit den zu empfangenen Daten

// Kamera Objekt erstellen:
CCamera myCamera(pinCamereaEventEntry, pinCameraEventExit, pinCameraWakeUp);
bool once = true;

//Tof Obj erstellen:
CTof myTOF;

// Batterie Objekt erstellen:
CBattery myBattery(pinBattery);

// Zaehler Objekt erstellen:
CZaehler myZaehler;

CSchlafen ESP_schlaf(1); //Oder in Global?
//CSchlafen ESP();
CSensor ToF_innen(24);
//CSignalLicht L(5);
CSignalLicht Stop(Rot,LED_Rot);
CSignalLicht Go(Gruen,LED_Gruen);
CSignalLicht MultiMatrix(Rot,LED_Rot); //Farbe ist hier egal. LED_Rot Pin oder anderer verwenden.

void checkIfNewMessageFromServer(); // Überprüft ob eine neue Nachricht von der Webseite vorliegt, falls ja wird diese gelesen und gespeichert
void wakeupISR();

void setup()
{
  //pinMode(SDA,INPUT_PULLUP);
  //pinMode(SCL,INPUT_PULLUP);
  Serial.begin(115200);
  // Kamera starten:
  myCamera.init();     // Pins der Kamera werden aktiviert
  myCamera.wakeUpCamera();  // Kamera ESP32 aufwecken
  Serial.println("Before Flash init");
  // Flash starten
  myFlash.init();
  Serial.println("After Flash init");
  // Webserver starten:

  // Mit Router verbinden:
  //myServer.setNewConnection((char*)"PartyHart",(char*)"9876543210", false);
  //myServer.setNewConnection((char*)"FRITZ!Box 7590 UP",(char*)"45525509900873963179", false);
  

  
  myServer.init();     // Server wird gestartet
  //myServer.setNewConnectionAP((char *) "hallowasgehtHALLO", (char *) "123456789");
  
  // delay(10000);
  // hier müssen noch die gespeicherten Daten geschickt werden
  // myServer.transmitData(mySendData);


  // Lichtschranke starten
  //Wire.begin(SDA, SCL); //!!!!! Macht vill Probleme
 // Lichtschranke.init();
 myTOF.init();


  MultiMatrix.init();
  // Init Batterie
  myBattery.init();
  // PIR PIN zum Aufwachen
  pinMode(WakeupPin, INPUT_PULLDOWN);
  attachInterrupt(WakeupPin, wakeupISR, RISING);

  if(aufwachZaehler>0){
  Serial.println("Zum "+ String(aufwachZaehler)+" mal Aufgewacht");
   print_wakeup_reason();
  }
  else{
  Serial.println("Setup Abgeschlossen. Neustart");
  }
  aufwachZaehler++;
  /* Wurden durch neue Funktionen von Julian ersetzt
  //Daten Rücklesen
  EEPROM.begin(46); //46 Byte Reservieren
  menschenImRaumMax=ESP_schlaf.getData(AdresseMesnchenMax);
  menschenImRaum=ESP_schlaf.getData(AdresseMesnchenZaehler);
  energiesparmodus=ESP_schlaf.getData(Adresseenergiesparmodus);
  */
  myFlash.loadMenschenImRaum(menschenImRaum);
  myFlash.loadMenschenImRaumMax(menschenImRaumMax);
  myFlash.loadEnergiesparmodus(energiesparmodus);
  Serial.print("Energiespaeren war : "); //Debug
  Serial.println(energiesparmodus);
  Serial.println(menschenImRaumMax);
  Serial.println(menschenImRaum);
  myServer.sendDebugMessage("Setup abgeschlossen");
}

//int i = 0;

void loop() //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi
{ 
  checkIfNewMessageFromServer();
  int8_t zaehlerAenderung = myZaehler.updateZaehler(myCamera.run(), myCamera.run()); // Lichtschranke.get_Direction());  // Sensor Fusion, mit Kamera und Sensor Ergebnis aufrufen
  menschenImRaum += zaehlerAenderung;
  if(zaehlerAenderung != 0)
  {
    mySendData.akkustand = akkustand; // temp: // hier wird der aktuelle Akkustand geschickt
    mySendData.personenzahlAktuell = menschenImRaum; // hier wird die aktuelle Personenzahl geschickt
    mySendData.flagGetTime = false; // keine Zeit anforderun,
    myServer.transmitData(mySendData);  // Daten an Webseite schicken
  }
  akkustand = myBattery.getBatteryLevel();
  //Serial.printf("Batterieladezustand: %d %% \n",akkustand); // debug
  // Lichtschranke.run();
  if (menschenImRaum>=menschenImRaumMax) //Wichtigste if im ganzen System
  {
    Go.setLicht(false);
    Stop.setLicht(true);
    MultiMatrix.SetSymbol_Cross();
  }
  else
  {
    Go.setLicht(true);
    Stop.setLicht(false);
    MultiMatrix.SetSymbol_Arrow();
  }
  myServer.checkIsWorkingSTA();   // überprüft ob eine Verbindung zum Router besteht

  ESP_schlaf.energiesparen(); //Sende ESP in den Deepsleep wenn es zeit ist.
  if (!energiesparmodus)
    ESP_schlaf.resetSleepTime(); //Verzögert Energiesparen
  //myServer.sendDebugMessage("Hallo hier ist der ESP32");
  // myServer.sendTOFData((float)i/2.0,(float)i/10.0);
  //myServer.sendTOFData(millis() % 10, millis() % 10 + 10);
  //delay(100);
  //i++;

  /*
  myFlash.saveWlanAccessDataSta("test12345","test54321");
  delay(10000);
  String ssidAP, pswAP;
  myFlash.loadWlanAccessDataSta(ssidAP, pswAP);
  Serial.print("Daten sind: ");
  Serial.println(ssidAP);
  Serial.println(pswAP);
  delay(10000);
  */

} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende



void checkIfNewMessageFromServer()
{
  switch (myServer.receiveData(myReceivedData))
  {
  case noMessage:   // 0 = keine Nachricht,
    //Serial.println("Keine Nachrichten");  // debug
    break;
  case changeOnWebsite:   // 1 = Änderungen an der Webseite,
    // debug Ausgabe
    Serial.println("Empfangene Daten: "); //debug
    Serial.print("Enegiesparmodus: ");  //debug
    Serial.println(myReceivedData.energiesparmodus);  //debug
    Serial.print("Personenzahl aktuell: "); //debug
    Serial.println(myReceivedData.personenzahlAktuell);   //debug
    Serial.print("Personenzahl maximal: "); //debug
    Serial.println(myReceivedData.personenzahlMax); //debug
    // debug Ausgabe Ende

    if (energiesparmodus != myReceivedData.energiesparmodus)
    {
      myFlash.saveEnergiesparmodus(myReceivedData.energiesparmodus);
      //ESP_schlaf.datensichern( (int) myReceivedData.energiesparmodus, Adresseenergiesparmodus); 
      Serial.printf("Energiesparmodus wird gespeichert: %d",(int) myReceivedData.energiesparmodus);
    }
    // Empfangene Daten speichern
    menschenImRaum = myReceivedData.personenzahlAktuell;
    menschenImRaumMax = myReceivedData.personenzahlMax;
    energiesparmodus = myReceivedData.energiesparmodus;

    ESP_schlaf.resetSleepTime(); //Verzögert energiesparen Besser die client.connekt abfragen!

    mySendData.akkustand = akkustand; // temp: // hier wird der aktuelle Akkustand geschickt
    mySendData.personenzahlAktuell = menschenImRaum;  // hier wird die aktuelle Personenzahl geschickt
    mySendData.personenzahlMax = menschenImRaumMax; // maximale Personenzahl im Raum schicken
    mySendData.energiesparmodus = energiesparmodus; // temp: // energiesparmodus schicken;
    mySendData.flagGetTime = false; // keine Zeit anforderun,
    myServer.transmitData(mySendData);  // Daten an Webseite schicken
    break;

  case getTimeFromWebsite:   // 2 = Datum und Zeit wird geschickt nachdem eine Person durchgelaufen ist
    // debug Ausgabe
    Serial.println("Empfangene Daten: "); //debug
    Serial.print("Das Datum: ");  //debug
    Serial.println(myReceivedData.dateTime);  //debug
    // debug Ausgabe Ende
    break;
  case initialMessage:   // 3 = initiale Nachricht
    myServer.checkIsWorkingSTA(true);
    Serial.println("Website verbunden:"); //debug
    mySendData.akkustand = akkustand; // hier wird der aktuelle Akkustand geschickt
    mySendData.personenzahlAktuell = menschenImRaum; // hier wird die aktuelle Personenzahl geschickt
    mySendData.personenzahlMax = menschenImRaumMax; // maximale Personenzahl im Raum schicken
    mySendData.energiesparmodus = energiesparmodus; // energiesparmodus schicken;
    mySendData.flagGetTime = false; // keine Zeit anforderun,
    myServer.transmitData(mySendData);  // Daten an Webseite schicken
    myServer.transmitDataWIFI();  // Die gespeicherten WiFi Daten schicken
    ESP_schlaf.resetSleepTime();
    break;
  case wlanConfigAP: // 4 = WLAN AP config
    Serial.println("WLAN Config AP"); //debug
    myServer.setNewConnectionAP(myReceivedData.ssidAP, myReceivedData.passwordAP);
    break;
  case wlanConfigSTA: // 5 = WLAN STA config
    Serial.println("WLAN Config STA"); //debug
    myServer.setNewConnectionSTA(myReceivedData.ssidSTA, myReceivedData.passwordSTA);
    break;
  default:  // Fehler
    Serial.println("Fehler des Servers beim Empfangen der Nachrichten.");
    break;
  }
}
void wakeupISR()
{
  ESP_schlaf.resetSleepTime(); //Verzögert Energiesparen
}