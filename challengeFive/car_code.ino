
#include <PID_v1.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <math.h>




#define PIN_INPUT 0
#define PIN_OUTPUT 3

// Servos
Servo wheels;
Servo esc;
bool startup = true;
int startupDelay = 1000;
double maxSpeedOffset = 45;
double maxWheelOffset = 85;
int wheel_input;
int esc_input;


// Ultrasonic sensor
const int pwPin = 11;
long ultra, inches, cm;
long avg;
const int stopPin = 1;

//XBee comms
SoftwareSerial XBee(2,3); // RX, TX



//Define Variables we'll be connecting to PID control
double Setpoint, Input, Output;
long Input_long;
double left_right;

//Specify the links and initial tuning parameters
double Kp=3.3, Ki=0, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// LIDAR
unsigned long pulse_width_left, pulse_width_right;


void setup()
{
  
  Serial.begin(9600); // Start serial communications
  XBee.begin(9600); // Start XBee communications
  
  // LIDAR left
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  pinMode(3, INPUT); // Set pin 3 as monitor pin
  pinMode(4, OUTPUT); // Set pin 4 to control power enable line
  digitalWrite(4,HIGH); //Turn sensor on
  digitalWrite(2, LOW); // Set trigger LOW for continuous read
  // LIDAR right
  pinMode(8, OUTPUT); // Set pin 2 as trigger pin
  pinMode(9, INPUT); // Set pin 3 as monitor pin
  pinMode(12, OUTPUT); // Set pin 4 to control power enable line
  digitalWrite(12,HIGH); //Turn sensor on
  digitalWrite(8, LOW); // Set trigger LOW for continuous read

  
  
  //initialize the variables we're linked to PID
  Input = 30; // LIDAR Delta
  Setpoint = 30; // distance we want to maintain
  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  // Servo attach
  wheels.attach(6);
  esc.attach(5);
  // Calibration
  wheels.write(90);
  delay(1000);
  esc.write(90); // neutral

  // Ultrasonic setup
  pinMode(pwPin, INPUT);
  pinMode(stopPin, OUTPUT);
  digitalWrite(stopPin, LOW);
  
  
}


double degToRad(double degrees) {
  return (degrees * 71) / 4068;
}

double ragToDeg(double radians) {
  return (radians * 4068) / 71;
}


/*
// Calibrate the ESC by sending a high signal, then a low, then middle.
void calibrateESC(){
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}
void oscillate(){
  for (int i =0; i < 360; i++){
    double rad = degToRad(i);
    double speedOffset = sin(rad) * maxSpeedOffset;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(90 + speedOffset);
    wheels.write(90 + wheelOffset);
    delay(50);
  }
}
*/


void loop()
{
  // LIDAR
  pulse_width_left = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
  pulse_width_right = pulseIn(9, HIGH);
  
  if(pulse_width_left != 0)
  { // If we get a reading that isn't zero, let's print it
  
      pulse_width_left = pulse_width_left/10; // 10usec = 1 cm of distance for LIDAR-Lite
      Serial.print("Left LIDAR: ");
      Serial.println(pulse_width_left); // Print the distance
      
      XBee.print("l, " + String(pulse_width_left)); // send values 
      XBee.write("\n"); // binary data
  }
  
  else
  { // We read a zero which means we're locking up. 
  
    digitalWrite(4,LOW); // Turn off the sensor
    delay(1);// Wait 1ms
    digitalWrite(4,HIGH); //Turn on te sensor
    delay(1);//Wait 1ms for it to turn on.
    
  }
  
  delay(1); //Delay so we don't overload the serial port
  
  if(pulse_width_right != 0)
  { // If we get a reading that isn't zero, let's print it
  
        pulse_width_right = pulse_width_right/10; // 10usec = 1 cm of distance for LIDAR-Lite
        Serial.print("Right LIDAR: ");
        Serial.println(pulse_width_right); // Print the distance
        
        XBee.print("r, " + String(pulse_width_right)); // send values 
        XBee.write("\n"); // binary data
        
  }
  
  else
  { // We read a zero which means we're locking up.
  
    digitalWrite(12,LOW); // Turn off the sensor
    delay(1);// Wait 1ms
    digitalWrite(12,HIGH); //Turn on te sensor
    delay(1);//Wait 1ms for it to turn on.
    
  }
  
  delay(1); //Delay so we don't overload the serial port


  // PID
  Input_long = (pulse_width_left - pulse_width_right);
  left_right = double(Input_long);
  Input = abs(left_right);
  myPID.Compute();
  Serial.print("left_right: ");
  Serial.println(left_right);  
  Serial.print("PID input: ");
  Serial.println(Input);
  Serial.print("PID output: ");
  Serial.println(Output);
  XBee.print("p, " + String(Output)); // send values 
  XBee.write("\n"); // binary data


  // turn
  if (Input > 10) {
    if (left_right > 0) {
      Serial.print("Go <<LEFT<< with strength: ");
      wheel_input = 60;   
    }
    else if (left_right < 0) {
      Serial.print("Go >>RIGHT>> with strength: ");
      wheel_input = -60;
    }
    Serial.println(Output);
  } 
  // don't turn
  else {
    Serial.println("Go ^^STRAIGHT^^");
    wheel_input = 0;
  }

  // update wheels
  wheels.write(90 + wheel_input);
  


  Serial.println();
  delay(1000);



  // Ultrasonic sensor
  ultra = pulseIn(pwPin, HIGH);
  inches = ultra / 147;
  cm = inches * 2.54;
  if (cm <= 50){
    digitalWrite(stopPin, HIGH);
    Serial.println("STOP CAR!!");
    esc.write(90);  
  }
  else {
    esc.write(20);
  }
  Serial.print(cm);
  Serial.println("cm");

  

  // go slow
  //esc.write(80);

  
  // SetSampleTime() // default 200ms

  //analogWrite(PIN_OUTPUT, Output);
}