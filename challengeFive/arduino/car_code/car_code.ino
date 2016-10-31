// Libraries
#include <Servo.h>

/* ------- PINS ------- */

// Servo pins
#define ESC_PIN 5
#define WHEEL_PIN 6

// Lidar pins
#define L_LIDAR_TRIG 2
#define L_LIDAR_MON 3
#define L_LIDAR_ENABLE 4
#define R_LIDAR_TRIG 8
#define R_LIDAR_MON 9
#define R_LIDAR_ENABLE 12

// Ultrasonic pins
#define ULTRA_PIN 11
#define ULTRA_STOP 1

//LED Control Pins
#define STOP 0
#define STRAIGHT 1
#define RIGHT 13
#define LEFT 10
/* ---------- Global Variables ---------- */

// Servo global variables
Servo wheels;
Servo esc;

// PID setpoint (wheel orientation)
const double setpoint = 90;

// PID constants
const double Kp=1.5, Ki=0, Kd=0;

// Lidar readings
unsigned long lidar_L, lidar_R;
double lidar_L_signed, lidar_R_signed;

// Ultrasonic global variables
long ultra, inches, cm, average;
bool stopCar = false;

// On/off 
int start = 1;

// PID global variables
const double dt = 10; // poll interval in ms
double delta, delta_90, process_feedback, wheel_angle;
double error, previous_error, integral, derivative, output;
int maxDelta = 150; // max difference of lidar readings
int n_constant = 90; // scales -1:1 to -90:90

/* --------------------- Functions --------------------- */

/*

Different Cases For Speed:

  (1) Wheel adj angle is small (< 5): can keep moving at fast speed
  (2) Wheel adj is of medium size (< 10): slow down by 3 while turning
  (3) Wheel adj is large (> 10): slow down by 5 while turning 

Different Cases for movement along Path:

  (1) Lidar initial position senses walls on both sides 
      - walls are close to equidistant --> move forward
      - one wall is closer than other by > 20 cm --> slowly bring car to center
  (2) Lidar initial posiiton senses only one wall 
      - one wall is much farther away or not there diff >> 200cm  --> will move straight with wall next to lidar
        do this until Lidar sees walls on both sides (diff < 200cm)
  (3) Car is in sideways initial position ******* NEED TO THINK ABOUT HOW TO DEAL WITH THIS ******
  (4) Lidar is moving and loses one wall 
      - diff between left and right >> 200cm --> continue straight do not adjust 
        until Lidar sees two walls
        
*/

// Initialize Poisition of Car to a neutral spot
/*void InitializeCarPosition()
{
  Serial.print("Initializing Position");
 //Check initial condition cases
 if (abs(delta) < 20 ) // walls approx equidistant do nothing
 {
   Serial.print("Walls are only: ");
   Serial.println(delta);
   Serial.print(" apart, move forward");
   wheels.write(90);
   delay(1);
   esc.write(75);
   delay(100);
 }
 
 else if (20 < abs(delta) < 150)
 {
    Serial.print("Difference is bigger than 20 but less than 200 wall is there try to find it"); 
 }
 else if (abs(delta) > 150)
 {
   Serial.print("No wall next to start point... adjust");
   wheels.write(90);
   delay(1);
   esc.write(74); //jolt forward
   delay(100); 
 }

     //NEED TO DETERMINE HOW TO FIX CASE WHERE CAR IS NOT STRAIGHT  
}*/

void printPID()
{
  Serial.print("setpoint: ");
  Serial.println(setpoint);
  Serial.print("process_feedback: ");
  Serial.println(process_feedback);
  Serial.print("error: ");
  Serial.println(error);
  Serial.print("PID output: ");
  Serial.println(output);
  
    if ((output >= -0.000001) && (output <= 0.000001))
    { // (supposedly bad practice to use == with floats,
      Serial.println("Going STRAIGHT."); 
      digitalWrite(STRAIGHT, HIGH);
      digitalWrite(RIGHT, LOW);
      digitalWrite(LEFT, LOW);
      digitalWrite(STOP, LOW);
    }
  else if (output > 0.000001) 
  {
    Serial.println("Turning LEFT.");
    digitalWrite(LEFT, HIGH);
    digitalWrite(RIGHT, LOW);
    digitalWrite(STRAIGHT, LOW);
    digitalWrite(STOP, LOW);
  }
  else if (output < -0.000001) {
    Serial.println("Turning RIGHT.");
    digitalWrite(RIGHT, HIGH);
    digitalWrite(LEFT, LOW);
    digitalWrite(STRAIGHT, LOW);
    digitalWrite(STOP, LOW);
  }
  Serial.print("wheel_angle: ");
  Serial.println(wheel_angle);
  Serial.println(); 
  
}
  

// PID CONTROL
void PID_Control() {
  delay(dt);  
  error = setpoint - process_feedback;
  integral = integral + (error * dt);
  derivative = (error - previous_error) / dt;
  output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  previous_error = error; 
  // control wheels
  wheel_angle = 90 + output;
  //wheels.write(wheel_angle);
  printPID();
  MoveCar(output);
}

