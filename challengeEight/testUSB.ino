int binNum = 0; //initialize to zero

void setup(){
Serial.begin(9600);
}

void loop(){
if (Serial.available()) 
{
  binNum = Serial.read();
  Serial.println("Bin Number is: ");
  Serial.println(binNum);
}
delay(500);
}