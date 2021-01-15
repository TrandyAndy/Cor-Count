/* 
Hier werden alle Globalen Bibliotheken, Defines, und Enums erzeugt.

Autor: Andy

*/

//Inlcudes:
#include <Arduino.h>    //Grundlegende Funktionen   Nutzung: Andy
#include <Wire.h>       //I²C Funktion              Nutzung: Sensor.h
#include "CServer.h"


//Pin Map:
//#define WakeupPin 33    //RTC fähiger Pin 
#define WakeupPin 15    //by Julian
#define LED_Rot 12       //GPIO
#define LED_Gruen 14
#define SHT_LOX1 2      //shutdown Pin Tof1
#define SHT_LOX2 4      //shutdown Pin Tof2
#define TOF1_PIN 16     //Tof 1 Interrupt
#define TOF2_PIN 17     //Tof 2 Interrupt
#define pinBattery 32
#define pinCamereaEventEntry 33 
#define pinCameraEventExit 25
#define pinCameraWakeUp 26



//Global Veriablen:         // Julian: kein schöner Stil, wurde gezwungen dass so zu machen.
extern RTC_DATA_ATTR int aufwachZaehler;   //Im RTC nciht von DeepSleep betroffen
extern long unsigned int leerlaufZeit;      //Zeit in der nichts passiert
extern int menschenImRaum; // aktuelle Personenzahl im Raum
extern int menschenImRaumMax;  // maximale Personenzahl im Raum
extern bool energiesparmodus;
extern bool flagGetTime;
extern byte akkustand;


//Global Objekte:

extern CServer myServer;    

//Defines:
#define Schalfenszeit 80000             //Wartezeit für DeepSleep in ms //ehemals 4000
#define SolangesollderESPschalfen 60    //sec
#define AdresseMesnchenZaehler  0       //Speicherort ! Nur 0 geht???
#define AdresseMesnchenMax 2
#define Adresseenergiesparmodus 4
#define PersonErkannt 1
#define NiemandDa 0
#define ImProzess 2
#define LichtAn true
#define Grundhelligkeit 50               //Grundwert für die LEDs in %
#define Rot 0                            //RGB 0,1,2
#define Gruen 1
#define HelligkeitGain 10
#define PIXEL_COUNT 64                  // Number of NeoPixels
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

//Batterie
#define faktorSpannungsteiler 0.5       // Faktor des Spannungsteiler [ R2 / (R1+R2) ], z.B. R1 = 1k und R2 = 1k--> 0.5 oder R1 = 3k und R2 = 1k  --> 0.25
