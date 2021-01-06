#include "CTof.h"

bool ISR_Flag1=0;
bool ISR_Flag2=0;

void CTof::ISR_ToF1()
{
    if (currRange1 < prevRange1 - Hysteresis)                                                 // Person/Objekt wurde erkannt
  {
    //Serial.println("currRange1 < prevRange1 - Hysteresis");
    prevRange1 = currRange1;
    Signal1 = 1;
  }
  else if (prevRange1 - Hysteresis <= currRange1 && currRange1 <= prevRange1 + Hysteresis)  // Person/Objekt befindet sich noch im Feld
  {
    //Serial.println("prevRange1 - Hysteresis <= currRange1 && currRange1 <= prevRange1 + Hysteresis");
    //get_Direction();
  }
  else if (currRange1 > prevRange1 + Hysteresis)                                            // Person/Objekt hat das Feld verlassen
  {
    //Serial.println(currRange1);
    //Serial.println(prevRange1);
    //Serial.println("currRange1 > prevRange1 + Hysteresis");
    prevRange1 = currRange1;
    Signal1 = 0;
  }
  else
  {
    //Serial.print("currRange1 :");
    //Serial.print(currRange1);
    //Serial.print(" prevRange1: ");
    //Serial.println(prevRange1);
  }
  //Serial.print("Signal1: ");
  //Serial.println(Signal1);
  
}

void CTof::ISR_ToF2() 
{
  
  if (currRange2 < prevRange2 - Hysteresis)                                                 // Person/Objekt wurde erkannt
  {
    //Serial.println("currRange2 < prevRange2 - Hysteresis");
    prevRange2 = currRange2;
    Signal2 = 2;
  }
  else if (prevRange2 - Hysteresis <= currRange2 && currRange2 <= prevRange2 - Hysteresis)  // Person/Objekt befindet sich noch im Feld
  {
    //Serial.println("prevRange2 - Hysteresis <= currRange2 && currRange2 <= prevRange2 - Hysteresis");
    //get_Direction();
  }
  else if (currRange2 > prevRange2 + Hysteresis)                                            // Person/Objekt hat das Feld verlassen
  {
    //Serial.println("currRange2 > prevRange2 + Hysteresis");
    prevRange2 = currRange2;
    Signal2 = 0;                                                                           // eventuell abfrage ob Signal2 kleiner null ?  Ou of range drinne lassen
  }
  else
  {
    //Serial.print("currRange2 :");
    //Serial.print(currRange2);
    //Serial.print(" prevRange2: ");
    //Serial.println(prevRange2);
  }
  //Serial.print("Signal2: ");
  //Serial.println(Signal2);

}

void CTof::ISR1()
{
  ISR_Flag1=true;
}
void CTof::ISR2()
{
  ISR_Flag2=true;
}

void CTof::init()
{
    pinMode(SHT_LOX1, OUTPUT);
    pinMode(SHT_LOX2, OUTPUT);

    Serial.println("Shutdown pins inited...");

    digitalWrite(SHT_LOX1, LOW);
    digitalWrite(SHT_LOX2, LOW);

    Serial.println("Both in reset mode...(pins are low)");
    
    
    Serial.println("Starting...");
    setID();
    
    attachInterrupt(TOF1_PIN, ISR1, RISING);
    attachInterrupt(TOF2_PIN, ISR2, RISING);
}

void CTof::run()
{
    read_dual_sensors();
    Richtung=get_Direction();

    //ISR Flag abfragen
    if(ISR_Flag1)
    {
      ISR_ToF1();
      ISR_Flag1=false;
    }
    if(ISR_Flag2)
    {
      ISR_ToF2();
      ISR_Flag2=false;
    }
}

