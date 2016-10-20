// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

// This #include statement was automatically added by the Particle IDE.
#include "ThingSpeak/ThingSpeak.h"


#include <math.h>

TCPClient client;
/*
VERY ACCURATE TEMPERATURE READINGS
----------------------------------
Better voltage reading:
  Used A4 pin (less jumpy)
  Got two measurements each pull, discarded the first
Better R_therm reading:
  Measured V_source accurately
  measured R1 accurately
Better temperature reading:
  Used accurate temperature sensor to get 3 data points
  Solved resulting system of equations for Steinhart-Hart coefficients
  Base conversion (needed base-2 instead of base-10)
  (temp slightly rising at start due to current self-heating unavoidable with thermistor)
*/

// Global variables
int AnalogPin = A4; // has much less jumpy readings than A0 (maybe A0 slightly damage from overuse)
float V_source = 4.90; // measured
//String randID = "";
//int handshake = 0;
unsigned int myChannelNumber = 172815;
const char * myWriteAPIKey = "KD2E4ZEG5996CJNY";
const char * name = "Dev1";

int led = D7;
int trans = 0;
int t = 0;
double temp_F;
double T1;
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Calculate thermistor voltage
float Voltage(float RawADC) {
  float meter_calibration = 0; // don't seem to need, matches my multimeter exactly
  float V_therm = (V_source * RawADC) / 4100.0;
  V_therm += meter_calibration;
  return V_therm;
}
// Calculate thermistor resistance
// sometimes a bit off what voltage divider eq says it should be (probably due to input impedance of ADC)
float Thermistor(float V_therm) {
  float resistance_calibration = 25; // possibly small breadboard/wires resistance
  float R_known = 560 + resistance_calibration; // measured (1% tol resistor)
  float R_therm =  ((R_known * V_therm) / (V_source - V_therm));
  return R_therm;
}
// Base conversion
float log2(float num) {
  return (log(num) / log(2));
}
// Calculate temperature
float Temperature(float R_therm) {
  float A = -0.000857348;
  float B = 0.000275445;
  float C = 0.000000234994;
  float temp_K = 1 / (A + B*log2(R_therm) + C*pow(log2(R_therm),3));
  return temp_K;
}
// Convert to Celsius or Fahrenheit
float ConvertTemp(float temp_K, char T) {
  if (T == 'C')  
    return (temp_K - 273.15);
  else if (T == 'F') 
    return (temp_K * 1.8 - 459.67);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

// Setup
void setup() {
 
 pinMode(led, OUTPUT);
 Particle.function("transmission", ToggleTrans);
 //we can access the value of the Temp from the cloud
  Particle.variable("tempFer", temp_F);
 digitalWrite(led, LOW);
// Particle.publish("PowerON","Dev1",60,PRIVATE);
 
}

// MAIN LOOP
void loop()
{
    
    if (trans == 1){
    float RawADC = analogRead(AnalogPin);
    float V_therm = Voltage(RawADC);
    float R_therm = Thermistor(V_therm);
    float temp_K = Temperature(R_therm);
    float temp_C = ConvertTemp(temp_K, 'C');
   // float temp_F = ConvertTemp(temp_K, 'F');
    temp_F = ConvertTemp(temp_K, 'F');
    //Spark.publish("VOLTAGE", String(V_therm));
    //Spark.publish("Raw_ADC", String(RawADC));
    //Spark.publish("R_Therm", String(R_therm));
    //Spark.publish("Kelvin", String(temp_K) );
    //Spark.publish("Temperature (°C)", String(temp_C));  // return degrees C
     T1 = temp_F;
   Spark.publish("Temperature (°F)", String(temp_F));   // return degrees F
 //thermister_temp(analogRead(4));
  //t = t + 1;
 // T1 = temp_F;
 // T1 = String(temp_F);
  //string(t);
  //ThingSpeak.setField(1,t);
  //ThingSpeak.setField(2,name);
 // ThingSpeak.setField(2,temp_F);
  
//  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  
  Particle.publish("temperature",String(temp_F),PRIVATE);
    }
    else{
        delay(500);
    }
 delay(2000);
}

int ToggleTrans(String command) {
    

    if (command=="on") {
        digitalWrite(led,HIGH);
        trans = 1;
        //digitalWrite(led2,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(led,LOW);
        trans = 0;
        //digitalWrite(led2,LOW);
        return 0;
    }
    else {
        return -1;
    }
}

