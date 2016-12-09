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
  res.sendFile('/Users/jmeunier28/Desktop/EC544/challenges/EC544/challengeEight/public/nearBy.html');

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


var usbPort = '/dev/cu.usbmodem1411';
var sampleDelay = 2000;


usbPortConfig = {
	baudRate: 115200,
	parser: SerialPort.parsers.readline('\n')

};

var usb;
usb = new SerialPort.SerialPort(usbPort, usbPortConfig);

/* -------- App Logic ----------*/

/*
				usb.on('data',function(data){
					console.log("Read back");
					buff = new Buffer(data, "utf-8")
					var read = buff.toString('ascii')
					//console.log(buff);
					console.log(read);
					socket.emit('carSpeed', read); // send to client side
				});
				*/



usb.on('open', function(){
	console.log("Opened Serial Port USB comms");
		console.log("writing to serialport");


		io.on('connection', function(socket)
		{

			  usb.on('data', function(data)
			  {
      			//console.log('key returned: ' + data.toString());
      			var buffer = data.toString.split(",");
				console.log(buffer);
				var speed = buffer[0];
				var bin = buffer[1];
				var mode = buffer[2];
				var wheel_angle = buffer[3];
				console.log("speed " + speed + ", bin " + bin + ", mode " + mode + ", wheel angle " + wheel_angle);
				socket.emit('carMode', mode);
				socket.emit('wheelAngle', wheel_angle);
				socket.emit('binNumber', bin);
				socket.emit('carSpeed', speed);

  			});


			socket.on('goLeft', function(string){
				console.log("Moving wheels 10 degrees to the left...\n");
				usb.write('<'); //send ASCII char l to arduino

			});// end go left
			socket.on('goRight', function(string){
				console.log("Moving wheels 10 degrees to the right...\n");
				usb.write('>'); // send ASCII char r to arduino

			});// end go right
			socket.on('increaseSpeed', function(string){
				console.log("Moving forward...\n");
				usb.write('U'); //send ASCII char f to arduino

			});// end go increase speed
			socket.on('decreaseSpeed', function(string){
				console.log("Moving Backwards...\n");
				usb.write('D'); // send ASCII char b to arduino
			});// end decrease speed
			socket.on('stopCar', function(string){
				console.log("Stopping...\n");
				usb.write('S'); // send ASCII char s to arduino

			});// end stop car

			//Functions for starting/stopping manual control
			socket.on('startMan', function(string){
				console.log("Overriding Autonomy of car. starting manual control..\n");
				usb.write('T'); // send ASCII char m to arduino

			});// end car mode

	});// end io conn
}); //end usb port comms



