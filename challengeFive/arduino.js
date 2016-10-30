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

app.get('/', function(req, res){
  res.sendFile('/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeFive/public/showdata.html');
});

app.use(express.static(__dirname + '/public'));

/* ----- Configure Serial Port -------- */

var portName = '/dev/cu.usbserial-AD01SSII',
portConfig = {
	baudRate: 9600,
};

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


/* -------- App Logic ----------*/

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);
sp.on("open", function (err) {

	var buffer;
	var info;
	var num = 0;
	var count = 0;
	str = "";

	// Turn on
	console.log("Sending LED on signal.");
	console.log("Ctrl+C to quit and send LED off signal.");


	io.on("connection",function(socket){
		
	  
		console.log('a user connected');

		sp.on('data',function(data){ //grab data from XBee 
			buffer = String(data).split('\n'); // break by new line
			console.log("This is XBee "+ data);
			//info = buffer[0].split(', '); // break into [ID, Data]
			//console.log(buffer[0]);

			/*if(info[0] === "l"){
				str = info[1]; // data from Left Lidar
				socket.emit('left', str);
			}

			if(info[0] === "r"){

				str = info[1]; // data from Right Lidar
				socket.emit('right', str);
			}

			if(info[0] === 'p'){

				str = info[1]; // data output from PID
				socket.emit('pid', str);
			}*/


		}); //end data conn
		
			
	}); //end io conn
}); // end open serial port