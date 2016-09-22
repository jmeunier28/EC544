
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
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
 return Temp;                     // Return the Temperature
}
double lala(double temp){
  temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
  return temp;
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
  
}
int id = random(1,1000);

void loop() {
  float temp = 0.0;
  int tempArray[5];
  float sum = 0.0;
  float ftemp 0.0;
  //Serial.print("The id is: " + id);
  /*for (int i = 0; i <5; i++){
    temp=(int)Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
    temp = tempArray[i];
    Serial.print(tempArray[i]);
    delay(10);
  }
  for (int i =0; i< 5;i++){
    sum = sum + tempArray[i];
  }*/
  //float average = (sum/5);
  //Serial.write("the average is: " + average);
  temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
  ftemp = lala(temp);
  String mytemp = String(temp);
  String msg = String("D, " + mytemp);
  String msg = String("FD, " + ftemp);
  XBee.print(msg);
  XBee.write("\n");
  Serial.print("Celsius: "); 
  Serial.println(temp, 1);                             // display Celsius
  Serial.println("");                                   
  delay(2000);                                      // Delay a bit... 
}

