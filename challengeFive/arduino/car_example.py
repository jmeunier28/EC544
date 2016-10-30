import time

# basically just proportionality-control algorithm since Ki=Kd=0
Kp = 1
Ki = 0
Kd = 0

# desired output (go straight)
setpoint = 90

# initial position: left side of hallway (needs to turn RIGHT)
lidar_L = 200
lidar_R = 600
delta = lidar_L - lidar_R

# normalize from -1 to +1  (saying min/max is 25-meter delta)
delta_N = 2*((delta + 2500)/(5000)) - 1

# normalize from -90 to +90
delta_90 = 90 * delta_N

# set initial crawler lidar input (90 +- 90 = 0 to 180)
process_feedback = 90 - delta_90
print("Initial crawler input: ", process_feedback)
if (process_feedback < 90):
	print("(LEFT turn required from initial position", end="")
elif (process_feedback > 90):
	print("(RIGHT turn required from initial position", end="")
print(" in order to move process_feedback toward setpoint.)\n\n")	
time.sleep(2)


# poll period in seconds (will be ~0.2 on car)
dt = 2

# main control loop
previous_error = setpoint - process_feedback
integral = 0
while (1):
	time.sleep(dt)
	# print goal and actual output first
	print("setpoint: ", setpoint)
	print("process feedback: ", process_feedback)
	error = setpoint - process_feedback
	integral = integral + (error*dt)
	derivative = (error - previous_error)/dt
	Output = (Kp*error) + (Ki*integral) + (Kd*derivative)
	previous_error = error
	print("error:", error)
	#print("integral:", integral)
	#print("derivative: ", derivative)
	print("Output: ", Output)
	print("previous_error: ", previous_error)
	print("\n")
	# input being changed according to output
	process_feedback = process_feedback + Output 
	
		
	
	
	
	