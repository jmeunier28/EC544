#include <SoftwareSerial.h>

int ledPin = 13;
SoftwareSerial XBee(2,3); // RX, TX

// Setup
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
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
          digitalWrite(ledPin, LOW);
          Serial.println();
        }
       if (r == '1') {
          Serial.println(r);
          Serial.println();
          Serial.println("Turning LED on.");
          digitalWrite(ledPin, HIGH);
          Serial.println();
        }     
} 
