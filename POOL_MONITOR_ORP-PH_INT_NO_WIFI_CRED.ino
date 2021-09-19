#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "xxxxxxxxxxxxxxxxxxxxxxxxx";            // You should get Auth Token in the Blynk App.

char ssid[] = "xxxxxxxxxxxx";    // Your WiFi credentials.
char pass[] = "xxxxxxxxxx";  // Set password to "" for open networks.


#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;
const float multiplier = 0.0002*0.60665; //0.0002 times 3.3/(ADC SIGNAL V)  

void setup() {
  
  pinMode(15, OUTPUT);
  digitalWrite(15,LOW);  //Turn off the pump
  Serial.begin(9600);
 


  ads.setGain(GAIN_ONE);      // (for an input range of +/-4.096V) 1 bit = 2mV
  ads.begin();
  Wire.begin();

  int PPrime_time=1;  //time in minutes
  PPrime_time=PPrime_time*60*1000;
  
  digitalWrite(15,HIGH);  //Turn on the pump
  delay (PPrime_time); // Run pump before sampling to wash out sample vessel
  

  float PH=0;
  float ORP=0;
  float PHT=0;
  float ORPT=0;
  int loopcount=0;
  int nSamples=20;


  int sample_time=4;  //time in minutes
  int sample_delay=sample_time*60*1000/nSamples;
  float Sleep_Time = 55;  // Minutes

  
  Serial.print("Num Samples:");
  Serial.println(nSamples);
  Serial.print("Sample Time (min):");
  Serial.println(sample_time);
  Serial.print("Sleep_Time (min):");
  Serial.println(Sleep_Time);
  Sleep_Time=Sleep_Time*60*1E6;  // Sleep time in microseconds
  

  while (loopcount <nSamples)  //Take nSamples over sample_time and avergae them.
  {
        delay(sample_delay);   //Time to pause between readings
       
        PH = getPH();
        ORP = getORP();
          loopcount=loopcount+1;
          Serial.print("PH Single Read:");
          Serial.println(PH);
          Serial.print("LoopCount:");
          Serial.println(loopcount);
          PHT=PHT+PH;
          ORPT=ORPT+ORP;
          
  }  //End of While
  
  digitalWrite(15,LOW);  //Turn off the pump
  
  PH=PHT/loopcount;    //Average the sums
  ORP=ORPT/loopcount;

 
  Serial.print("PH Average:");
  Serial.println(PH);
    Serial.print("ORP Average:");
  Serial.println(ORP);
  
  Blynk.begin(auth, ssid, pass);
   while (Blynk.connect() == false) {
}
  Blynk.run();
  Blynk.virtualWrite(V4, PH);
  Blynk.virtualWrite(V5, ORP);
  delay(1000);
  Blynk.disconnect();
  Serial.print("Gone to Sleep!");
  
  ESP.deepSleep(Sleep_Time,WAKE_RF_DEFAULT);  //Go to deep sleep to save power
  
 
}

float getPH()   //Function to Read the PH from Phidgits Card
{
  float voltage;
  float PH = 0;
  float sum = 0;
  float avg = 0;
  for (int i = 0; i <= 100; i++) {
  
    voltage = ads.readADC_SingleEnded(0);
   delay(10);
 
    sum += voltage;
      
   }  // End of for loop
 
  voltage = multiplier*sum/100;
  PH=3.56*voltage-1.889;  //Voltage to PH calculation
  return (PH);
}

float getORP()   //Function to Read the  ORP from Phidgits Card
{
  float voltage;
  float avg = 0;
  float sum = 0;
  float ORP = 0;
  
  for (int i = 0; i <= 100; i++) {
  
      voltage = ads.readADC_SingleEnded(1);
     delay(10);
     
      sum += voltage;
      
   }  // End of for loop

 
  voltage = multiplier*sum/100;
  ORP=1000*(2.5-voltage)/1.037;   //Voltage to ORP Calculation
  
  return (ORP);
}

void loop() {

}
