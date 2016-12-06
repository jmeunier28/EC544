'''
Reads model from joblib and then can predict 
where car is based on RSSI input point from 
XBees. This script is called eacha time data is avaliable 
in the server

Sends bin number prediction from RPi ---> Arduino
via a serial connection

'''


from sklearn.neighbors import KNeighborsClassifier, NearestNeighbors
import pandas as pd
import numpy as np
import sys
import argparse
from sklearn.externals import joblib
import serial

parser = argparse.ArgumentParser()
parser.add_argument("b1", help="signal Beacon one", type = float)
parser.add_argument("b2", help="signal Beacon two", type = float)
parser.add_argument("b3", help="signal Beacon three", type = float)
parser.add_argument("b4", help="signal Beacon four", type = float)
args = vars(parser.parse_args())

b1 = args.get("b1",None)
b2 = args.get("b2",None)
b3 = args.get("b3",None)
b4 = args.get("b4",None)

def sendData(bin):
	#open up serial port
	ser = serial.Serial('dev/cu.usbmodem1411', 9600) # double check that this is correct
	bin.encode('utf-8') #encode to send 
	ser.write(bin) #send to arduino


def predict(point):
	nbrs = joblib.load('data.pkl')
	bin = nbrs.predict(point)
	return bin



point = np.array([b1,b2,b3,b4])
point = point.reshape(1, -1)
bin = predict(point)
#sendData(bin) #send the bin number to the ARduino through USB
print(bin)


