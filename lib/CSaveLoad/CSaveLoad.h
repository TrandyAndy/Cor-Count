#pragma once
#include <Arduino.h>

class CSaveLoad
{
private:
    /* data */
public:
     CSaveLoad(/* args */);
    ~ CSaveLoad();
    void init();
    void saveWlanAccessDataSta(String ssid, String password);
    void loadWlanAccessDataSta(String & ssid, String & password);
    void saveWlanAccessDataAp(String ssid, String password);
    void loadWlanAccessDataAp(String & ssid, String & password);
    void saveMenschenImRaum(int menschenImRaum);
    void saveMenschenImRaumMax(int menschenImRaumMax);
    void saveEnergiesparmodus(bool energiesparmodus);
    void loadMenschenImRaum(int & menschenImRaum);
    void loadMenschenImRaumMax(int & menschenImRaumMax);
    void loadEnergiesparmodus(bool & energiesparmodus);
    void saveScannedNetworks(String * scannedNetworks, byte numberOfScannedNetworks);
    void loadScannedNetworks(String (& scannedNetworks)[20], byte & numberOfScannedNetworks);
};



