/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-01-05 21:31:05
 * @Description: Anbindung der Camera an die Hauptsteuerung über 3 GPIO Pins
 */

#include "CCamera.h"

bool flagEventEntry;    // Flag für ISR Personen hineingelaufen
bool flagEventyExit;    // Flag für ISR Personen hinausgelaufen

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;    // für die isr

CCamera::CCamera(byte pPinEventEntry, byte pPinEventExit, byte pPinWakeUp): pinEventEntry(pPinEventEntry), pinEventExit(pPinEventExit), pinWakeUp(pPinWakeUp)
{
    flagEventEntry = false;
    flagEventyExit = false;
    Serial.println("Kamera Objekt erzeugt");
}

void CCamera::init()
{
    pinMode(pinEventEntry, INPUT_PULLDOWN);
    pinMode(pinEventExit, INPUT_PULLDOWN);
    pinMode(pinWakeUp, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(pinEventEntry), isrEventEntry, RISING);
    attachInterrupt(digitalPinToInterrupt(pinEventExit), isrEventExit, RISING);
}
int8_t CCamera::run()
{
    int8_t returnValue = 0;
    if(flagEventEntry)
    {
        flagEventEntry = false;
        Serial.println("Person hineingelaufen");
        returnValue++;
    }
    if(flagEventyExit)
    {
        flagEventyExit = false;
        Serial.println("Person hinausgelaufen");
        returnValue--;
    }
    return returnValue;
}
void CCamera::isrEventEntry()
{
    portENTER_CRITICAL_ISR(&mux);
    flagEventEntry = true;
    portEXIT_CRITICAL_ISR(&mux);
}
void CCamera::isrEventExit()
{
    portENTER_CRITICAL_ISR(&mux);
    flagEventyExit = true;
    portEXIT_CRITICAL_ISR(&mux);
}
void CCamera::wakeUpCamera()
{
    digitalWrite(pinWakeUp, HIGH);
}
void CCamera::setCameraToSleep()
{
    digitalWrite(pinWakeUp, LOW);
}