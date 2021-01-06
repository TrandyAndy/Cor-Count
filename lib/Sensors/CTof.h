#include "Global.h"

class CTof
{
private:
    void IRAM_ATTR ISR_ToF1();
    void IRAM_ATTR ISR_ToF2();
    int Richtung=0;
public:
    void init();
    void run();
    int getErgebnis();
    int get_Direction();
};
