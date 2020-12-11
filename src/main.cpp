/* Corona-Ampel
Ein System zum Zählen von Personenfluss durch Türen. 
Insbensondere um die Corona-Beschränkungen erfüllen zu können. 
Als Steuerung wird ein ESP32 der Firma Espressif eingesetzt.

HIT Projekt WS20/21
Version: 0.1  Datum: 05.12.20
*/
#include "Global.h"   //Global wird überall includiert.
#include "CorCount.h"
#include "CSensor.h"

CSchalfen ESP_schlaf; //Oder in Global?
CSensor ToF_innen(24);
CSignalLicht L(5);
CSignalLicht R(18);

void setup()
{
  Serial.begin(115200);
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
  Serial.println("Hallo Team Cor-Count");
  Serial.println(menschenImRaum++);
  delay(800);
  L.LDR_pruefen();
  R.setLicht(true);
  delay(100);
  R.setLicht(false);
  R.LDR_pruefen();
  ESP_schlaf.resetSleepTime();
  ESP_schlaf.energiesparen(); //Sende ESP in den Deepsleep

  Serial.println("Hallo Corona Team");

} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende