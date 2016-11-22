#include <Servo.h>
#include <math.h>
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
 
void setup()
{
<<<<<<< HEAD
  //wheels.attach(6); // initialize wheel servo to Digital IO Pin #8
=======
  wheels.attach(6); // initialize wheel servo to Digital IO Pin #8
>>>>>>> dami
  esc.attach(5); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();
}

/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC(){
<<<<<<< HEAD
  //  esc.write(180); // full backwards
  //  delay(startupDelay);
  //  esc.write(0); // full forwards
  //  delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
   // esc.write(90); // reset the ESC to neutral (non-moving) value
=======
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
>>>>>>> dami
}

/* Oscillate between various servo/ESC states, using a sine wave to gradually 
 *  change speed and turn values.
 */
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
<<<<<<< HEAD

 
void loop()
{

    esc.write(70);
    delay(100);
  // oscillate();
=======
 
void loop()
{
   oscillate();
>>>>>>> dami
}


