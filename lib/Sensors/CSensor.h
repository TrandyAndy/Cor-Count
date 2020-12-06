/*
Sensor Basisklasse
Definiert die Grundfunktionen eines Sensors ToF oder Kamera
Autor: Andy
*/
#pragma once
//#include <Global.h>

class CSensor{
    public:
    //Konstruktor Initialisiert den Sensor (Wakeup)
    CSensor(int Adresse);      

    ~CSensor();

    //Wird Koninuierlich aufgerufen. 
    //Returnt #PersonErkannt oder NiemandDa ...
    int statusUpdate();         

    private:
    //Interne Main Funktion
    int SensorMain();        

    //Variablen
    double interneBeispielVar=0;   

};