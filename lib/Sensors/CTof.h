/*
 * @Author: JLS666
 * @Email: 
 * @Date: 2021-06-26 12:17:28
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-06-26 12:18:06
 * @Description: TOF program, programmed von Khariyah modified by Julian
 */


#include "Global.h"
#include "VL53L1X.h"

class CTof 
{
private:
    int people_counter = 0;
    int baseline_sensor_1 = 0;
    int baseline_sensor_2 = 0;
    int sensor_readout_1 = 0;
    int sensor_readout_2 = 0;
    int threshold_sensor_1 = 0;
    int threshold_sensor_2 = 0;
    bool trig_sensor_1 = false;
    bool trig_sensor_2 = false;
    VL53L1X sensor;
    int read_sensor_1();
    int read_sensor_2();
public:
    void init();
    int8_t run();   // 0 = nothing, 1 = in -1 = out
    bool checkBrightness();
/*    void run();
    //int getErgebnis();
    int get_Direction(); //0=nix 1=rein -1=raus */
};