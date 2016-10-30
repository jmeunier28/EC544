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


/* ---------- Global Variables ---------- */
// Servo global variables
Servo wheels;
Servo esc;

// PID setpoint (wheel orientation)
const double setpoint = 90;

// PID constants
const double Kp=1, Ki=0, Kd=0;

// Lidar readings
unsigned long lidar_L, lidar_R;

// Ultrasonic global variables
long ultra, inches, cm, average;

// On/off 
int start = 1;

// PID global variables
const double dt = 200; // poll interval in ms
double delta, delta_90, process_feedback;
double error, previous_error, integral, derivative, output;

/* --------------------- Functions --------------------- */

void PID_Control() {
  delay(dt);  
  Serial.print("setpoint: ");
  Serial.println(setpoint);
  Serial.print("process_feedback: ");
  Serial.println(process_feedback);
  error = setpoint - process_feedback;
  integral = integral + (error * dt);
  derivative = (error - previous_error) / dt;
  output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  previous_error = error;
  Serial.print("error: ");
  Serial.println(error);
  Serial.print("PID output: ");
  Serial.println(output);
  //Serial.print("previous_error: ");
  //Serial.print(previous_error);
  if ((output >= -0.000001) && (output <= 0.000001)){ // (supposedly bad practice to use == with floats,
    Serial.println("Going STRAIGHT.");        // but when it shows -0.000000 it's actually less than 0) 
  }
  else if (output > 0.000001) {
    Serial.println("Turning LEFT.");
  }
  else if (output < -0.000001) {
    Serial.println("Turning RIGHT.");
  }
  Serial.println();
  // control wheels
  wheels.write(90 + output);
}                 

void Poll_Lidars() {
  lidar_L = pulseIn(L_LIDAR_MON, HIGH) / 10; // 10usec = 1 cm of distance for LIDAR-Lite
  lidar_R = pulseIn(R_LIDAR_MON, HIGH) / 10;
  Serial.print("lidar_L: ");
  Serial.print(lidar_L);
  Serial.print("cm       ");
  Serial.print("lidar_R: ");
  Serial.print(lidar_R);
  Serial.println("cm");
  delta = lidar_L - lidar_R;
  delta_90 = 90*(2*((delta + 2500)/(5000)) - 1);
  process_feedback = 90 - delta_90;
}

void Poll_Ultrasonic(){
  ultra = pulseIn(ULTRA_PIN, HIGH);
  inches = ultra / 147;
  cm = inches * 2.54;
  Serial.print("Front sensor distance: ");
  Serial.print(cm);
  Serial.println("cm");
  if (cm <= 50) {
    esc.write(90); // stop
    Serial.println("Stop car!");
  }
  else {
    esc.write(70); // go slow
  }
}

/* --------------------------- SETUP --------------------------- */

void setup() {
  if (start) { // to be remote start signal 
    Serial.begin(9600);
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
  
    // PID controller setup
    delta = lidar_L - lidar_R;
    delta_90 = 90*(2*((delta + 2500)/(5000)) - 1);
    process_feedback = 90 - delta_90;
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
    // left-right orientation
    if (lidar_L > lidar_R)
      Serial.println("Starting car closer to right wall.");
    else if ( lidar_L < lidar_R) 
      Serial.println("Starting car closer to left wall.");
    else 
      Serial.println("Starting car evenly between walls.");
    // process_feedback (interpreted based on how it needs to affect output)
    //Serial.print("process_feedback: ");
    //Serial.println(process_feedback);
    // Left or Right turn required
    if (process_feedback < 90) {
      Serial.print("LEFT turn required from initial position in order to ");
      Serial.println("move process_feedback toward setpoint.");
    }
    else if (process_feedback > 90) {
      Serial.print("RIGHT turn required from initial position in order to ");
      Serial.println("move process_feedback toward setpoint."); 
    }
    else if (process_feedback == 90) {
      Serial.println("Going STRAIGHT already! (process_feedback == setpoint)");   
    }
    Serial.println("Starting...");
    Serial.println("\n");
    delay(1500);
  }
}

/* --------------------------------- MAIN --------------------------------- */

void loop() {

  // Stopping
  Poll_Ultrasonic();
  
  // Steering
  PID_Control();  
}

