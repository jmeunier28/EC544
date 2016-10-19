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

const token = process.env.particle_token;



/*----------- Connecting to Mongo --------------- */

var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');
var ObjectId = require('mongodb').ObjectID;
var url = 'mongodb://localhost:27017/ch4_Test';


particle.login({
    username: process.env.particle_username, 
    password: process.env.particle_password 
}).then(
  function(data){
    //console.log('API call completed on promise resolve');
    setTimeout(getDevs, 2000);
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

         console.log('Dev ID: ', getInfo.id)
         console.log('Dev ID: ', devName)

         particle.getEventStream({ name: 'indTemp', auth: token}).then(function(stream) {
  stream.on('event', function(data) {
   
   var curTmp = data.data;
   //console.log(curTmp);

   MongoClient.connect(url, function (err, db) {
  if (err) {
    console.log('Unable to connect to the mongoDB server. Error:', err);
  } else {
    
    //console.log('Connection established to: ', url);

    var document = {"Time": datetime, "Temp": curTmp};
    //console.log('Doc Format: ', document)

db.collection(devName).insert(document, function(err, records) {
            if (err) throw err;
            //console.log("Record added as "+records[0]._id);
            console.log(devName,": inserted into collection");
         });

//     Close connection
//db.close();
  }
});

   // console.log("Event: " + data.name + "   " + data.data);
  });
});



      }
      });
    
    setTimeout(getDevs, 2000);
  },
  function(err) {
    console.log('List devices call failed: ', err);
  }
  
);
