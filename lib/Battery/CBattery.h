#pragma once

#include <Arduino.h>
#include "Global.h"

class CBattery
{
    public:
        CBattery(byte pPinBattery); // Kunstruktor mit verwendeten Pins
        void init();                // Pins werden zugewisen
        uint8_t getBatteryLevel();  // Gibt den Ladezustand der Batterie in % zurück.
    private:
        byte mPinBattery;
};