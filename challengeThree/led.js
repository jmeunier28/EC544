var SerialPort = require('serialport');
var express = require('express');
app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ON_DEATH = require('death');

var portName = '/dev/ttyUSB0',
portConfig = {
	baudRate: 9600,
};

var sp;
sp = new SerialPort(portName, portConfig);
sp.on("open", function (err) {	
	// Turn on
	console.log("Sending LED on signal.");
	console.log("Ctrl+C to quit and send LED off signal.");
	sp.write('1');
	
	// Turn off on program exit
	ON_DEATH(function(signal, err) {
		var death_msg = 'Q';
		sp.write(death_msg);
		sp.close();   
		console.log("\n\nSending reset signal to nodes.\n\n")
		process.exit();
	})
}); 