void MoveCar(double output)
{
   
  if (stopCar)
  {
    //put everything in neutral
    esc.write(90);
    delay(1);
    wheels.write(90); 
    delay(10);
    digitalWrite(RIGHT, LOW);
    digitalWrite(LEFT, LOW);
    digitalWrite(STRAIGHT, LOW);
    digitalWrite(STOP, HIGH);
    
    //esc.write(120); //backup
    //delay(100); 
  }
  
  else 
  {
  // Address Speed Cases:
  
    //Case One Wheel Angle Adj is small
     if (abs(output) < 10)
      {
        Serial.print("Case One: ");
         wheels.write(wheel_angle);
         delay(1);
         esc.write(72); //can move pretty fast here
         delay(50);
       
       } //end case one
     
     //Case two Wheel adj medium
     else if (10 < abs(output) < 25)
     {
       Serial.print("Case Two: ");
        wheels.write(wheel_angle);
        delay(200);
        esc.write(76); //slow down to make this adj
        delay(50);
        
      } //end case two
      
      //Case three wheel adj large
      else if(abs(output) > 25) 
        {
          Serial.print("Case Three output: ");
          if (abs(delta) > 150) //if diff greater than 200 we lost a wall
          { 
            wheels.write(90); //keep moving straight 
            delay(1);
            esc.write(76); //slow down a little bit
            delay(50);
          }
          
          else
          {
            wheels.write(wheel_angle);
            delay(1);
            esc.write(77); // go real slow to make this adjustment
            delay(50);
          }
          
        } //end case three
       
  } //end car moving else statement 
        
        
} //end MoveCar function 

void printLidar()
{  
  Serial.print("lidar_L: ");
  Serial.print(lidar_L_signed);
  Serial.print("cm       ");
  Serial.print("lidar_R: ");
  Serial.print(lidar_R_signed);
  Serial.println("cm");
  Serial.print("delta: ");
  Serial.println(delta);
  Serial.println();
}
  

// LIDAR SENSORS
void Poll_Lidars() 
{
  lidar_L = pulseIn(L_LIDAR_MON, HIGH) / 10; // 10usec = 1 cm of distance for LIDAR-Lite
  lidar_R = pulseIn(R_LIDAR_MON, HIGH) / 10;
  lidar_L_signed = double(lidar_L);
  lidar_R_signed = double(lidar_R);
  delta = lidar_L_signed - lidar_R_signed;
  printLidar();

 if (abs(delta) > 150)
 {
   if (lidar_L_signed > lidar_R_signed)
   {
      delta = 75 - lidar_R_signed; //left wall dropped out assign it to be max distance away
      Serial.print("New Delta no left wall: ");
      Serial.println(delta);
      delta_90 = n_constant * (2*((delta + maxDelta)/(2*maxDelta)) - 1);
      process_feedback = 90 - delta_90; 
      Serial.print("delta_90: ");
      Serial.println(delta_90);
   }
   
   else if (lidar_L_signed < lidar_R_signed)
   {
      delta = lidar_L_signed - 75; // if its greater than 150 we lost a wall so we want to just move straight
      Serial.print("No Right wall, new delta: ");
      Serial.println(delta);
      delta_90 = n_constant * (2*((delta + maxDelta)/(2*maxDelta)) - 1);
      process_feedback = 90 - delta_90; 
      Serial.print("delta_90: ");
      Serial.println(delta_90);
   } 
 }
 else
 {
      delta_90 = n_constant * (2*((delta + maxDelta)/(2*maxDelta)) - 1);
      process_feedback = 90 - delta_90; 
      Serial.print("delta_90: ");
      Serial.println(delta_90);
 }
}

// ULTRASONIC
void Poll_Ultrasonic(){
  ultra = pulseIn(ULTRA_PIN, HIGH);
  inches = ultra / 147;
  cm = inches * 2.54;
  Serial.print("Front sensor distance: ");
  Serial.print(cm);
  Serial.println("cm");
  if (cm <= 50) {
    esc.write(90); // stop
    stopCar = true;
    Serial.println("Stop car!");
  }
  else {
    //esc.write(74); // go slow
    stopCar = false;
  }
}

/* --------------------------- SETUP --------------------------- */

void setup() 
{
  if (start) 
  { // to be remote start signal 
    Serial.begin(9600);
    
    //DRIVING LIGHTS
    pinMode(STRAIGHT, OUTPUT);
    pinMode(RIGHT, OUTPUT);
    pinMode(LEFT, OUTPUT);
    pinMode(STOP, OUTPUT);    
    
    
    digitalWrite(STRAIGHT, LOW);
    digitalWrite(RIGHT, LOW);
    digitalWrite(LEFT, LOW);
    digitalWrite(STOP, LOW);
   
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
    // Servo attach
    esc.attach(ESC_PIN);
    wheels.attach(WHEEL_PIN);
    // Servo calibration
    esc.write(90); // neutral
    delay(2000);
    wheels.write(90); // possibly not even needed, try commenting out
    // Ultrasonic
    pinMode(ULTRA_PIN, INPUT);
    pinMode(ULTRA_STOP, OUTPUT);
    digitalWrite(ULTRA_STOP, LOW);
    // Initial lidar poll
    Poll_Lidars();
    // initialize PID variables
    previous_error = setpoint - process_feedback;
    integral = 0;
    /* A bunch of initial setup print statements */
    // distances
    Serial.print("Distance from left wall: ");
    Serial.print(lidar_L);
    Serial.println("cm");
    Serial.print("Distance from right wall: ");
    Serial.print(lidar_R);
    Serial.println("cm");
    //InitializeCarPosition();
    Serial.println("Starting...");
    Serial.println("\n");
    delay(1000);
  }
}

/* --------------------------------- MAIN --------------------------------- */

void loop() {
  // Stopping
  Poll_Ultrasonic();
  
  // Steering
  Poll_Lidars();
  PID_Control();  
}

