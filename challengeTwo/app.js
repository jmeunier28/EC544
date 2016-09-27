var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var mongo = require('mongodb');
var monk = require('monk');
var db = monk('localhost:27017/challengeTwo');

app.use(function(req,res,next){
    req.db = db;
    next();
});

var routes = require('./routes/index');
var users = require('./routes/users');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('/users', users);

// catch 404 and forward to error handler
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


module.exports = app;

var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};


var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

/*app.get('/', function(req, res){
  res.sendfile('index.html');
});*/

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

//code purposely commented out will add back later
/*var info_object = function(){
  this.id;
  this.info = [];
}*/

sp.on("open", function () {
  console.log('open');

  var tempArray = [0,0,0,0];
  //var tempArray = [];
  var names = []; //names will be assigned to each arduino 
  var countArray = [0,0,0,0];
  //var countArray = [];
  var times = [];
  var sensor_array = [];

  sp.on('data', function(data) {
    var flag = false;
    var time = new Date();

    times.push(time.getTime()); //add to time array
    //console.log(data); // log incoming data from XBee
    message = data.split(','); //separate data
    //console.log(message);

    //code purposely commented out... would give ability to scale to many devices

    /*if (sensor_array.length === 0){

        var whatup = new info_object();
        whatup.id = message[0];
        whatup.info.push(message[1]);
        console.log(message[0]);
        sensor_array.push(whatup);
        console.log("Creating initial ID");

    }

    for(var i=0; i<sensor_array.length; i++){

      if(sensor_array[i].id=== String(message[0])){
        console.log(sensor_array[i].id);
        sensor_array[i].info.push(message[1]);
        flag = true;
        console.log("I already have " + sensor_array[i].id + " and I'm adding to it");
      }
    }

    if (flag===false){
        console.log("Creating new id object...");
        var heythere = new info_object();
        heythere.id = message[0];
        heythere.info.push(message[1]);
        sensor_array.push(heythere);
      }
    
    
    for (var i = 0; i < sensor_array.length-1;i++){
      names.push(sensor_array[i]['id']);
      tempArray.push(sensor_array[i]['info'].pop());
    }
    console.log("This is temp array " + tempArray);*/


   if(message[0]==='A'){

      names.push(message[0]);
      tempArray[0]= parseInt(message[1]);
      countArray[0]++;

     //console.log(TempArray[0]);
   }

   if(message[0]==='B'){

      names.push(message[0]);
      tempArray[1] = parseInt(message[1]);
      countArray[1]++;

   }

   if(message[0]==='C'){

      names.push(message[0]);
      tempArray[2] = parseInt(message[1]);
      countArray[2]++;

   }

   if(message[0]==='D'){

      names.push(message[0]);
      tempArray[3] = parseInt(message[1]);
      countArray[3]++;

   }

   // get all 4 entries and sum them together after the array 
   // of times has 4 entries 

  //var timeDiff = times[times.length - 1] - times[0];
    var error = false;
    var count = 0;
  
var calc_average = function(){

   
  if (tempArray.length === 4){

    var counter = 0;
    var sum = 0;
    //console.log("counting average...");


      for (var i = 0; i < tempArray.length;i++){
        sum += tempArray[i]; //add each temperature together 
        counter++;
      }
    }

    if (error){
      var average = (sum/(counter - count)).toFixed(2);
    }
    else{
      var average = (sum/counter).toFixed(2);
    }

    
  //console.log("Data reveived from " + names + ", and the average temperature is "+ average +"*C. ");

      var avgString = average;
      
      console.log(avgString);
      io.emit('chat message',avgString);
    }

  setInterval(function(){ calc_average() },2000);


  for (var i = 0; i < countArray.length;i++){
    if (countArray[i]=== 0 ){
      console.log("Error Sending Data From Sensor Number " + i);
      var errmsg = "Error Sending Data From Sensor Number " + i + "...Correcting Average";
      count++
      //io.emit('chat message',errmsg);
    }
  }
  if (count > 0){
    error = true;
    console.log("Adjusting Average...");
  }
    //code purposely commented out... calc temp avg of device ids that have been added
    //dynamically... not functioning at this time
    /*var counter = 0;
    var sum = 0;
    var calc_avg = function(){
    //console.log("temp array is "+ tempArray);


      for (var i = 0; i < tempArray.length;i++){
      console.log("im computing the average");
      sum += tempArray[i]; //add each temperature together 
      console.log("this is the sum" + sum);
      counter++; }

      var average = (sum/counter).toFixed(2);
      console.log("Data reveived from " + names + ", and the average temperature is "+ average +"*C. ");
      var avgString = "Data reveived from " + names + ", and the average temperature is "+ average +"*C";
      io.emit(avgString); 
    }
    setInterval(function(){calc_avg()}, 1 * 2000);*/
  });
});

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


module.exports = app;
