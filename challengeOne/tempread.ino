
#include <SoftwareSerial.h>
#include <math.h>

//#define ThermistorPIN 0                 // Analog Pin 0
int ThermistorPIN0 = A0;
SoftwareSerial XBee(2,3); //RX, TX


float vcc = 4.91;                       // only used for display purposes, if used
                                        // set to the measured Vcc.
float pad = 9850;                       // balance/pad resistor value, set this to
                                        // the measured resistance of your pad resistor
float thermr = 10000;                   // thermistor nominal resistance

float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=pad*((1024.0 / RawADC) - 1); 
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  return Temp;                                      // Return the Temperature
}

void setup() {
  Serial.begin(9600);
  pinMode(ThermistorPIN0, INPUT);
  XBee.begin(9600);
}

void loop() {
  float temp;
  temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
  String mytemp = String(temp);
  String msg = String("A, " + mytemp);  
  XBee.print(msg);
  XBee.write("\n");
  Serial.print("Celsius: "); 
  Serial.println(temp, 1);                             // display Celsius
  //temp = (temp * 9.0)/ 5.0 + 32.0;                  // converts to  Fahrenheit
  //Serial.print(", Fahrenheit: "); 
  //Serial.print(temp,1);                             // display  Fahrenheit
  Serial.println("");                                   
  delay(10000);                                      // Delay a bit... 
}

