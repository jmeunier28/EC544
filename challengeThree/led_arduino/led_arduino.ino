#include <SoftwareSerial.h>

int redLED = 11;
int greenLED = 12;
int blueLED = 13;

SoftwareSerial XBee(2,3); // RX, TX

// Setup
void setup() {
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  XBee.begin(9600);
  for (int i=0; i<100; i++) 
    Serial.print("\n"); 
  // Random ID
  Serial.println("Listening...");
}

// MAIN LOOP
void loop() {
  char r;
      if (XBee.available()) {
         r = XBee.read(); 
      }
        if (r == 'Q') {
          Serial.println(r);
          Serial.println();
          Serial.println("Turning LED OFF.");
          digitalWrite(redLED, LOW);
          digitalWrite(greenLED, LOW);
          digitalWrite(blueLED, LOW);
          Serial.println();
        }
       if (r == '1') {
          Serial.println(r);
          Serial.println();
          Serial.println("Turning red LED on.");
          digitalWrite(redLED, HIGH);
          Serial.println();
        }   
        else if (r == '2') {
          Serial.println(r);
          Serial.println();
          Serial.println("Turning green LED on.");
          digitalWrite(greenLED, HIGH);
          Serial.println();  
        }
        else if (r == '3') {
          Serial.println(r);
          Serial.println();
          Serial.println("Turning blued LED on.");
          digitalWrite(blueLED, HIGH);
          Serial.println(); 
        }       
} 
