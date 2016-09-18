var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);



var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};


var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

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

let msg = new Set();

sp.on("open", function () {
  console.log('open');

  var tempArray = [0,0,0,0];
  var names = []; //names will be assigned to each arduino 
  var countArray = [0,0,0,0];
  var times = [];

  sp.on('data', function(data) {
    var time = new Date();

    times.push(time.getTime()); //add to time array
    console.log(data); // log incoming data from XBee
    message = data.split(','); //separate data
    //console.log(message);

    //message = msg.split(',');
    var length = message.length; //array length == num of sensors
    console.log("the lenght of message is " + length);

    /*for (var i = 0; i < message.length -1; i++){
      console.log("hello bitch");
      msg.add(message[0]);
      console.log("the message is " + message[0]);
      console.log(msg[message[0]]);
      console.log(msg);
      if (msg[message[0]]){
        console.log("in dis bitch");
        names.push(message[1]);
        tempArray[i] = parseInt(message[1]);
        countArray[i]++;
        console.log(tempArray);
      }
    }*/



    if(message[0]=='A'){
      names.push(message[0]);
      tempArray[0]= parseInt(message[1]);
      countArray[0]++;
     //console.log(TempArray[0]);
   }
   if(message[0]=='B'){
      names.push(message[0]);
      tempArray[1] = parseInt(message[1]);
      countArray[1]++;
   }
   if(message[0]=='C'){
      names.push(message[0]);
      tempArray[2] = parseInt(message[1]);
      countArray[2]++;
   }
   if(message[0]=='D'){
      names.push(message[0]);
      tempArray[3] = parseInt(message[1]);
      countArray[3]++;
   }

   // get all 4 entries and sum them together after the array 
   // of times has 4 entries 

  //var timeDiff = times[times.length - 1] - times[0]; 
  if (tempArray.length == 4){

    var counter = 0;
    var sum = 0;
    console.log("im in this loop!!!!!!!");

    for (var i = 0; i < tempArray.length;i++){
      sum += tempArray[i]; //add each temperature together 
      counter++;
    }
  }

  var average = (sum/counter).toFixed(2);
  console.log("Data reveived from," + names + " and the average temperature is "+ average +"*C. ");
  var avgString = "Data reveived from " + names + ", and the average temperature is "+ average +"*C";
  io.emit(avgString);

  for (var i =0; i < tempArray. length; i++){
    console.log("Individual Sensor " + names[i] + "Value: " + tempArray[i]);
    var individualTemp = "Individual Sensor " + names[i] + "Value: " + tempArray[i];
    io.emit(individualTemp)
  }

  for (var i = 0; i < countArray.length;i++){
    if (countArray[i] == 0 ){
      console.log("Error Sending Data From Sensor Number " + i);
      var errmsg = "Error Sending Data From Sensor Number " + i;
      io.emit(errmsg);
    }
  }
  });
});

