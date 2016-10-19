/*

EC544 Group 4

Challenge 4

Temperature Data on Cloud

*/

/* ----------- Declare Dependencies ------------ */

//var SerialPort = require("serialport");
var express = require('express');
app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ON_DEATH = require('death');
var Particle = require('particle-api-js');
var particle = new Particle();

var dev1, dev2, dev3, dev4;

var datetime = new Date();

//const deviceId = this.get('deviceId');
const token = process.env.particle_token;

//console.log(deviceId);


/*----------- Connecting to Mongo --------------- */

var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');
var ObjectId = require('mongodb').ObjectID;
var url = 'mongodb://localhost:27017/ch4_Test';

MongoClient.connect(url, function (err, db) {
  if (err) {
    console.log('Unable to connect to the mongoDB server. Error:', err);
  } else {
    
    console.log('Connection established to', url);


    //Close connection
    db.close();
  }
});

particle.login({
    username: process.env.particle_username, 
    password: process.env.particle_password 
}).then(
  function(data){
    console.log('API call completed on promise resolve');
    //setTimeout(getDevs, 2000);
  },
  function(err) {
    console.log('API call completed on promise fail: ', err);
  }
);


var getDevs = particle.listDevices({ auth: token }).then(
  function(devices){

    devices.body.forEach(function(getInfo){
      if(getInfo.connected){
        var devName = getInfo.name;

        // console.log('Dev ID: ', getInfo.id)
        // console.log('Dev ID: ', devName)

         particle.getVariable({ deviceId: getInfo.id, name: 'tempFer', auth: token }).then(function(data) {
  var currentTemp = data.body.result;
  console.log('Dev ID: ', devName)
  console.log('Temp: ', currentTemp)
  console.log('\n ')

}, function(err) {
  console.log('An error occurred while getting attrs:', err);
});

      }
      //var testName = devices.body;
      //console.log('TEST: ', testName);

    })
    //console.log('TEST: ', hello);

    //console.log('Devices: ', devices);
     // dev1 = devices.body[0].name;
     // dev2 = devices.body[1].name;
     // dev3 = devices.body[2].name;
     // dev4 = devices.body[3].name;
    // console.log('ID 1:', dev1);
    // console.log('ID 2:', dev2);
    // console.log('ID 3:', dev3);
    // console.log('ID 4:', dev4);


//     particle.getVariable({ deviceId: hello, name: 'tempFer', auth: token }).then(function(data) {
//   console.log('Device variable retrieved successfully:', data);

// }, function(err) {
//   console.log('An error occurred while getting attrs:', err);
// });
    setTimeout(devicesPr, 2000);
  },
  function(err) {
    console.log('List devices call failed: ', err);
  }
  
);




//Particle.variable("tempFer", &temp_K, DOUBLE);

