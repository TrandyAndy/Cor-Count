#include "CSaveLoad.h"
#include <Preferences.h>

Preferences preferences;

CSaveLoad:: CSaveLoad(/* args */)
{
    
}

CSaveLoad::~ CSaveLoad()
{

}

void CSaveLoad::init()
{
    preferences.begin("PeopleCounter", false);  // Read only = false
}

void CSaveLoad::saveWlanAccessDataSta(String ssid, String password)
{
    preferences.putString("ssidSTA", ssid); 
    preferences.putString("passwordSTA", password);
}
void CSaveLoad::loadWlanAccessDataSta(String & ssid, String & password)
{
    ssid = preferences.getString("ssidSTA", ""); 
    password = preferences.getString("passwordSTA", "");
}
void CSaveLoad::saveWlanAccessDataAp(String ssid, String password)
{
    
    preferences.putString("ssidAP", ssid); 
    preferences.putString("passwordAP", password);
}
void CSaveLoad::loadWlanAccessDataAp(String & ssid, String & password)
{
    ssid = preferences.getString("ssidAP", ""); 
    password = preferences.getString("passwordAP", "");
}
void CSaveLoad::saveMenschenImRaum(int menschenImRaum)
{
    preferences.putInt("menschen", menschenImRaum);
}
void CSaveLoad::saveMenschenImRaumMax(int menschenImRaumMax)
{
    preferences.putInt("menschenMax", menschenImRaumMax);
}
void CSaveLoad::saveEnergiesparmodus(bool energiesparmodus)
{
    preferences.putBool("energie", energiesparmodus);
}
void CSaveLoad::loadMenschenImRaum(int & menschenImRaum)
{
    menschenImRaum = preferences.getInt("menschen", 666);
}
void CSaveLoad::loadMenschenImRaumMax(int & menschenImRaumMax)
{
    menschenImRaumMax = preferences.getInt("menschenMax", 666);
}
void CSaveLoad::loadEnergiesparmodus(bool & energiesparmodus)
{
    energiesparmodus = preferences.getBool("energie", false);
}
void CSaveLoad::saveScannedNetworks(String scannedNetworks[20], byte numberOfScannedNetworks)
{
    preferences.putUChar("number", numberOfScannedNetworks);
    for (byte i = 0; i < numberOfScannedNetworks; i++)
    {
        Serial.println(scannedNetworks[i]);
        String name = "network" + (String) i;
        preferences.putString(name.c_str(), scannedNetworks[i]);
    }
}
void CSaveLoad::loadScannedNetworks(String (& scannedNetworks)[20], byte & numberOfScannedNetworks)
{
    byte number = preferences.getUChar("number", 111);
    numberOfScannedNetworks = number;
    for (byte i = 0; i < number; i++)
    {
        String name = "network" + (String) i;
        scannedNetworks[i] = preferences.getString(name.c_str(), "");
        Serial.println(scannedNetworks[i]);
    }
}