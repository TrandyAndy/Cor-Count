/* 
Hier werden alle Globalen Bibliotheken, Defines, und Enums erzeugt.

Autor: Andy

*/

//Inlcudes:
#include <Arduino.h>    //Grundlegende Funktionen   Nutzung: Andy
#include <Wire.h>       //I²C Funktion              Nutzung: Sensor.h


//Pin Map:
#define WakeupPin 33    //RTC Fähiger Pin 

//Global Veriablen:
RTC_DATA_ATTR int aufwachZaehler = 0;   //Im RTC nciht von DeepSleep betroffen
long unsigned int leerlaufZeit =0;      //Zeit in der nichts passiert
int menschenImRaum =0;

//Global Objekte:



//Defines:
#define Schalfenszeit 4000              //Wartezeit für DeepSleep in ms
#define AdresseMesnchenZaehler  0       //Speicherort ! Nur 0 geht???
#define PersonErkannt 1
#define NiemandDa 0
#define ImProzess 2