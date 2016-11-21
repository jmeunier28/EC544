/* Two ultrasonic sensors for collision detection
Angled on front sides

Checks for object and sends digital high if distance is less than set threshold
(ran out of pwm pins on main arduino)
*/

#define LEFT_INPUT 5
#define RIGHT_INPUT 6
#define LEFT_OUTPUT 8
#define RIGHT_OUTPUT 9


// distance to stop
int stopThreshold = 36;  // ~10cm

// vars
long ultraLeft, inchesLeft, cmLeft;
long ultraRight, inchesRight, cmRight;
int objectDetectedLeft, objectDetectedRight;



void Poll_Ultrasonic() 
{
  // left
  ultraLeft = pulseIn(LEFT_INPUT, HIGH);
  inchesLeft = ultraLeft / 147;
  cmLeft = inchesLeft * 2.54;
  if (cmLeft < stopThreshold)
    objectDetectedLeft = 1;
  else
    objectDetectedLeft = 0;
 
  // right
  ultraRight = pulseIn(RIGHT_INPUT, HIGH);
  inchesRight = ultraRight / 147;
  cmRight = inchesRight * 2.54;
  if (cmRight < stopThreshold)             
    objectDetectedRight = 1;
  else
    objectDetectedRight = 0;  
}

void setup()
{
    // Serial monitor
  Serial.begin(9600);
  // pins
  pinMode(LEFT_INPUT, INPUT);
  pinMode(RIGHT_INPUT, INPUT);
  pinMode(LEFT_OUTPUT, OUTPUT);
  pinMode(RIGHT_OUTPUT, OUTPUT); 
}

void loop()
{
  // check distances, set flags
  Poll_Ultrasonic();

  // set output pins high on detection, print detection messages
  if (objectDetectedLeft) {
    digitalWrite(LEFT_OUTPUT, HIGH); 
    Serial.println("\n  LEFT object detected!\n");
  }    
  else
    digitalWrite(LEFT_OUTPUT, LOW);

  if (objectDetectedRight) {
    digitalWrite(RIGHT_OUTPUT, HIGH);
    Serial.println("\n  RIGHT object detected!\n");
  }
  else
    digitalWrite(RIGHT_OUTPUT, LOW);

  // print distances
  Serial.print("Left ultrasonic distance: ");
  Serial.print(cmLeft);
  Serial.println("cm");
  Serial.print("Right ultrasonic distance: ");
  Serial.print(cmRight);
  Serial.println("cm");   
}

