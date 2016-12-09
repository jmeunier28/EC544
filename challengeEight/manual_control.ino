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
#define ULTRA_RIGHT 7 

// Servo global variables
Servo wheels;
Servo esc;

// Lidar readings
unsigned long lidar_L, lidar_R;
double leftDistance, rightDistance;

// Ultrasonic global variables
int objectDetected = 0;

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
int elevator = 0;


int binNum = 0;
int goCar = 0;
int driveMode = 0; // 0 for autonomous, 1 for manual (toggled with manualToggle)
int stopToggle = 0; // 0 for stopped, 1 for not stopped
int manualSpeed = 90;
int manualAngle = 90;
char manualToggle = 'T';
char manualIncreaseSpeed = 'U';
char manualDecreaseSpeed = 'D'; // also works to reverse
char manualLeftTurn = '<';
char manualRightTurn = '>';
char manualCenter = 'C';
char manualStopToggle = 'S';
char startCar = 'g';
char stopCar = 'Q';

bool user_detect = 0;
String user_command;
char inChar;

void serialEvent()
{
    inChar = Serial.read();
    //user_command += inChar;
    user_detect = 1;

}


void doSomething() 
{
    if (inChar == 'T') {
      driveMode = !driveMode;
      //inChar = '';
      manualSpeed = 90;
      manualAngle = 90;
      stopToggle = 0;
      //esc.write(manualSpeed);
      //wheels.write(manualAngle);
      
      //Serial.print("\nToggling auto/manual modes. Current mode: ");
      //Serial.println(driveMode);
      //Serial.println();
    }
    else if (inChar == manualStopToggle) {
        stopToggle = !stopToggle;
        manualSpeed = 90;

        
        //esc.write(manualSpeed);
        //inChar = '';
//        Serial.print("\nToggling start/stop. Current mode: ");
//        Serial.println(stopToggle);
//        Serial.println();
    }
    else if (inChar == manualIncreaseSpeed) {
        manualSpeed -= 5;
        //esc.write(manualSpeed);
        //inChar = '';
//        Serial.print("\nIncreasing speed. Current speed: ");
//        Serial.println(manualSpeed);
//        Serial.println();
    }
    else if (inChar == manualDecreaseSpeed) {
        manualSpeed += 5;
        //esc.write(manualSpeed);
        //inChar = '';
//        Serial.print("\nDecreasing speed. Current speed: ");
//        Serial.println(manualSpeed);
//        Serial.println();
    }  
    else if (inChar == manualLeftTurn) {
        manualAngle += 20;
        //wheels.write(manualAngle);
        //inChar = '';
//        Serial.print("\nIncreasing left turn. Current angle: ");
//        Serial.println(manualAngle);
//        Serial.println();
    }    
    else if (inChar == manualRightTurn) {
        manualAngle -= 20;
        //wheels.write(manualAngle);
        //inChar = '';
//        Serial.print("\nIncreasing right turn. Current angle: ");
//        Serial.println(manualAngle);
//        Serial.println();
    }
    else if (inChar == manualCenter) {
        manualAngle = 90;
        //wheels.write(manualAngle);
        //inChar = '';
//        Serial.print("\nCentering wheels. Current angle: ");
//        Serial.println(manualAngle);
//        Serial.println();
    } 
      String msg = "3";
      String man = "m";
      
      moveForward(manualSpeed,manualAngle);
      
      //esc.write(manualSpeed);
      //wheels.write(manualAngle);

       String sendData = String(manualSpeed) + ',' + String(msg) + ',' + String(man) + ',' + String(manualAngle);
       Serial.println(sendData);    
   
  }
  void moveForward(int manSpeed, int manAng){
      esc.write(manSpeed);
      wheels.write(manAng);
  }

void setup() {
  
  Serial.begin(9600);
  
  // Servo attach
  esc.attach(ESC_PIN);
  wheels.attach(WHEEL_PIN);


  // Servo calibration
  //delay(100);
  esc.write(90); // neutral
  
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
  //SetTunings(0.67,0.0,0.01);
  //SetsampleTime(10); 

}


void loop() {
    if (user_detect){
      if (inChar == startCar){
        goCar = 1;
      }
      else if(inChar == stopCar){
        goCar = 0;
      }
      if(inChar == manualToggle){
        driveMode = !driveMode;
      }
      
      if(driveMode && goCar){
        doSomething();
      }

      user_detect = 0; //reset flag
  }
  
    if (goCar && !driveMode){
      //drive autonomous
        Serial.println(40);
      
    }
    delay(1000);
 

}