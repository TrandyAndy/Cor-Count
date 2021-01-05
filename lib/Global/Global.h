/* 
Hier werden alle Globalen Bibliotheken, Defines, und Enums erzeugt.

Autor: Andy

*/

//Inlcudes:
#include <Arduino.h>    //Grundlegende Funktionen   Nutzung: Andy
#include <Wire.h>       //I²C Funktion              Nutzung: Sensor.h


//Pin Map:
#define WakeupPin 33    //RTC Fähiger Pin 
#define LED_Rot 5       //GPIO
#define LED_Gruen 6
#define pinCamereaEventEntry 32 
#define pinCameraEventExit 33
#define pinCameraWakeUp 25

//Global Veriablen:         // Julian: geht das so überhaupt? 
RTC_DATA_ATTR int aufwachZaehler = 0;   //Im RTC nciht von DeepSleep betroffen
long unsigned int leerlaufZeit = 0;      //Zeit in der nichts passiert
int menschenImRaum = 0; // aktuelle Personenzahl im Raum
int menschenImRaumMax = 0;  // maximale Personenzahl im Raum
bool energiesparmodus = false;
bool flagGetTime = false;
byte akkustand = 0;
//Global Objekte:



//Defines:
#define Schalfenszeit 4000              //Wartezeit für DeepSleep in ms
#define AdresseMesnchenZaehler  0       //Speicherort ! Nur 0 geht???
#define PersonErkannt 1
#define NiemandDa 0
#define ImProzess 2
#define LichtAn true
#define Grundhelligkeit 50               //Grundwert für die LEDs in %
#define HelligkeitGain 10