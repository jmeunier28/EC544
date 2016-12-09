// Libraries
#include <SoftwareSerial.h>
#include <Servo.h>

// Servo pins
#define ESC_PIN 5
#define WHEEL_PIN 6

// Lidar pins
#define L_LIDAR_TRIG 13
#define L_LIDAR_MON 10
#define L_LIDAR_ENABLE 4
#define R_LIDAR_TRIG 8
#define R_LIDAR_MON 9
#define R_LIDAR_ENABLE 12

// Ultrasonic pins
#define ULTRA_LEFT 7
#define ULTRA_RIGHT 11

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SET THIS
const double minHallWidthInFt = 0;  // 5


// Remote
SoftwareSerial XBee(2,3); // RX, TX
int Start = 0;

// Servo global variables
Servo wheels;
Servo esc;

// Lidar readings
unsigned long lidar_L, lidar_R;
double leftDistance, rightDistance;


// Ultrasonic global variables
int stop = 0;
int objectDetected = 0;
int leftOrRight;  // L=-1 C=0 R=1 Both=2

// PID global variables
unsigned long lastTime;
double input, output, setpoint;
double error, errSum, lastInput;
double kp, ki, kd;
int sampleTime = 10; // 20ms    (check lidar maximum sample time)

// Movement
int minHallWidth;
int sign;
int Angle;
int Speed;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRINT    PRINT    PRINT    PRINT    PRINT    PRINT    PRINT    PRINT    PRINT    PRINT    PRINT    PRINT  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Print() {
  // Lidars
  Serial.print("Left: ");
  Serial.print(leftDistance);
  Serial.print("cm       ");
  Serial.print("Right: ");
  Serial.print(rightDistance);
  Serial.println("cm");
  // PID
  Serial.print("Setpoint: ");
  Serial.println(setpoint);
  Serial.print("input: ");
  Serial.println(input);
  Serial.print("lastInput: ");
  Serial.println(lastInput);
  Serial.print("error: ");
  Serial.println(error);
  Serial.print("errSum: ");
  Serial.println(errSum);
  Serial.print("output: ");
  Serial.println(output);
  Serial.print("sign: ");
  Serial.println(sign);
  // Set values
  Serial.print("Speed: ");
  Serial.println(Speed);
  Serial.print("Angle: ");
  Serial.println(Angle);
  if (Angle < 90)
    Serial.println("RIGHT turn");
  else 
    Serial.println("LEFT turn");
  Serial.println("\n");
  
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  AUXILIARY FUNCTIONS    AUXILIARY FUNCTIONS    AUXILIARY FUNCTIONS    AUXILIARY FUNCTIONS  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetHall()
{
  if (minHallWidthInFt != 0)
   minHallWidth = int(minHallWidthInFt / 0.032808);
  else if (minHallWidthInFt == 0){
    Serial.println("Looking at lidar distances to determine hall width.");
    PollLidars();
    Serial.print("Left: ");
    Serial.print(leftDistance);
    Serial.print("cm       ");
    Serial.print("Right: ");
    Serial.print(rightDistance);
    Serial.println("cm");
    Serial.println((leftDistance + rightDistance + 5) / 2);
    
    minHallWidth = (leftDistance + rightDistance + 5) / 2;
  }
  else 
    Serial.println("\nDEBUG\n");

    
  //Serial.println  
}


void signMultiplier()
{
  if (setpoint != (minHallWidth / 2)) // two walls
    sign = 1; // could be either  
  else {
    char m = minSide();
    if (m == 'L')
      sign = -1;
    else
      sign = 1;
  }
}

char minSide() 
{
  if (leftDistance < rightDistance)
    return 'L';
   else
    return 'R';
}

int farWalls() 
{
  if ((leftDistance > 200) && (rightDistance > 200))
    return 3;
  else if ((leftDistance > 150) && (rightDistance > 150))
    return 2;
  else if ((leftDistance > 100) && (rightDistance > 100))
    return 1;
  else 
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SETPOINT/INPUT/SPEED  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AdjustSetpoint() 
{
  // two walls (center)
  if ((leftDistance < 150) && (rightDistance < 150)) 
    setpoint = (leftDistance+rightDistance)/2; // middle distance
  // one wall (follow wall)
  else
    setpoint = minHallWidth / 2; // cm
}

void AdjustInput()
{
  if (setpoint == (minHallWidth / 2)) {
    // set input to shorter side
    char s = minSide();
    if (s == 'L')
      input = leftDistance;
    else
      input = rightDistance;
  }
  else input = rightDistance; // could be either

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetTunings(double Kp, double Ki, double Kd) 
{
   double sampleTimeInSec = ((double)sampleTime)/1000;
   kp = Kp;
   ki = Ki * sampleTimeInSec;
   kd = Kd / sampleTimeInSec;
}

void SetsampleTime(int NewsampleTime) 
{
   if (NewsampleTime > 0)
   {
      double ratio  = (double)NewsampleTime / (double)sampleTime;
      ki *= ratio;
      kd /= ratio;
      sampleTime = (unsigned long)NewsampleTime;
   }
}

void Compute() 
{
   unsigned long now = millis();
   int timeChange = (now - lastTime);
   if(timeChange>=sampleTime)
   {
      /*Compute all the working error variables*/
      error = setpoint - input;

      /*
      Serial.print("debug: setpoint: ");
      Serial.println(setpoint);
      Serial.print("debug: input: ");
      Serial.println(input);
      Serial.print("debug: error: ");
      Serial.println(error);
      */
      
      errSum += error;
      double dInput = (input - lastInput);
 
      /*Compute PID output*/
      output = kp * error + ki * errSum - kd * dInput;
 
      /*Remember some variables for next time*/
      lastInput = input;
      lastTime = now;
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE   MOVE  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Move() {
  double limitedOutput;
  signMultiplier();

  if (abs(output) >= 85)
    limitedOutput = 85 * ((output > 0) - (output < 0));
  else limitedOutput = output;     

  Angle = 90 + sign*limitedOutput;
  wheels.write(Angle);

  // Forward or Reverse
  if (!objectDetected) 
    Forward();
  else
    StopCar(); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Forward() 
{

  // change this a bit later

   if ((Angle > 70) && (Angle < 110) && (leftDistance > 40) && (rightDistance > 40))
    Speed = 73;
   else
    Speed = 78;
  esc.write(Speed);
}

void Reverse() 
{
  Serial.println("   REVERSING!");
  Speed = 110;
  // left object
  if (leftOrRight == -1) {
    esc.write(Speed);
    wheels.write(5);
    delay(850);
    wheels.write(175);
    delay(1600);
    wheels.write(60);
    esc.write(90);
  }
  // right object
  else if (leftOrRight == 1) {
    esc.write(Speed);
    wheels.write(175);
    delay(850);
    wheels.write(5);
    delay(1600);
    wheels.write(120);
    esc.write(90);
  }
  else
    Serial.println("Error in Reverse function");
}

void StopCar() 
{
  if (leftOrRight == -1) {
    Serial.println("\n   LEFT object detected.\n");
    Reverse();
  }
  else if (leftOrRight == 1) {
    Serial.println("\n   RIGHT object detected.\n");
    Reverse();   
  }  
  else if (leftOrRight == 2) {
    esc.write(90);
    Serial.println("\n    STOPPING!. Object detected on BOTH sides.");
  }
  else // should never reach this
    Serial.println("Ultrasonic debug statement: bad condition");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PollLidars()
{
  lidar_L = pulseIn(L_LIDAR_MON, HIGH) / 10; // 10usec = 1 cm of distance for LIDAR-Lite
  lidar_R = pulseIn(R_LIDAR_MON, HIGH) / 10;
  leftDistance = double(lidar_L);
  rightDistance = double(lidar_R);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PollUltrasonic() 
{
  if(digitalRead(ULTRA_LEFT) && digitalRead(ULTRA_RIGHT)) 
    leftOrRight = 2;
  else if (digitalRead(ULTRA_LEFT))
    leftOrRight = -1;
  else if (digitalRead(ULTRA_RIGHT))
    leftOrRight = 1;
  else
    leftOrRight = 0;

  if (leftOrRight)
    objectDetected = 1;  
  else 
    objectDetected = 0; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckOn() 
{
  char r;
  String incomingData;
  if (XBee.available()) {
  do {
    r = XBee.read();
    incomingData += r;   
  } 
  while (r != -1);
  incomingData = incomingData.substring(0,incomingData.length() - 1);
  }
  
  // Check data for server shutdown signal while awaiting handshake
  for (int i=0;i<incomingData.length();i++) {
    // Quit command received
    if (incomingData[i] == '1') {
      Serial.println("Remote ON received. Calibrating ESC.");
      Start = 1;
    }
  }
}

void CheckOff() 
{
  char r;
  String incomingData;
  if (XBee.available()) {
  do {
    r = XBee.read();
    incomingData += r;   
  } 
  while (r != -1);
  incomingData = incomingData.substring(0,incomingData.length() - 1);
  }
  
  // Check data for server shutdown signal while awaiting handshake
  for (int i=0;i<incomingData.length();i++) {
    // Quit command received
    if (incomingData[i] == 'Q') {
      Serial.println();
      Serial.println("Remote OFF received. Exiting Loop.");
      Serial.println();
      esc.write(90);
      Start = 0;
      delay(250);
      setup();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  // Serial monitor
  Serial.begin(9600);

  // Xbee start
  XBee.begin(9600);
  
  // Servo attach
  esc.attach(ESC_PIN);
  wheels.attach(WHEEL_PIN);

  // Remote start
  for (int i = 0; i < 100; i++)
  Serial.println();
  Serial.println("Arduino on. Waiting for remote start.\n");

  // Servo calibration
  //delay(100);
  esc.write(90); // neutral
  delay(500);
  
  //Left lidar
  pinMode(L_LIDAR_TRIG, OUTPUT);
  pinMode(L_LIDAR_MON, INPUT);
  pinMode(L_LIDAR_ENABLE, OUTPUT);
  digitalWrite(L_LIDAR_ENABLE, HIGH); //Turn sensor on
  digitalWrite(L_LIDAR_TRIG, LOW); // Set trigger LOW for continuous read
  
  // Right lidar
  pinMode(R_LIDAR_TRIG, OUTPUT);
  pinMode(R_LIDAR_MON, INPUT);
  pinMode(R_LIDAR_ENABLE, OUTPUT);
  digitalWrite(R_LIDAR_ENABLE, HIGH); //Turn sensor on
  digitalWrite(R_LIDAR_TRIG, LOW); // Set trigger LOW for continuous read

  // Ultrasonics
  pinMode(ULTRA_LEFT, INPUT);
  pinMode(ULTRA_RIGHT, INPUT);

  // initialize PID variables
  SetTunings(0.5,0.1,0);
  SetsampleTime(10); 

    while (!Start) {
    CheckOn();
  }  
  delay(500);
    
  // Corresponds to follow distance if not two walls
  // Should probably add option for dynamic adjustment in beginning
  SetHall();

  Serial.print("Minimum hall width: ");
  Serial.print(minHallWidth);
  Serial.println("cm");
  Serial.println("Starting...");
  Serial.println("\n");
  delay(1000);
}
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() 
{
  // Stopping
  CheckOff();
  PollUltrasonic();

  // Moving
  PollLidars();
  AdjustSetpoint();
  AdjustInput();  
  Compute();
  Move();

  // Serial Monitor
  Print();
}

