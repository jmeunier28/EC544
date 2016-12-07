#include <String.h>
#include <stdlib.h>

int sp = 90;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  char s;
  char buff[7];
  if (Serial.available() > 0){
    s = char(Serial.read());
  }
   //go right
    if (s == '>'){
      int wheel = 800;
      String data = String(wheel);
      data.toCharArray(buff,7);
      Serial.println(data);
      //Serial.write("\n");
    }
    //go left
    if (s == '<'){
      int wheel = 25;
      String data = String(wheel);
      data.toCharArray(buff,7);
      Serial.println(data);
      //Serial.write("\n");
    }
    //Toggle
     if (s == 'T'){
      char y = 'm';
      String data = String(sp);
      data.toCharArray(buff,7);
      Serial.println(y);
      //Serial.write("\n");
    }
    //go faster
     if (s == 'U'){
      sp += 10;
      String data = String(sp);
      data.toCharArray(buff,7);
      Serial.println(data);
      //Serial.write("\n");
    }
    //go slower
     if (s == 'D'){
      sp -= 10;
      String data = String(sp);
      data.toCharArray(buff,7);
      Serial.println(data);
      //Serial.write("\n");
    }
    //stop
     if (s == 'S'){
      sp = 90;
      String data = String(sp);
      data.toCharArray(buff,7);
      Serial.println(data);
      //Serial.write("\n");
    }
  
}