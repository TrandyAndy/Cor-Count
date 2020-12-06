//#include "Global.h"

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
        //Daten Sichern
        esp_deep_sleep_start();
        Serial.println("This will never be printed");
    };
    private:
    long unsigned leerlaufZeit=0;
    long unsigned altleerlaufZeit=0;
};
