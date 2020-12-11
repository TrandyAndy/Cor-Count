/*
Allgemeine Funktionen 
Autor: Andy
*/
#pragma once
#include <EEPROM.h>     //Speichern des Zaehlers

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


class CSchalfen{
    public:
    CSchalfen(void){};
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
        esp_sleep_enable_timer_wakeup(6000000);
        datensichern(&menschenImRaum);//Daten Sichern
        esp_deep_sleep_start(); 
        Serial.println("This will never be printed");
    };
    int getData(){
        return EEPROM.read(AdresseMesnchenZaehler);
    };
    private:
    long unsigned leerlaufZeit=0;
    long unsigned altleerlaufZeit=0;
    void datensichern(int *Data1){  //private damit das niemand zu schnell macht.
        EEPROM.write(AdresseMesnchenZaehler,*Data1); //Läuft bei 255 Über
        EEPROM.commit();
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