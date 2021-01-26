#include "CBattery.h"

CBattery::CBattery(byte pPinBattery): mPinBattery(pPinBattery)
{
    Serial.println("Batterie Objekt erzeugt");  // debug
}
void CBattery::init()
{
    pinMode(mPinBattery, INPUT);
}
uint8_t CBattery::getBatteryLevel()
{
    uint16_t analogValue = analogRead(mPinBattery);  // analogValue Rohwert des ADC
    //Serial.printf("Unser Analoger Akkuwert ist: %d\n", analogValue);
    //delay(100);
    // Serial.print("Analogwert ist: "); // debug
    // Serial.println(analogValue);  // debug
    /*
    Umrechnung vom Analogwert in Spannung: 
    y = x  * (3,3 V) / (2^12 - 1) / Faktor_Spannungsteiler
        x: Rohwert ADC 
        y: Spannung in V
    */
    float batteryVoltage = ( analogValue * 3.6 / (float)4095 ) / (float)faktorSpannungsteiler;  // batteryVoltage in Volt // 3.6 ist bei komischen Lolin schöner
    //Serial.printf("Unser Batteriespannung ist: %f\n", batteryVoltage);
    // Serial.print("Gemessene Spannung ist: "); // debug
    // Serial.println(batteryVoltage);  // debug
    /* 
    Umrechnung von Spannung in SOC
    Verwendetes Polynom:
    0 mAh / 3350 mAh:      0 % --> 4,2 V        // 4,2
    2000 mAh / 3350 mAh:  60 % --> 3,5 V
    3350 mAh / 3350 mAh: 100 % --> 2,5 V
    Matlab:   polyfit([4.2, 3.5, 2.5], [0, 60, 100], 1);
                y = - 57.5 * x + 249 
                x: Spannung in V
                y: State of Charge in %
    */
    
    int8_t soc = (int8_t) roundf(-57.5 * batteryVoltage + 249);  // erg in %
    if (soc > 100)
    {
        soc = 100;
    }
    else if (soc < 0)
    {
        soc = 0;
    }
    return (uint8_t) soc;
}