int CTof::get_Direction()         //prevNumbers
{
  direction_number = -1;
  sum = Signal1 + Signal2;

  if(sum != prevSum || (prevSum == 0 && sum == 0)) 
  {
    numbers[numbersCounter] = sum;
    prevSum = sum;
    
    if (numbersCounter >= 4)
    {
      prevNumbers = numbers[numbersCounter];
      numbersCounter = 0;
    }
    else
    {
      numbersCounter++;
    }
    
  }

  if((numbers[0] == 0 && numbers[1] == 1 && numbers[2] == 3 && numbers[3] == 2 && numbers[4] == 0) || 
  ((numbers[0] == 0 && numbers[1] == 0 && numbers[2] == 1 && numbers[3] == 3 && numbers[4] == 2) /*&& prevNumbers == 0*/) || 
  (numbers[0] == 2 && numbers[1] == 0 && numbers[2] == 0 && numbers[3] == 1 && numbers[4] == 3) || 
  (numbers[0] == 3 && numbers[1] == 2 && numbers[2] == 0 && numbers[3] == 0 && numbers[4] == 1) || 
  (numbers[0] == 1 && numbers[1] == 3 && numbers[2] == 2 && numbers[3] == 0 && numbers[4] == 0))
  {
    //Serial.println("------------------------------------------ Raus");
    direction_number = 1;
    setNumbers();
  }
  else if((numbers[0] == 0 && numbers[1] == 2 && numbers[2] == 3 && numbers[3] == 1 && numbers[4] == 0) || 
  (numbers[0] == 0 && numbers[1] == 0 && numbers[2] == 2 && numbers[3] == 3 && numbers[4] == 1) || 
  (numbers[0] == 1 && numbers[1] == 0 && numbers[2] == 0 && numbers[3] == 2 && numbers[4] == 3) || 
  (numbers[0] == 3 && numbers[1] == 1 && numbers[2] == 0 && numbers[3] == 0 && numbers[4] == 2) || 
  (numbers[0] == 2 && numbers[1] == 3 && numbers[2] == 1 && numbers[3] == 0 && numbers[4] == 0))
  {
    //Serial.println("------------------------------------------ Rein");
    direction_number = 2;
    setNumbers();
  }
  else if((numbers[0] == 0 && numbers[1] == 1 && numbers[2] == 3 && numbers[3] == 1 && numbers[4] == 0) || 
  (numbers[0] == 0 && numbers[1] == 0 && numbers[2] == 1 && numbers[3] == 3 && numbers[4] == 1) || 
  (numbers[0] == 1 && numbers[1] == 0 && numbers[2] == 0 && numbers[3] == 1 && numbers[4] == 3) || 
  (numbers[0] == 3 && numbers[1] == 1 && numbers[2] == 0 && numbers[3] == 0 && numbers[4] == 1) || 
  (numbers[0] == 1 && numbers[1] == 3 && numbers[2] == 1 && numbers[3] == 0 && numbers[4] == 0))
  {
    //Serial.println("Raus und doch wieder rein");
    direction_number = 3;
    setNumbers();
  }
  else if((numbers[0] == 0 && numbers[1] == 1 && numbers[2] == 3 && numbers[3] == 2 && numbers[4] == 3) || 
  (numbers[0] == 1 && numbers[1] == 0 && numbers[2] == 0 && numbers[3] == 0 && numbers[4] == 1) || 
  (numbers[0] == 3 && numbers[1] == 2 && numbers[2] == 3 && numbers[3] == 1 && numbers[4] == 1) || 
  ((numbers[0] == 0 && numbers[1] == 0 && numbers[2] == 1 && numbers[3] == 3 && numbers[4] == 2) && prevNumbers == 1) || 
  (numbers[0] == 3 && numbers[1] == 3 && numbers[2] == 1 && numbers[3] == 0 && numbers[4] == 0) ||
  (numbers[0] == 1 && numbers[1] == 3 && numbers[2] == 2 && numbers[3] == 2 && numbers[4] == 3) || 
  (numbers[0] == 1 && numbers[1] == 0 && numbers[2] == 0 && numbers[3] == 1 && numbers[4] == 3) || 
  (numbers[0] == 3 && numbers[1] == 2 && numbers[2] == 3 && numbers[3] == 1 && numbers[4] == 0) || 
  (numbers[0] == 0 && numbers[1] == 1 && numbers[2] == 1 && numbers[3] == 3 && numbers[4] == 2) || 
  (numbers[0] == 3 && numbers[1] == 1 && numbers[2] == 0 && numbers[3] == 0 && numbers[4] == 0))
  {
    //Serial.println("Rein und doch wieder raus");
    direction_number = 4;
    setNumbers();
  }

  //Serial.println(numbers[]);

  return direction_number;
}

void CTof::setID() 
{
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println("Failed to boot first VL53L0X");
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println("Failed to boot second VL53L0X");
    while(1);
  }
}

void CTof::read_dual_sensors() 
{
  
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  // print sensor one reading
  //Serial.print(F("1: "));
  if(measure1.RangeStatus != 4) {               // if not out of range
    currRange1 = measure1.RangeMilliMeter;
    //Serial.print(currRange1);
  } else {
    //Serial.println(F("1: Out of range"));
    currRange1 = 1800;
  }
  
  //Serial.print(F(" "));

  // print sensor two reading
  //Serial.print(F("2: "));
  if(measure2.RangeStatus != 4) {
    //Serial.print(measure2.RangeMilliMeter);
    currRange2 = measure2.RangeMilliMeter;
  } else {
    //Serial.println(F("2: Out of range"));
    currRange2 = 1800;
  }

  if (firstMeasurement)
  {
    prevRange1 = currRange1;
    prevRange2 = currRange2;
    firstMeasurement = false;
  }
  
  //Serial.println();
}

void CTof::setNumbers()
{
  for(int i = 0; i < sizeof(numbers)/sizeof(numbers[0]); i++)
  {
    numbers[i] = 0;
  }
  /*
  Serial.print("  size of numbers "); 
  Serial.print(sizeof(numbers)); 
  Serial.print("sizeof(numbers)/sizeof(numbers[0])  "); 
  Serial.println(sizeof(numbers)/sizeof(numbers[0])); */
}

