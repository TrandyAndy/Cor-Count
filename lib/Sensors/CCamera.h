/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-01-04 16:22:09
 * @Description: Anbindung der Camera an die Hauptsteuerung über 3 GPIO Pins
 */


#pragma once

#include <Arduino.h>
class CCamera
{
    public:
        CCamera(byte pPinEventEntry, byte pPinEventExit, byte pPinWakeUp); // Kunstruktor
        void setup();           // muss in der setup aufgerufen werden!!!
        void run();             // muss in der loop aufgerufen werden
        void wakeUpCamera();    // Die Kamera soll aufgewacht werden
        void setCameraToSleep();// Die Kamera soll schlafen gehen
    private:
        byte pinEventEntry;     // Pin des Inputs für Personen die hineingehen    
        byte pinEventExit;      // Pin des Inputs für Personen die hinausgehen
        byte pinWakeUp;         // Pin für das aufwachen der Kamera
        static void IRAM_ATTR isrEventEntry();    // ISR soll aufgerufen werden, wenn eine Person hineinläuft
        static void IRAM_ATTR isrEventExit();     // ISR soll aufgerufen werden, wenn eine Person herausläuft
};