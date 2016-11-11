/*
EC544 group 4
Challenge Five 
Read values from XBee
Drive Car in Straight Line
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

app.get('/', function(req, res){
  res.sendFile('/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeFive/public/showdata.html');
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

var portName = '/dev/cu.usbserial-AD01SSII',
var sampleDelay = 3000;


//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

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

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
  }
});


io.on("connection",function(socket){
	
  
	console.log('a user connected');
	var data_point = [12, 22, 3, 12.6];
	//var train = 1;

	var options = {
		pythonOptions: ['-W', 'ignore'], //for supressing warnings
		scriptPath: '/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeSix',
		args: [data_point[0], data_point[1], data_point[2], data_point[3]]//, train]
		};

	PythonShell.run('classifier.py', options, function (err, results) {
		if (err) throw err;
		// results is an array consisting of messages collected during execution
		console.log('results: %j', results);
		socket.emit('left', results);
		});

	// pyshell.on('message', function (message) {
	// // received a message sent from the Python script (a simple "print" statement) 
	// console.log(message);
	// //var update = message;
	// socket.emit('update', message);

	// // end the input stream and allow the process to exit 
	// pyshell.end(function (err) {

	// if (err) throw err;
	//     console.log('finished');
	//   });
		
			
}); //end io conn
