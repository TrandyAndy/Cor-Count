/*
Allgemeine Funktionen 
Autor: Andy
*/
// Julian: Warum ist die Datei nicht in h und cpp aufgeteilt??????

#pragma once
#include <EEPROM.h>     //Speichern des Zaehlers
#include <Preferences.h>
#include <Adafruit_NeoPixel.h>


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

class CSchlafen{
    public:
    CSchlafen(){
        //int x=1+1;
    };
    CSchlafen(int x){
        
        //Serial.begin(115200);
        //EEPROM.begin(1);
        Serial.println("Ich war hier");
        
        //P.begin("nvs",false);
    };
    void resetSleepTime(){
        altleerlaufZeit=millis();
        leerlaufZeit=0;
    };
    void energiesparen(){
        //Update leerlauf
        leerlaufZeit=millis()-altleerlaufZeit; //if Millis =0 nach Aufwachen
        if (leerlaufZeit<Schalfenszeit)
        return; //Noch nicht so weit
        Serial.println("ESP muede, ESP schlafen");
        esp_sleep_enable_ext0_wakeup(gpio_num_t (WakeupPin),1); //1 = High, 0 = Low
        esp_sleep_enable_timer_wakeup(SolangesollderESPschalfen*1000000);
        datensichern(menschenImRaum, AdresseMesnchenZaehler);//Daten sichern
        datensichern(menschenImRaumMax, AdresseMesnchenMax);
        datensichern(int(energiesparmodus), Adresseenergiesparmodus);  
        myServer.close();       // Websocket Verbindung wird geschlossen.
        esp_deep_sleep_start(); 
        Serial.println("This will never be printed");
    };
    int getData(int Adr) 
    {
        return (EEPROM.read(Adr)<<8)+(EEPROM.read(Adr+1));
    };
    private:
    long unsigned leerlaufZeit=0;
    long unsigned altleerlaufZeit=0;
    //Preferences P;
    public:
    void datensichern(int Data1, int Adr){  //private damit das niemand zu schnell macht.
        int Daten=Data1; //MAX 16Bit int
        EEPROM.write(Adr,Daten>>8); //High Byte 
        EEPROM.write(Adr+1,Daten&0xFF); //Low Byte
        EEPROM.commit();
        //P.putUChar("addr",65);
        //Serial.println("Daten gesichert");
    };
};

class CSignalLicht{
    public:
    CSignalLicht(int pFarbe, int pPin): ring(PIXEL_COUNT, pPin, NEO_GRB + NEO_KHZ800)
    {
        mFarbe=pFarbe;
    };
    void setLicht(bool Zustand)     //Licht An oder Aus
    { 
        if (Zustand==altZustand)
            return;
        int Arr[]={0,0,0};
        Arr[mFarbe]=Grundhelligkeit*Zustand;
        for(int i=0; i<ring.numPixels(); i++) { // For each pixel in strip...
            ring.setPixelColor(i,Arr[0],Arr[1],Arr[2]);         //  Set pixel's color (in RAM)
            ring.show();                          //  Update strip to match
            delay(5);                           //  Pause for a moment
            }
        altZustand=Zustand;
        //Serial.println(ring.getPixelColor(1)); //Debug
    };
    void init(){
        ring.begin(); // Initialize NeoPixel strip object (REQUIRED)
        ring.show();  // Initialize all pixels to 'off'
    };
    private:
    Adafruit_NeoPixel ring; //Wir in der Liste gemacht
    int mFarbe=2;
    int altZustand=-1; //Damit es inital immer schaltet.
    //int CSignalZaehler=5;
    static int CSignalZaehler;    //public?                  //Instanzen Zählen Nur Deklatration möglich!
};
int CSignalLicht::CSignalZaehler=0;