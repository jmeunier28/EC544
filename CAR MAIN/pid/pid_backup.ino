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
#define ULTRA_LEFT 11 
#define ULTRA_RIGHT 7 //7

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS    GLOBAL VARS  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SET THIS
const double minHallWidthInFt = 0;  // 5
const int padding = 50;

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
char leftOrRight = 'x';  

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

// super basic queue for past ultrasonic values
const int queueSize = 8;
char ultraPast[queueSize] = {'x'};

// corner variables
int cornerBins[4] = {1, 6, 11, 18};
int binNum = 0;

// Manual controls
int driveMode = 0; // 0 for autonomous, 1 for manual (toggled with manualToggle)
int stopToggle = 0; // 0 for stopped, 1 for not stopped
int manualSpeed = 90;
int manualAngle = 90;
const char manualToggle = 'T';
const char manualIncreaseSpeed = 'U';
const char manualDecreaseSpeed = 'D'; // also works to reverse
const char manualLeftTurn = '<';
const char manualRightTurn = '>';
const char manualCenter = 'C';
const char manualStopToggle = 'S';


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
}

void signMultiplier()
{
    char m = minSide();
    if (m == 'L')
      sign = -1;
    else
      sign = 1; 
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

bool all_are(char* i_begin, size_t sz, char x)
{
  for (int i = 0; i < queueSize; i++) {
    if (i_begin[i] != x)
      return false;
  }
  return true;



  
  //  const int* i_end = i_begin + sz;
 //   for(; i_begin != i_end; ++i_begin)
 //       if(*i_begin != x) return false;
//    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SETPOINT/INPUT/SPEED  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AdjustSetpoint() 
{
  // no walls
  if ((leftDistance >= 150) && (rightDistance >= 150)) 
    setpoint = 0;
  // two walls (center)
  else if ((leftDistance < 150) && (rightDistance < 150))    
    setpoint = (leftDistance+rightDistance)/2; // middle distance
  // one wall (follow wall)
  else 
    setpoint = (minHallWidth + padding) / 2; // cm
}

void AdjustInput()
{
  // no walls
  if (setpoint == 0) 
    input = 0;
  // one wall
  else if (setpoint == (minHallWidth + padding) / 2) {
    // set input to shorter side
    char s = minSide();
    if (s == 'L')
      input = leftDistance;
    else
      input = rightDistance;
  }
  // two walls
  else if (setpoint == (leftDistance+rightDistance)/2) {
        char s = minSide();
    if (s == 'L')
      input = leftDistance;
    else
      input = rightDistance; 
  }
    //input = leftDistance; // could be either
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
      if ((input == 0) && (setpoint == 0)) // no walls, go straight
        output = 0;
 
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


if (!StopCheck()) 
  Forward(); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE   FORWARD/REVERSE  

void Forward() 
{

  // change this a bit later

   if ((Angle > 70) && (Angle < 110) && (leftDistance > 40) && (rightDistance > 40))
    Speed = 44; // 42
   else
    Speed = 58;    // 58
  esc.write(Speed);
}

void Reverse() 
{ // have to play with this to get timing correct
  Serial.println("REVERSING.\n");
  Speed = 110;
  // left object
  if (ultraPast[0] == 'L') {
    esc.write(Speed);
    wheels.write(5);
    delay(850);
    wheels.write(175);
    delay(1600);
    wheels.write(60);
    esc.write(90);
  }
  // right object
  else if (ultraPast[0] == 'R') {
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

int StopCheck() 
{
  if ((ultraPast[0] == 'L') || (ultraPast[0] == 'R') || (ultraPast[0] == 'B'))
    esc.write(90);
  
  if (ultraPast[0] == 'B') {
    Serial.println("STOPPING. Object detected on both sides.\n");
    esc.write(90);
    delay(1500);
    return 1;
  }
  
  // check left (average of past n values)
  else if (all_are(ultraPast, queueSize, 'L')) {
    Serial.print("LEFT object detected. ");
    esc.write(90);
    Reverse(); 
    for (int i = 0; i < queueSize; i++)
      ultraPast[i] = 'x'; 
    return 1;
  }
  // check right (average of past n values)
  else if (all_are(ultraPast, queueSize, 'R')) {
    Serial.print("RIGHT object detected. ");
    esc.write(90);
    Reverse(); 
    for (int i = 0; i < queueSize; i++)
      ultraPast[i] = 'x';  
    return 1; 
  }  

  else {
    return 0;
  //   Serial.println("Single left or right ultrasonic value: bad data, ignore it");
  }
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
    leftOrRight = 'B';
  else if (digitalRead(ULTRA_LEFT)) {
    leftOrRight = 'L';
  }
  else if (digitalRead(ULTRA_RIGHT)) {
    leftOrRight = 'R';
  }
  else
    leftOrRight = 'N';

  // shift past values back, add current to front
  for (int i = queueSize-2; i >= 0; i--)
    ultraPast[i+1] = ultraPast[i];
  ultraPast[0] = leftOrRight;
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

void CheckSignals() 
{
  char r;
  String incomingData;

  do {
    r = XBee.read();
    incomingData += r;   
  } 
  while (r != -1);
  incomingData = incomingData.substring(0,incomingData.length() - 1);
  
  
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
    else if (incomingData[i] == manualToggle) {
      driveMode = !driveMode;
      manualSpeed = 90;
      manualAngle = 90;
      Serial.print("\nToggling auto/manual modes. Current mode: ");
      Serial.println(driveMode);
      Serial.println();
    }
    else if (incomingData[i] == manualStopToggle) {
        stopToggle = !stopToggle;
        Serial.print("\nToggling start/stop. Current mode: ");
        Serial.println(stopToggle);
        Serial.println();
    }
    else if (incomingData[i] == manualIncreaseSpeed) {
        manualSpeed -= 5;
        Serial.print("\nIncreasing speed. Current speed: ");
        Serial.println(manualSpeed);
        Serial.println();
    }
    else if (incomingData[i] == manualDecreaseSpeed) {
        manualSpeed += 5;
        Serial.print("\nDecreasing speed. Current speed: ");
        Serial.println(manualSpeed);
        Serial.println();
    }  
    else if (incomingData[i] == manualLeftTurn) {
        manualAngle += 10;
        Serial.print("\nIncreasing left turn. Current angle: ");
        Serial.println(manualAngle);
        Serial.println();
    }    
    else if (incomingData[i] == manualRightTurn) {
        manualAngle -= 10;
        Serial.print("\nIncreasing right turn. Current angle: ");
        Serial.println(manualAngle);
        Serial.println();
    }
    else if (incomingData[i] == manualCenter) {
        manualAngle = 90;
        Serial.print("\nCentering wheels. Current angle: ");
        Serial.println(manualAngle);
        Serial.println();
    }          
   
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CORNERS    CORNERS    CORNERS    CORNERS    CORNERS    CORNERS    CORNERS    CORNERS    CORNERS    CORNERS  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CheckCorners()
{
  int inCorner;
  if ((binNum != cornerBins[0]) && (binNum != cornerBins[1]) 
      && (binNum != cornerBins[2]) && (binNum != cornerBins[3]))
    inCorner = 0;
  else
    inCorner = 1;
  return inCorner; 
}

// going to always turn left
void turnCorner() {
  
  // check front collision
  if (ultraPast[0] = 'B') {
    esc.write(110);
    delay(1000);
    esc.write(90);
  } 
  // navigate turn
  else {
    esc.write(90);
    PollLidars();
    
    // if very close to the wall move away from it
    if (leftDistance < 30) {
      wheels.write(50);
      esc.write(80);
      delay(1000);
    }
    // if near the wall but not very close continue to go straight
    else if (leftDistance < 60) {
        wheels.write(90);
        esc.write(80);
    }
    // if enough distance between wall make the turn
    else {     
      wheels.write(135); // try this (left turn)
      esc.write(80);
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
  SetTunings(0.62,0.0,0.01);
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
  // forward collision detection
  PollUltrasonic();
  // Stopping

  if (XBee.available()) // incoming data
    CheckSignals();
  else
  {  
      
      // autonomous driving
      if (driveMode == 0) {
        if (CheckCorners()) { // going to drive counter-clockwise around loop
           // do corner stuff
    
           turnCorner();
             
        } 
        else { // going straight down hallway
          // Moving
          PollLidars();
          AdjustSetpoint();
          AdjustInput();  
          Compute();
          Move();
          // Serial Monitor
          Print();
        }
      }
      // manual driving
      else // driveMode == 1
      {
        if (!stopToggle) {
          esc.write(manualSpeed);
          wheels.write(manualAngle);   
        }
        else 
          esc.write(90);
        
      }

  }
}

