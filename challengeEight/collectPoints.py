'''

Script to collect points to train on based on 
BU wifi access point signal strength
Will write all points to a csv file to use for training

command to get access points is: sudo iwlist wlan0 scanning | egrep 'Cell |Quality|ESSID'

'''


import re
import os
import sys
import subprocess

results = []

def getData():
	try:
		proc = subprocess.Popen(['sudo', 'iwlist', 'wlan0', 'scanning', '|', 'egrep', 'Cell', '|Quality|ESSID'],stdout = subprocess.PIPE)
	except OSError, ValueError:
		print ("Could not query for data on command line")

	out, err = proc.communicate()
	lines = re.split("\n", str(out))
	for line in lines:
		if "Quality=" in line:
			output = re.split("=", line)
			results.append(output[1]) #add quality signal strength 
			print output[1]

	return results


print("\n\nSo we got these values for training:")
print(getData())