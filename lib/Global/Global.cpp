#include <Arduino.h>
#include "CServer.h"

// globale Variablen
RTC_DATA_ATTR int aufwachZaehler = 0;   //Im RTC nciht von DeepSleep betroffen
long unsigned int leerlaufZeit = 0;      //Zeit in der nichts passiert
int menschenImRaum = 0; // aktuelle Personenzahl im Raum
int menschenImRaumMax = 0;  // maximale Personenzahl im Raum
bool energiesparmodus = false;
bool flagGetTime = false;
byte akkustand = 0;

// globale Objekte
CServer myServer((char*)"Cor-Count",(char*)"COVID-19", (char*)"cor-count");    