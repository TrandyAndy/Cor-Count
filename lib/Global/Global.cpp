#include <Arduino.h>
#include "Global.h"
//#include "CServer.h"
//#include "CSaveLoad.h"

// globale Variablen
RTC_DATA_ATTR int aufwachZaehler = 0;   //Im RTC nciht von DeepSleep betroffen
long unsigned int leerlaufZeit = 0;      //Zeit in der nichts passiert
int menschenImRaum = 0; // aktuelle Personenzahl im Raum
int menschenImRaumMax = 0;  // maximale Personenzahl im Raum
bool energiesparmodus = false;
bool flagGetTime = false;
byte akkustand = 0;
bool tooBrightForTof = 0;

// globale Objekte
CServer myServer("Counting-Cube", "COVID-19", "counting-cube");    // Cor-Count
CSaveLoad myFlash;