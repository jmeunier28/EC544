//Digital pin 7 for reading in the pulse width from the MaxSonar device.

//This variable is a constant because the pin will not change throughout execution of this code.

const int pwPin = 7;
const int pwPin2 = 8;



//variables needed to store values

long pulse, inches, cm;
long pulse2;
long avg;
const int stopPin = 1;

void setup()

{

  //This opens up a serial connection to shoot the results back to the PC console

  Serial.begin(9600);

}



void loop()

{

  pinMode(pwPin, INPUT);
  pinMode(pwPin2, INPUT);
  pinMode(stopPin, OUTPUT);
  digitalWrite(stopPin, LOW);

  //Pulse Width representation with a scale factor of 147 uS per Inch.

  pulse = pulseIn(pwPin, HIGH);
  pulse2 = pulseIn(pwPin2, HIGH);

  //147uS per inch
  /*for (int i = 0; i<5; i++){
    
    avg = (pulse + pulse2)/2;
    inches = avg / 147;
    cm = inches * 2.54;
    delay(50);
    
  }*/
  
  //change inches to centimetres
  
  avg = (pulse + pulse2)/2;
  inches = avg / 147;
  cm = inches * 2.54;
    
    if (cm <= 20){
      digitalWrite(stopPin, HIGH);
      Serial.println("STOP CAR!!");
    }

  Serial.print(cm);

  Serial.print("cm");

  Serial.println();

 delay(50);
 //delay(500);

}