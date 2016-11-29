// Libraries
#include <SoftwareSerial.h>
#include <Servo.h>

/* ------- PINS ------- */

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
#define ULTRA_PIN 11

//LED Control Pins
#define RIGHT 13
#define LEFT 10

/* ---------- Global Variables ---------- */
// Remote
SoftwareSerial XBee(2,3); // RX, TX
int Start = 0;

// Servo global variables
Servo wheels;
Servo esc;

// PID Setpoint (difference in distance between walls)
const int Setpoint = 0;   // maybe make this an int

// PID constants
const double Kp = 1;
const double Ki = 0;
const double Kd = 0;

// Lidar readings
unsigned long lidar_L, lidar_R;
int lidar_L_signed, lidar_R_signed;

// Ultrasonic global variables
long ultra, inches, cm, average;
int Stop = 0;
int ObjectDetected = 0;

// PID global variables
const double dt = 10; // poll interval in ms
int Input;   // Positive means closer to right (so move left)
int Output; 
int Error;
int PreviousErr;
double Integral;
double Derivative;

// Movement
int Angle;
int Speed;

/* --------------------- Functions --------------------- */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID    PID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// PID CONTROL
void PID_Control() {
  delay(dt);

  Error = Setpoint - Input;

  Integral = Integral + (Error * dt);
  Derivative = (Error - PreviousErr) / dt;
  
  Output = (Kp * Error) + (Ki * Integral) + (Kd * Derivative);

  PreviousErr = Error;

  Serial.print("Error: ");
  Serial.println(Error);
  Serial.print("Output: ");
  Serial.println(Output);


  if (!ObjectDetected)
    MoveCar();
  else {
    Serial.println("OBJECT DETECTED! \/\/ Backing up. \/\/");
    
    Speed = 110;
    esc.write(Speed);
    delay(3000);

    Serial.print("Speed: ");
    Serial.println(Speed);
    Serial.println();
  }

  
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE    MOVE  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveCar() {
  int turnNumber;
  int Angle;
  int Speed;
  int Offset;
  int turnMultiplier = 7;

  // Bad Lidar reading (really when == 0)
  if ((lidar_L_signed < 15) || (lidar_R_signed < 15)) {
    Serial.println("   CASE 1");
    // slow down while lidar reading bad value
    Speed = 78;
    esc.write(Speed); // go slow when no walls on either side (for now at least) 
    
    // bad left reading
    if (lidar_L_signed < 15) {
      // try to stay ~2ft from only (right) wall
      if (lidar_R_signed > 40) // move closer to only (right) wall
        wheels.write(55);
      else if (lidar_R_signed < 30) // move further from only (right) wall
        wheels.write(105);
      else 
        wheels.write(90);
      Serial.println("Bad lidar reading. Trying to follow RIGHT wall.");     
      Serial.print("Speed: ");
      Serial.println(Speed);
    }
    // bad right reading
    else if (lidar_R_signed < 15) {
      // try to stay ~2ft from only (left) wall
      if (lidar_L_signed > 40) // move closer to only (left) wall
        wheels.write(125);
      else if (lidar_L_signed < 30) // move further from only (left) wall
        wheels.write(75);
      else 
        wheels.write(90);
      Serial.println("Bad lidar reading. Trying to follow LEFT wall.");     
      Serial.print("Speed: ");
      Serial.println(Speed);
    }  
  }
     
  // WALLS ON BOTH SIDES  (walls or objects on both sides)
  else if ((abs(Input) < 150) && ((lidar_L_signed < 150) && (lidar_R_signed < 150))) {  
    Serial.println("   CASE 2");
      // Turn amount 5:max , 0:none
      if (abs(Input) > 100) 
        turnNumber = 5;
      else if (abs(Input) > 70)
        turnNumber = 4;
      else if (abs(Input) > 50)
        turnNumber = 3;
      else if (abs(Input) > 30)
        turnNumber = 2;
      else if (abs(Input) > 15)
        turnNumber = 1;
      else 
        turnNumber = 0;     
        
      // Make a turn
      if (turnNumber > 0) {
          // Move Left
          if (Output < 0) { // Left turn is increasing angle
              Angle = 90 + turnMultiplier*turnNumber;
              wheels.write(Angle);
              Serial.print("Wheels: << Left turn <<");  
          }
          // Move Right
          else if (Output > 0) { // Right turn is decreasing angle
              Angle = 90 - turnMultiplier*turnNumber; // lowered this
              wheels.write(Angle);
              Serial.print("Wheels: >> Right turn >>");
          }
          else { // Output == 0 and Input > 10 (should never happen)
            Serial.print("Wheels: Fuckin up..shouldn't be here...");
          }
          Serial.print("   turnNumber: ");
          Serial.print(turnNumber);
          Serial.print("    Angle: ");
          Serial.println(Angle);
      }
      else { // turnNumber == 0 (no turn)
        wheels.write(90);
        Serial.println("Wheels: Straight   turnNumber: 0"); 
      }
      // Bigger turn -> slower speed
      Speed = 67 + turnNumber; 
      esc.write(Speed);
      Serial.print("Speed: ");
      Serial.println(Speed);
  }


  // OPENING ON ONE OR BOTH SIDES
  else {
      Serial.println("   CASE 3");
 
      // slow down while one or more walls is missing
      Speed = 77;
      esc.write(Speed); // go slow when no walls on either side (for now at least)  
      // no walls
      if ((lidar_L_signed > 155) && (lidar_R_signed > 155)) {
          if (Input == 0) // straight
            Offset = 0;
          else if (Input < 0) // closer to left, drift right slightly
            Offset = -5;
          else if (Input > 0) // closer to right, drift left slightly
            Offset = 5;
          Angle = 90 + Offset;
          wheels.write(Angle);
          Serial.println("NO WALLS! Slowing down.");
          Serial.print("Speed: ");
          Serial.println(Speed);
      }
      // no left wall
      else if (lidar_L_signed > lidar_R_signed) {
        // try to stay ~2ft from only (right) wall
        if (lidar_R_signed > 95) // move closer to only (right) wall
          wheels.write(80);
        else if (lidar_R_signed < 70) // move further from only (right) wall
          wheels.write(140);
        else 
          wheels.write(90);
        Serial.println("No LEFT wall! slowing down.");     
        Serial.print("Speed: ");
        Serial.println(Speed);
      }
      // no right wall
      else if (lidar_R_signed > lidar_L_signed) {
        // try to stay ~2ft from only (left) wall
        if (lidar_L_signed > 95) // move closer to only (left) wall
          wheels.write(100);
        else if (lidar_L_signed < 70) // move further from only (left) wall
          wheels.write(40);
        else 
          wheels.write(90);
        Serial.println("No RIGHT wall! slowing down.");     
        Serial.print("Speed: ");
        Serial.println(Speed);
    } 
  }
 
  Serial.println();
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS    LIDARS  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// LIDAR SENSORS
void Poll_Lidars()
{
  lidar_L = pulseIn(L_LIDAR_MON, HIGH) / 10; // 10usec = 1 cm of distance for LIDAR-Lite
  lidar_R = pulseIn(R_LIDAR_MON, HIGH) / 10;
  lidar_L_signed = double(lidar_L);
  lidar_R_signed = double(lidar_R);
  Input = lidar_L_signed - lidar_R_signed; 
  PrintLidar();
}

void PrintLidar()
{
  Serial.print("Left: ");
  Serial.print(lidar_L_signed);
  Serial.print("cm       ");
  Serial.print("Right: ");
  Serial.print(lidar_R_signed);
  Serial.println("cm");
  Serial.print("Input (L - R): ");
  Serial.println(Input);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC    ULTRASONIC  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Poll_Ultrasonic() {
  ultra = pulseIn(ULTRA_PIN, HIGH);
  inches = ultra / 147;
  cm = inches * 2.54;
  Serial.print("Front: ");
  Serial.print(cm);
  Serial.println("cm");

  if (cm < 44)               // CHANGE THIS BACK AFTER TESTING
    ObjectDetected = 1;
  else
    ObjectDetected = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  REMOTE    REMOTE    REMOTE    REMOTE    REMOTE    REMOTE    REMOTE    REMOTE    REMOTE    REMOTE    REMOTE  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Poll_Remote() {

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

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
  while (!Start)
    CheckOn();

  // Servo calibration
  esc.write(90); // neutral
  delay(1000);

  //DRIVING LIGHTS
  pinMode(RIGHT, OUTPUT);
  pinMode(LEFT, OUTPUT);
  digitalWrite(RIGHT, LOW);
  digitalWrite(LEFT, LOW);
  
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
    
  // Servo calibration
  delay(500);

  // Ultrasonic
  pinMode(ULTRA_PIN, INPUT);

  // initialize PID variables
  PreviousErr = Setpoint - Input;
  Integral = 0;

  // Check distances
  Serial.print("Initial Position:   "); 
  Poll_Lidars();
  Serial.println("Starting...");
  Serial.println("\n");
  delay(500);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF    ON/OFF  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckOn() {
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

void CheckOff() {
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
//  MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN    MAIN  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {


  // Remote
  CheckOff();

  // Stopping
  Poll_Ultrasonic();
  Poll_Remote();

  // Steering
  Poll_Lidars();
  PID_Control();

}

