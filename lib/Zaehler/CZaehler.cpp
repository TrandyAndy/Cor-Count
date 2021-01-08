#include "CZaehler.h"

int8_t CZaehler::updateZaehler(int8_t cameraEvent, int8_t tofEvent)
{
  if (cameraEvent == 0 && tofEvent == 0)  // keine Änderung
  {
    return 0;
  }
  else // Sensor Fusion
  {
    // Bei verunden: speichert Millis vom ersten Sensor ab, zweite Sensor warten. 
    //return cameraEvent; // temp
    // return tofEvent;  // temp
    return cameraEvent+tofEvent; //temp
  }
}