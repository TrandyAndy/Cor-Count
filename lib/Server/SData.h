/*
 * @Author: Julian Schweizerhof
 * @Email: diegruppetg@gmail.com
 * @Date: 2021-01-04 16:17:54
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-06-04 16:36:04
 * @Description: Stucts für die Kommunikation mit der Webseite
 */

#include <Arduino.h>
struct DataReceive
{
    int personenzahlMax;
    int personenzahlAktuell;
    String dateTime;
    bool energiesparmodus;

    // Wlan Einstellungen
    String ssidAP;
    String passwordAP;
    String ssidSTA;
    String passwordSTA;
};

struct DataSend
{
    int personenzahlMax;
    int personenzahlAktuell;
    unsigned char akkustand;
    bool energiesparmodus;
    bool flagGetTime;

    bool isActiveAP;
    bool isActiveSTA;
};
