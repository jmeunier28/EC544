#include <SoftwareSerial.h>
#include <math.h>

//#define ThermistorPIN 0                 // Analog Pin 0
int ThermistorPIN0 = A0;
SoftwareSerial XBee(2,3); //RX, TX


float thermr = 9100;                   // thermistor nominal resistance

double Thermistor(int RawADC) {
 double Temp;
 Temp = log(((10240000/RawADC) - thermr));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;              // Convert Kelvin to Celsius
 Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
 return Temp;                     // Return the Temperature
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
  /*for (int i = 0; i <5; i++){
    temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
    temp = tempArray[i];
    delay(10);
  }
  for (int i =0; i< 5;i++){
    sum += tempArray[i];
  }

  float average = (sum/5);*/
  temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
  String mytemp = String(temp);
  String msg = String("A, " + mytemp);
  XBee.print(msg);
  XBee.write("\n");
  Serial.print("Celsius: "); 
  Serial.println(temp, 1);                             // display Celsius
  Serial.println("");                                   
  delay(2000);                                      // Delay a bit... 
}
