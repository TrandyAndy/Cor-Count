#include "Global.h"
#include "Adafruit_VL53L0X.h"

class CTof
{
private:
    static void IRAM_ATTR ISR1();
    static void IRAM_ATTR ISR2();
    bool Error1=0, Error2=0;
    void ISR_ToF1();
    void ISR_ToF2();
    void setID();
    void read_dual_sensors();
    void setNumbers();
    // objects for the vl53l0x
    Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
    Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

    // this holds the measurement
    VL53L0X_RangingMeasurementData_t measure1;
    VL53L0X_RangingMeasurementData_t measure2;
    int Richtung=0;
    int numbers[5] = {};
    int direction_number = 0, Signal1 = 0, Signal2 = 0, sum = 0, prevSum = 0, prevNumbers = 100, numbersCounter = 0, Hysteresis = 200;         // Signal1 counts in one step --> 0, 1; Signal2 counts in two step --> 0, 2 
    int prevRange1, currRange1, prevRange2, currRange2;
    int people = 0, counterTriggered = 0;
    bool firstMeasurement = true;

//Measure without Delay
unsigned long previousMillis = 0;
const long interval = 50;  
public:
    void init();
    void run();
    //int getErgebnis();
    int get_Direction(); //0=nix 1=rein -1=raus
};
