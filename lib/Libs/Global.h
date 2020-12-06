/* 
Hier werden alle Globalen Bibliotheken, Defines, und Enums erzeugt.

Autor: Andy

*/

//Inlcudes:
#include <Arduino.h>    //Grundlegende Funktionen   Nutzung: Andy
#include <Wire.h>       //I²C Funktion              Nutzung: Sensor.h
#include <EEPROM.h>     //Speichern des Zaehlers

//Pin Map:
#define WakeupPin 33    //RTC Fähiger Pin 

//Global Veriablen:
RTC_DATA_ATTR int aufwachZaehler = 0;   //Im RTC nciht von DeepSleep betroffen
long unsigned int leerlaufZeit =0;      //Zeit in der nichts passiert

//Global Objekte:



//Defines:
#define Schalfenszeit 2000  //Wartezeit für DeepSleep in ms