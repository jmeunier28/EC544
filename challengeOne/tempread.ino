
#include <SoftwareSerial.h>
#include <math.h>

//#define ThermistorPIN 0                 // Analog Pin 0
int ThermistorPIN0 = A0;
SoftwareSerial XBee(2,3); //RX, TX


float vcc = 3.3;                       // only used for display purposes, if used
                                        // set to the measured Vcc.
float pad = 9850;                       // balance/pad resistor value, set this to
                                        // the measured resistance of your pad resistor
float thermr = 9100;                   // thermistor nominal resistance

float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=pad*((1024.0 / RawADC) - 1); 
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  return Temp;                                      // Return the Temperature
}

int randIntGen(){
  int num;
  num = random(1, 1000);
  return num;
}

void setup() {
  Serial.begin(9600);
  pinMode(ThermistorPIN0, INPUT);
  XBee.begin(9600);
  int id = randIntGen();
}

void loop() {
  float temp;
  float tempArray[5];
  float sum;
  for (int i = 0; i <5; i++){
    temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
    temp = tempArray[i];
    delay(10);
  }
  for (int i =0; i< 5;i++){
    sum += tempArray[i];
  }

  float average = (sum/5);
  //temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
  String mytemp = String(average);
  String msg = String("id, " + mytemp);
  XBee.print(msg);
  XBee.write("\n");
  Serial.print("Celsius: "); 
  Serial.println(temp, 1);                             // display Celsius
  Serial.println("");                                   
  delay(2000);                                      // Delay a bit... 
}

