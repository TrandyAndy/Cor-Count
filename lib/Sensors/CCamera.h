/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-01-05 21:30:27
 * @Description: Anbindung der Camera an die Hauptsteuerung über 3 GPIO Pins
 */


#pragma once

#include <Arduino.h>
class CCamera
{
    public:
        CCamera(byte pPinEventEntry, byte pPinEventExit, byte pPinWakeUp); // Kunstruktor
        void init();            // muss in der setup aufgerufen werden!!!
        int8_t run();           // muss in der loop aufgerufen werden, Rückgabe: 0 = nichts, -1 = rauslaufen, 1 = reinlaufen
        void wakeUpCamera();    // Die Kamera soll aufgewacht werden
        void setCameraToSleep();// Die Kamera soll schlafen gehen
    private:
        byte pinEventEntry;     // Pin des Inputs für Personen die hineingehen    
        byte pinEventExit;      // Pin des Inputs für Personen die hinausgehen
        byte pinWakeUp;         // Pin für das aufwachen der Kamera
        static void IRAM_ATTR isrEventEntry();    // ISR soll aufgerufen werden, wenn eine Person hineinläuft
        static void IRAM_ATTR isrEventExit();     // ISR soll aufgerufen werden, wenn eine Person herausläuft
};