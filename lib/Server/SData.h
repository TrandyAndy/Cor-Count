#include <Arduino.h>
struct DataReceive
{
    int personenzahlMax;
    int personenzahlAktuell;
    String dateTime;
    bool energiesparmodus;
};

struct DataSend
{
    int personenzahlMax;
    int personenzahlAktuell;
    unsigned char akkustand;
    bool energiesparmodus;
    bool flagGetTime;
};