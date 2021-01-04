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
bool messageFlag = false;       // Nachrichtenflag für Webseiten Daten

// Kamera Objekt erstellen:
CCamera myCamera(pinCamereaEventEntry, pinCameraEventExit, pinCameraWakeUp);




CSchlafen ESP_schlaf(1); //Oder in Global?
//CSchlafen ESP();
CSensor ToF_innen(24);
CSignalLicht L(5);
CSignalLicht R(18);

void setup()
{
  Serial.begin(115200);
  // Webserver starten:
  myServer.setup();     // Server wird gestartet
  // Kamera Objekt erstellen:
  myCamera.setup();     // Pins der Kamera werden aktiviert   

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
  myCamera.run();

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

} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende