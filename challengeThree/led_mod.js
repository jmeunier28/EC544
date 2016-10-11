/*

EC544 group 4

Challenge Three

Remote Access

*/

/* -------- Declare Dependices --------- */


var SerialPort = require('serialport');
var express = require('express');
app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ON_DEATH = require('death');

app.get('/', function(req, res){
  res.sendFile('/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeThree/public/button.html');
});

app.use(express.static(__dirname + '/public'));

/* ----- Configure Serial Port -------- */

var portName = '/dev/cu.Bluetooth-Incoming-Port',
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

	// Turn on
	console.log("Sending LED on signal.");
	console.log("Ctrl+C to quit and send LED off signal.");


	io.on("connection",function(socket){
	  
	  console.log('a user connected');
	  
	  socket.on('redbuttonPress',function(string){
	  		console.log("Red Button Press");
	  		var str = "You Turned ON The Red LED";
	  		socket.emit('msg', str);
			sp.write('1'); //red button pressed write 1 
			}); //end red

	  socket.on('greenbuttonPress',function(string){
	  		console.log("Green Button Press");
	  		var str = "You Turned ON The Green LED";
	  		socket.emit('msg', str);
			sp.write('2'); //green button pressed write 1 
			}); // end green

	  socket.on('bluebuttonPress',function(string){
	  		console.log("Blue Button Press");
	  		var str = "You Turned ON The Blue LED";
	  		socket.emit('msg', str);
			sp.write('3'); //blue button pressed write 1 
			}); // end blue


	  socket.on('killbuttonPress',function(string){
	  		console.log("Kill Button Press");
	  		var str = "You Turned Off The LED";
	  		socket.emit('msg', str);
			// Turn off on program exit
			ON_DEATH(function(signal, err) {
				var death_msg = 'Q';
				sp.write(death_msg);
				sp.close();   
				console.log("\n\nSending reset signal to nodes.\n\n")
				process.exit();
			}); //end ondeath 
		}); // end kill button
			
	}); //end io conn
}); // end open serial port

