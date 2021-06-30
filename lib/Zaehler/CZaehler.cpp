#include "CZaehler.h"
#include "Global.h"

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
    // debug message
    Serial.print("Erkannt von Camera:");
    Serial.println(cameraEvent); //Debug
    Serial.print("Erkannt vom TOF:");
    Serial.println(tofEvent); //Debug
    // end of debug message 
    myServer.sendDebugMessage("Sensor hat was erkannt: " + (String)tofEvent);
    myServer.sendDebugMessage("Kamera hat was erkannt: " + (String)cameraEvent);
    //return cameraEvent+tofEvent; //temp
    if (tooBrightForTof)
    {
      return cameraEvent;
    }
    else 
    {
      return tofEvent;
    }
  }
}