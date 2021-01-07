#include <Arduino.h>

class CZaehler
{
    public:
        int8_t updateZaehler(int8_t cameraEvent, int8_t tofEvent);  // Rückgabe: Änderung des Zählers, Parameter 1: Event der Kamera, Parameter 2: Event des TOF-Sensors
};

