var SerialPort = require('serialport');
var ON_DEATH = require('death');
var prompt = require('prompt');

// serial setup
var portName = '/dev/cu.usbmodem1411',
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};
var sp;
sp = new SerialPort.SerialPort(portName, portConfig);
var stop = '0';

sp.on("open", function (err) {
	if (err) 
		return console.log('Error opening port: ', err.message);
	function send()
	{
		sp.write('l');
		console.log("Wrote l");

	}
	sp.on('data', function(data){
		console.log(data);
	});

	setInterval(send, 2000);

}); 


/*
			//Functions for controlling the car manually 

			socket.on('goLeft', function(string){
				console.log("Moving wheels 10 degrees to the left...\n");
				sp.write('l'); //send ASCII char l to arduino
				//then emit some OK message back to client
			});
			socket.on('goRight', function(string){
				console.log("Moving wheels 10 degrees to the right...\n");
				sp.write('y'); // send ASCII char r to arduino
				//then emit some OK message to client
			});
			socket.on('goForward', function(string){
				console.log("Moving forward...\n");
				sp.write('f'); //send ASCII char f to arduino
				//then emit some OK message back to client
			});
			socket.on('goBack', function(string){
				console.log("Moving Backwards...\n");
				sp.write('b'); // send ASCII char b to arduino
				//then emit some OK message to client
			});
			socket.on('stopCar', function(string){
				console.log("Stopping...\n");
				sp.write('s'); // send ASCII char s to arduino
				//then emit some OK message to client
			});

			//Functions for starting/stopping manual control
			socket.on('startMan', function(string){
				console.log("Overriding Autonomy of car. starting manual control..\n");
				sp.write('m'); // send ASCII char m to arduino
				//then emit some OK message to client
			});
			socket.on('stopMan', function(string){
				console.log("Overriding manual drive... Autonomous control..\n");
				sp.write('a') // Send ASCII char a to arduino
				//then emit some OK message to client
			});

			*/