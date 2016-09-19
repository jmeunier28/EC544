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

// Variables and pins used for controlling the led and the tranmission mode
// Initially its in transmit mode

int ButtonPin = 2; // Get the value from the button
int TransPin = 1; // Control term for the transmission
int state = HIGH;
int reading;
int previous = LOW;
long time = 0;
long debounce = 200; // Debounce time 
int outPin = 13; // connect an Led to show the state its in


void setup() {
  Serial.begin(9600);
  pinMode(ThermistorPIN0, INPUT);
  XBee.begin(9600);
  int id = randIntGen();
  
  pinMode(ButtonPin, INPUT); 
  pinMode(outPin, OUTPUT);
  
}

void loop() {

//switch reading logic with debouncing
  
  reading = digitalRead(ButtonPin);
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;

    time = millis();    
  }

  digitalWrite(outPin, state);

  previous = reading;

// Using the outPin state to toggle between tranmit and non transmit mode

  if ( outPin == HIGH ) {
    if ( TransPin == 1){
      TransPin = 0;
    }
    else
      TransPin = 1;
  }
  


  
  float temp;
  
  /*
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

  float average = (sum/5);*/
  
  if (TransPin == 1) {                                //  Transmit only if the transPin is high
  temp=Thermistor(analogRead(ThermistorPIN0));       // read ADC and  convert it to Celsius
  String mytemp = String(temp);
  String msg = String("A, " + mytemp);
  XBee.print(msg);
  XBee.write("\n");
  Serial.print("Celsius: "); 
  Serial.println(temp, 1);                             // display Celsius
  Serial.println("");                                   
  delay(2000);// Delay a bit... 
  }
  
}
