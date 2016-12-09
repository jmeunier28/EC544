/*
EC544 group 4
Challenge Six 
Read values from XBee
And locate object on map
*/

/* -------- Declare Dependices --------- */


var SerialPort = require('serialport');
var express = require('express');
app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ON_DEATH = require('death');
var PythonShell = require('python-shell');
var xbee_api = require('xbee-api');

app.get('/', function(req, res)
{
  res.sendFile('/home/pi/EC544/challengeEight/public/Default.html');

});

app.use(express.static(__dirname + '/public'));


/* ------------ Error Handling ---------------*/

//catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});

/* -----Run App on LocalHost Port 3000----- */

http.listen(3000, function(){
  console.log('listening on *:3000');
});

/* -----Configure Xbee and serial port----- */

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});


var portName = '/dev/ttyUSB0';
var usbPort = '/dev/ttyACM0';
var sampleDelay = 2000;


//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  	parser: XBeeAPI.rawParser()
};

usbPortConfig = {
	baudRate: 9600,
};

var sp;
var usb;
sp = new SerialPort.SerialPort(portName, portConfig);
usb = new SerialPort.SerialPort(usbPort, usbPortConfig);

var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: "test"
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

/* -------- App Logic ----------*/

var numSamples = 2;


console.log(' Running till each beacon gets at least', numSamples ,'rssi values to average.')

// variables
var sampleDelay = 2000;
var r1 = [];
var r2 = [];
var r3 = [];
var r4 = [];
var finalPoints = [];
r1Arr = [];
r2Arr = [];
r3Arr = [];
r4Arr = [];

//first script to set up model
options = {
	pythonOptions: ['-W', 'ignore'], //for supressing warnings
	scriptPath: '/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeEight',
};

//second script call to predict bin number on the fly
options2 = {
	pythonOptions: ['-W', 'ignore'], //supress warnings so it doesnt screw me up
	scriptPath: '/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeEight',
	args: [44.2, 59.1, 88.3, 100.9]
}

options3 = {
	pythonOptions: ['-W', 'ignore'],
	//pythonPath: '/Users/jmeunier28/anaconda/bin/python2.7',
	scriptPath: '/home/pi/EC544/challengeEight',
}

// //Call script to get a model 

// PythonShell.run('classifier.py',options,function(err, results){
// 	console.log("Calling python trainer and outputting model file");
// 	if (err) throw err;
// });


// sp.on("open", function () {
// 	console.log("We opened XBee serialport");

// 		//console.log("We opened USB serial comms");
// 		// start main function, set interval
// 		requestRSSI();
// 		setInterval(requestRSSI, sampleDelay);
// 		// input

// 		PythonShell.run('sendData.py', options3, function(err, results){
// 			if (err) throw err;
// 			console.log("Called sendData");
// 			console.log("Got back: " + results);

// 		});
// 		io.on("connection",function(socket)
// 		{
// 			console.log("user connection");

// 			XBeeAPI.on("frame_object", function(frame) {

// 				console.log("I'm getting data heyyy")
// 				if (frame.type == 144)
// 				{
// 					console.log("hey frame");
// 					// split input, sample number
// 					var beaconID = frame.data[1];
// 					var rssiVal = frame.data[0];
// 					// check if finished data collection	
// 					if ((r1.length >= numSamples) && (r2.length >= numSamples) && 
// 					    (r3.length >= numSamples) && (r4.length >= numSamples))
// 					{
// 					//if (r4.length >= numSamples) { // for testing, use above with all 4 beacons
// 						// Average each of the four beacons' values
// 						var r1avg = 0;
// 						for (var i = 0; i < r1.length; i++)
// 							r1avg += r1[i];
// 						r1avg /= r1.length;
// 						finalPoints.push(r1avg);
// 						r1Arr.push(r1avg);
// 						var r2avg = 0;
// 						for (var i = 0; i < r2.length; i++)
// 							r2avg += r2[i];
// 						r2avg /= r2.length;
// 						finalPoints.push(r2avg);
// 						r2Arr.push(r2avg);
// 						var r3avg = 0;
// 						for (var i = 0; i < r3.length; i++)
// 							r3avg += r3[i];
// 						r3avg /= r3.length;
// 						finalPoints.push(r3avg);
// 						r3Arr.push(r3avg);
// 						var r4avg = 0;
// 						for (var i = 0; i < r4.length; i++)
// 							r4avg += r4[i];
// 						r4avg /= r4.length;
// 						finalPoints.push(r4avg);
// 						r4Arr.push(r4avg);
				
// 						console.log('\nTraining point values and bin: ' + r1Arr[r1Arr.length-1] + r2Arr[r2Arr.length-1] + r3Arr[r2Arr.length-1] + r4Arr[r2Arr.length-1])
						
