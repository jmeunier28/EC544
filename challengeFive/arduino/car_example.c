

#include <stdio.h>
#include <unistd.h>

/* GLOBAL VARIABLES */

// basically just proportionality-control algorithm since Ki=Kd=0
const double Kp = 1;
const double Ki = 0;
const double Kd = 0;

// desired wheel output (go straight)
const double setpoint = 90; // may change to int, but not sure about concatenation issues

// initial position for testing (car is closer to right wall, needs to move LEFT)
const int lidar_L = 20;
const int lidar_R = 600;
const double delta = lidar_L - lidar_R;

// normalize delta to ±90 (estimating max difference is ±25-meters)
const double delta_90 = 90*(2*((delta + 2500)/(5000)) - 1);

// crawler output
double process_feedback;

// poll period in microseconds (will actually be ~0.2 seconds on car)
const unsigned int dt = 1000000;


// for testing purposes	
void setup() {
	// set initial crawler lidar input (±90 --> 0 to 180)
	process_feedback = 90 - delta_90;
	printf("Left distance from wall: %dcm\n", lidar_L);
	printf("Right distance from wall: %dcm\n", lidar_R);
	if (lidar_L > lidar_R) {
		printf("Starting car closer to right wall.\n");
	}
	else if ( lidar_L < lidar_R) {
		printf("Starting car closer to left wall.\n");
	}
	else {
		printf("Starting car evenly between walls.\n");
	}
	//printf("Initial crawler input: %f\n", process_feedback);
	if (process_feedback < 90) {
		printf("LEFT turn required from initial position in order to ");
		printf("move process_feedback toward setpoint.\n");
	}
	else if (process_feedback > 90) {
		printf("RIGHT turn required from initial position in order to ");
		printf("move process_feedback toward setpoint.\n");	
	}
	else if (process_feedback == 90) {
		printf("Going STRAIGHT already! (process_feedback == setpoint)\n");		
	}
	printf("Starting...\n\n");
}
	
	
void PID_Control(double previous_error, double error, double integral, 
				 double derivative, double output) {
	usleep(dt);	
	printf("setpoint: %f\n", setpoint);
	printf("process feedback: %f\n", process_feedback);
	error = setpoint - process_feedback;
	integral = integral + (error * dt);
	derivative = (error - previous_error) / dt;
	output = (Kp * error) + (Ki * integral) + (Kd * derivative);
	previous_error = error;
	printf("error: %f\n", error);
	//printf("integral: %d\n", integral);
	//printf("derivative: %d\n", derivative);
	printf("PID output: %f\n", output);  // for full output: %.20lf
	if ((output >= -0.000001) && (output <= 0.000001)){ // (supposedly bad practice to use == with floats,
		printf("Going STRAIGHT.\n");        // but when it shows -0.000000 it's actually less than 0) 
	}
	else if (output > 0.000001) {
		printf("Turning LEFT.\n");
	}
	else if (output < -0.000001) {
		printf("Turning RIGHT.\n");
	}
	//printf("previous_error: %d\n", previous_error);
	printf("\n");
	
	// artifical parameter so it doesn't think it corrected input perfectly in just 1 iteration
	double scaling = 0.99; // set to 1 for 1-iteration correction
	// input being changed according to output
	process_feedback += scaling * output;
}	

	

int main() {
	// initial conditions according to lidar test parameters
	setup();
	
	// pause
	unsigned int sleep_μS = 1250000;
	usleep(sleep_μS);
	
	// initial variables
	double previous_error = setpoint - process_feedback;
	double integral = 0;
	double error, derivative, output;
	
	// main loop
	while (1) 
		PID_Control(previous_error, error, integral, derivative, output);	
	
}
		


	
	
	
	
