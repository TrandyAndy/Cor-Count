/*
Allgemeine Funktionen 
Autor: Andy
*/
// Julian: Warum ist die Datei nicht in h und cpp aufgeteilt??????

#pragma once
#include <EEPROM.h>     //Speichern des Zaehlers
#include <Preferences.h>

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

// Fragen von Julian: Wo ist das Wecken vom PIR? Ich müsste zusätzlich noch menschenImRaumMax und energiesparmodus speichern
class CSchlafen{
    public:
    CSchlafen(){
        //int x=1+1;
    };
    CSchlafen(int x){
        
        //Serial.begin(115200);
        //EEPROM.begin(1);
        Serial.println("Ich war hier");
        
        P.begin("nvs",false);
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
        esp_sleep_enable_timer_wakeup(15000000);
        datensichern(menschenImRaum, AdresseMesnchenZaehler);//Daten sichern
        datensichern(menschenImRaumMax, AdresseMesnchenMax);
        datensichern(int(energiesparmodus), Adresseenergiesparmodus);  
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
    Preferences P;
    void datensichern(int Data1, int Adr){  //private damit das niemand zu schnell macht.
        int Daten=Data1; //MAX 16Bit int
        EEPROM.write(Adr,Daten>>8); //High Byte 
        EEPROM.write(Adr+1,Daten&0xFF); //Low Byte
        EEPROM.commit();
        P.putUChar("addr",65);
        //Serial.println("Daten gesichert");
    };
};

class CSignalLicht{
    public:
    CSignalLicht(int LED_PIN){
        init(LED_PIN);
    };
    CSignalLicht(int LED_PIN, int I2C_Adresse){
        init(LED_PIN);
        //Lichtsensor suchen
        //if Wire.beginn...
        //if (Sensor da)
            LDR_verbunden=true;
        //else break??
    };
    void setLicht(bool Zustand)     //Licht An oder Aus
    { 
        if(!Zustand){
            ledcWrite(channel, 0);
        }
        else{
            int duty=LDR_pruefen();
            duty=255*duty/100;
            ledcWrite(channel, duty);
        }

    };
    int LDR_pruefen(){              //Oder Set_LDR Methode
        Serial.println(channel); //Test
        Serial.println("T5");
        if(LDR_verbunden==false)
            return Grundhelligkeit;
        //Sensorabfrage
        //Mitteln?
        return Grundhelligkeit-umgebungsHelligkeit*HelligkeitGain;
    };
    private:
    void init(int LED_PIN){
        channel=CSignalZaehler++;                 //Channel zuweisen + Instanz hochzählen
        Serial.println("T1");
        Serial.println(channel);
        ledcSetup(channel, 5000, aufloesung);     //Channel,Frequenz,Auflösung
        Serial.println("T2");
        ledcAttachPin(LED_PIN,channel);           //Nur im Setup??? Nein Achtung nicht jeder Pin Möglich =>GuruPanik
        Serial.println("T3");
        ledcWrite(channel, 0);                    //Channel,Duty  Erstmal Aus
        Serial.println("T4");
    };
    bool LDR_verbunden=false;
    int umgebungsHelligkeit=1;
    const int aufloesung=13;
    int channel=0;  //0-15
    //int CSignalZaehler=5;
    static int CSignalZaehler;    //public?                  //Instanzen Zählen Nur Deklatration möglich!
};
int CSignalLicht::CSignalZaehler=0;