// 						options2['args'][0] = r1Arr[r2Arr.length-1];
// 						options2['args'][1] = r2Arr[r2Arr.length-1];
// 						options2['args'][2] = r3Arr[r3Arr.length-1];
// 						options2['args'][3] = r4Arr[r4Arr.length-1];
// 						console.log("\nOptions are: " + options2['args']);
// 						PythonShell.run('predict.py',options2,function(err, results){
// 							console.log("Calling python classifier");
// 							console.log("\nPoint is: \n" + options2['args']);
// 							if (err) throw err;

// 							console.log("I am in bin: " + results);
							
		  
// 								console.log('a user connected');
// 								socket.emit('binNumber', results);
					
							
							
// 							r1 = [];
// 							r2 = [];
// 							r3 = [];
// 							r4 = [];

// 						});

// 						console.log('Training point values and bin: ' + r1Arr[0] + r2Arr[0] + r3Arr[0] + r4Arr[0])

						
// 					}
// 					// populate beacon arrays up to sample number, error check, print data and totals
// 					else if ((rssiVal != 0) && (rssiVal != 255))
// 					{
// 						console.log("Trying to populate arrays with data\n");
// 						//console.log("  Beacon ID: " + beaconID + ", RSSI: " + rssiVal);
// 						//console.log('   Pushing value to beacon #', beaconID)

// 						console.log(r1Arr);

// 						if (beaconID == 1){
// 						r1.push(rssiVal);
// 						}
// 						else if(beaconID == 2){
// 						r2.push(rssiVal);
// 						}
// 						else if(beaconID == 3){
// 						r3.push(rssiVal);
// 						}
// 						else if(beaconID == 4){
// 						r4.push(rssiVal);
// 						}
// 						//This could be beacon of CAR????
// 						else if(beaconID == 5){ 
// 							//emit some msg idk what this will be yet
// 						}

// 						console.log("Totals::  r1:", r1.length, "  r2:", r2.length, "  r3:", r3.length, "  r4:", r4.length);

// 						//console.log("Totals::  r1:", r1.length, "  r2:", r2.length, "  r3:", r3.length, "  r4:", r4.length);

// 					}//end else if arrays arent full

// 					/*
// 						Need to implement some functionailty so that I can commmunicate with XBee that is on Car
// 						Need to know XBee's Beacon ID... so I can do two way communication 
// 						Also dont know if this funciton should be here 
// 					*/

// 					//Functions for controlling the car manually 



// 				}// end if frame data correct

// 			}); // End XBee API.on function 
// 		}); // end IO webpage connection
	
// }); // end Xbee port comms

result = []
usb.on('open', function(){
	console.log("Opened Serial Port USB comms");
		console.log("writing to serialport");


		io.on('connection', function(socket)
		{
		//	setInterval(function (){
		//	PythonShell.run('makepredict.py',options3,function(err, results){
		//		if(err){throw err;}
                  //              console.log("making prediction");
		//		results = results[0].replace(/\D/g,'');
		//		console.log("I am in bin " + results);
		//		//result.push('s');
		//		//console.log(result);
		//	});
//},1000);
			if (result.length > 0){
				console.log("writing to usb: " + result[0]);
				usb.write(result[0]);
				usb.on('data',function(data){
					var buff = new Buffer(data,"utf-8");
					var read = buff.toString('ascii');
					console.log(read);
				});
				result = []
			}

			socket.on('goLeft', function(string){
				console.log("Moving wheels 10 degrees to the left...\n");
				usb.write('<'); //send ASCII char l to arduino
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('wheelAngle', read); // send to client side
				});
			});// end go left
			socket.on('goRight', function(string){
				console.log("Moving wheels 10 degrees to the right...\n");
				usb.write('>'); // send ASCII char r to arduino
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('wheelAngle', read); // send to client side
				});
			});// end go right
			socket.on('increaseSpeed', function(string){
				console.log("Moving forward...\n");
				usb.write('U'); //send ASCII char f to arduino
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('carSpeed', read); // send to client side
				});
			});// end go increase speed
			socket.on('decreaseSpeed', function(string){
				console.log("Moving Backwards...\n");
				usb.write('D'); // send ASCII char b to arduino
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('carSpeed', read); // send to client side
				});
			});// end decrease speed
			socket.on('stopCar', function(string){
				console.log("Stopping...\n");
				usb.write('S'); // send ASCII char s to arduino
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('carSpeed', read); // send to client side
				});
			});// end stop car

			//Functions for starting/stopping manual control
			socket.on('startMan', function(string){
				console.log("Overriding Autonomy of car. starting manual control..\n");
				usb.write('T'); // send ASCII char m to arduino
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('carMode', read); // send to client side
				});
			});// end car mode

	});// end io conn
}); //end usb port comms