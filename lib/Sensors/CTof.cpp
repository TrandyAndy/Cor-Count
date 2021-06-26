/*
 * @Author: JLS666
 * @Email: -
 * @Date: 2021-06-26 11:16:12
 * @Last Modified by: JLS666
 * @Last Modified time: 2021-06-26 12:03:22
 * @Description: TOF program, programmed von Khariyah modified by Julian
 */


#include "CTof.h"

void CTof::init()
{
    Wire.begin(SDA,SCL);
    Wire.setClock(400000); // use 400 kHz I2C
    /*  // Julian: is not needed
    // Initialize display, if fail, stuck inside for loop
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    */
    // Hold on initializing sensor (500ms)
    // If sensor fails to initialize, stuck inside while loop
    sensor.setTimeout(500);
    if (!sensor.init())
    {
        Serial.println("Failed to detect and initialize sensor!");
        // while (1);   // Julian: is not needed
        return; // Sonst teil irgendjemand durch 0
    }
    
    // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
    // You can change these settings to adjust the performance of the sensor, but
    // the minimum timing budget is 20 ms for short distance mode and 33 ms for
    // medium and long distance modes. See the VL53L1X datasheet for more
    // information on range and timing limits.
    
    sensor.setDistanceMode(VL53L1X::Medium);
    sensor.setMeasurementTimingBudget(33000);
    // <-- Disable continuous measurement mode -->
    // Start continuous readings at a rate of one measurement every 50 ms (the
    // inter-measurement period). This period should be at least as long as the
    // timing budget.
    //sensor.startContinuous(100);  // Julian: Why is that commented out?

    // Wait 1s before starting to get sensor baseline 
    delay(1000);    // Julian: Is this necessary?
    // Sensor 1 Baseline
    for (int i = 0; i < 100; i++)
    {
        sensor_readout_1 = read_sensor_1();
        baseline_sensor_1 = (baseline_sensor_1 + sensor_readout_1) / 2;   // Julian: Should this be arithmetic mean?
        // Arithmetic mean:
        baseline_sensor_1 = baseline_sensor_1 + sensor_readout_1;           
    }  
    baseline_sensor_1 = baseline_sensor_1 / 100;

    Serial.print("---Sensor 1 Baseline--->");
    Serial.println(baseline_sensor_1);
    delay(200);     // Julian: Is this necessary?

    // Sensor 2 Baseline
    for (int i = 0; i < 100; i++)
    {
        sensor_readout_2 = read_sensor_2();
        baseline_sensor_2 = (baseline_sensor_2 + sensor_readout_2)/2;   // Julian: Should this be arithmetic mean?
        // Arithmetic mean:
        baseline_sensor_2 = baseline_sensor_2 + sensor_readout_2;  
    }
    baseline_sensor_2 = baseline_sensor_2 /  100;
    Serial.print("---Sensor 2 Baseline--->");
    Serial.println(baseline_sensor_2);
    delay(200);     // Julian: Is this necessary?

    threshold_sensor_1 = 0.6 * baseline_sensor_1;
    threshold_sensor_2 = 0.6 * baseline_sensor_2;
    Serial.print("---Sensor 1 Threshold--->");
    Serial.println(threshold_sensor_1);
    Serial.print("---Sensor 2 Threshold--->");
    Serial.println(threshold_sensor_2);
}
int8_t CTof::run()
{
    sensor.readSingle(true);
  
    if ( (String) VL53L1X::rangeStatusToString(sensor.ranging_data.range_status) == (String) "wrap target fail" || 
        (String) VL53L1X::rangeStatusToString(sensor.ranging_data.range_status) == (String) "out of bounds fail")
    {
        /*  // Julian: is not needed
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(15,5);
        display.print("Detection");
        display.setCursor(15,30);
        display.print("FAIL");
        display.setTextSize(2);
        display.display();
        */
        Serial.println("Measurement Failed"); 
    }
    else
    {
        /* // Julian: is not needed
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(20,5);
        display.println("Distance:");
        display.setCursor(20,30);
        display.print(sensor.ranging_data.range_mm);
        display.print("mm");
        display.setTextSize(2);
        display.display();
        */
        Serial.print("Distance: ");
        Serial.print(sensor.ranging_data.range_mm);
        Serial.println("mm");

        sensor_readout_1 = read_sensor_1();
        sensor_readout_2 = read_sensor_2();

        // Either sensor 1 or 2 has dropped below threshold! MUST INVESTIGATE DIS.
        if (sensor_readout_1 < threshold_sensor_1 || sensor_readout_2 < threshold_sensor_2)
        {
            // Check whether sensor 1 or 2 is triggered first, and save the state of the one that is triggered first
            if (sensor_readout_1 < threshold_sensor_1 && trig_sensor_1 == false && trig_sensor_2 == false)
            {
                trig_sensor_1 = true;
            }
            else if (sensor_readout_2 < threshold_sensor_2 && trig_sensor_1 == false && trig_sensor_2 == false)
            {
                trig_sensor_2 = true;
            }

            // Since one of the sensors have been triggered, now we just wait for the other sensor to trigger
            // We set in the end for both triggers to be true, so we can reset the cycle in the while loop below
            else if (trig_sensor_1 == true && sensor_readout_2 < threshold_sensor_2 && trig_sensor_2 == false)
            {
                people_counter++;
                trig_sensor_2 = true;
                Serial.print("People going in! # people inside -->");
                Serial.println(people_counter);
                return 1; // in
            }
            else if (trig_sensor_2 == true && sensor_readout_1 < threshold_sensor_1 && trig_sensor_1 == false)
            {
                people_counter--;
                trig_sensor_1 = true;
                Serial.print("People going out! # people inside -->");
                // Prevents counter from going below zero
                if (people_counter < 0)
                {
                    people_counter = 0;
                }
                Serial.println(people_counter);
                return -1; // out
            }
        }
        
    //    Serial.printf("%d, %d, %d \n",sensor_readout_1, sensor_readout_2, people_counter);

        // Resets trigger when both engaged & both sensors above threshold value
        if (trig_sensor_1 == true && trig_sensor_2 == true)  // Julian: Replaced while with if
        {
            sensor_readout_1 = read_sensor_1();
            sensor_readout_2 = read_sensor_2();

            // Waits for both sensor readouts to rise above threshold
            // Prevents accidental counting when somebody is standing / stationary underneath sensor
            if (sensor_readout_1 > threshold_sensor_1 && sensor_readout_2 > threshold_sensor_2)
            {
                trig_sensor_1 = false;
                trig_sensor_2 = false;
            }
        } // if (formerly while)
    } 
    return 0;   //  nothing
}
int CTof::read_sensor_1()
{
  sensor.setROISize(6, 6);
  sensor.setROICenter(231);
  return sensor.readSingle(true);
} // Reads Sensor 1 and returns an int

int CTof::read_sensor_2()
{
  sensor.setROISize(6, 6);
  sensor.setROICenter(167);
  return sensor.readSingle(true);
} // Reads Sensor 2 and returns an int