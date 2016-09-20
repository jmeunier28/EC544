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



var info_object = function(){
  this.id;
  this.info = [];
}


sp.on("open", function () {
  console.log('open');

  var tempArray = [0,0,0,0];
  var names = []; //names will be assigned to each arduino 
  var countArray = [0,0,0,0];
  var times = [];
  var sensor_array = [];
  var flag= false;

  sp.on('data', function(data) {
    var time = new Date();

    times.push(time.getTime()); //add to time array
    console.log(data); // log incoming data from XBee

  //setInterval(function(){
    
    message = data.split(','); //separate data

    if (sensor_array.length == 0){

        var whatup = new info_object();
        whatup.id = message[0];
        whatup.info.push(message[1]);
        console.log(message[0]);
        sensor_array.push(whatup);
        console.log("Creating initial ID");

    }

    for(var i=0; i<sensor_array.length; i++){

      if(sensor_array[i].id== String(message[0])){
        console.log(sensor_array[i].id);
        sensor_array[i].info.push(message[1]);
        flag = true;
        console.log("I already have " + sensor_array[i].id + " and I'm adding to it");
      }
    }

    if (flag==false){
        console.log("Creating new id object...");
        var heythere = new info_object();
        heythere.id = message[0];
        heythere.info.push(message[1]);
        sensor_array.push(heythere);
      }
    
    
    for (var i = 0; i < sensor_array.length-1;i++){
      names.push(sensor_array[i]['id']);
      tempArray[i] = sensor_array[i]['info'].pop();

    }
    // console.log("This is temp array " + tempArray);

   /*if(message[0]=='A'){

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

   }*/

   // get all 4 entries and sum them together after the array 
   // of times has 4 entries   

    var counter = 0;
    var sum = 0;
    console.log("temp array is "+ tempArray);


      for (var i = 0; i < tempArray.length;i++){
      console.log("im computing the average");
      sum += tempArray[i]; //add each temperature together 
      counter++; }

      var average = (sum/counter).toFixed(2);
      console.log("Data reveived from " + names + ", and the average temperature is "+ average +"*C. ");
      var avgString = "Data reveived from " + names + ", and the average temperature is "+ average +"*C";
      io.emit(avgString); 

      //tempArray = [0,0,0,0];

    for (var i =0; i < tempArray.length; i++){
      console.log("Individual Sensor " + names[i] + "Value: " + tempArray[i]);
      var individualTemp = "Individual Sensor " + names[i] + "Value: " + tempArray[i];
      io.emit(individualTemp);
    }

    for (var i = 0; i < countArray.length;i++){
      if (countArray[i]== 0 ){
        console.log("Error Sending Data From Sensor Number " + i);
        var errmsg = "Error Sending Data From Sensor Number " + i;
        io.emit(errmsg);
      }
    }

    //}, 2000);
  });
});
