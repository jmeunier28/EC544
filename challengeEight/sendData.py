'''

Send manual chars to arduino bc for some reason stupid
nodejs wont let me access two different serialports at a time

'''

import sys
import argparse
import serial
import struct

parser = argparse.ArgumentParser()
parser.add_argument("in", help="signal Beacon one")
args = vars(parser.parse_args())

input = args.get("in",None)

def sendData(input):
	#open up serial port
	ser = serial.Serial('/dev/cu.usbmodem1411', 9600) # double check that this is correct
	byte = str.encode(input) #encode to send 
	print(byte)
	ser.write(byte) #send to arduino
	#num = struct.unpack(">L", ser.read())[0]
	num = ser.read()
	print(num)

sendData(input)

