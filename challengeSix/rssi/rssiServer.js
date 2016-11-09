var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var http = require('http').Server(app);
var io = require('socket.io')(http);
var math = require('math');
var excel = require('exceljs');
//var workbook = new excel.Workbook();
//var sheet = workbook.addWorksheet('RSSI_Vals');
var workbook = createAndFillWorkbook();
workbook.xlsx.writeFile('RSSI_Vals')
    .then(function() {
        // done
    });
sheet.addRow()


var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var binNum = process.argv[3];

//var sampleDelay = 3000;
var sampleDelay = 2000;
var r1 = [];
var r2 = [];
var r3 = [];
var r4 = [];
var finalPoints = [];
//var points


/*----------- Getting Current Date + Time in Right Format --------------- */

function getFormattedDate() {
    var date = new Date();
    var str = (date.getMonth() + 1) + ":" + date.getDate() + ":" + date.getFullYear() + ":" +  date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
    return str;
}


/*----------- Connecting to Mongo --------------- */

var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');
var ObjectId = require('mongodb').ObjectID;
var url = 'mongodb://localhost:27017/chal6';



//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
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

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
  	//console.log(frame);
  	//for(var i =0; i < 10)
  	//while (r1.length)
    console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    var beconID = frame.data[1];
    var rssiVal = frame.data[0];

    if ((r1.length >= 10) && (r2.length >= 10) && (r3.length >= 10) && (r4.length >= 10)){

    	finalPoints.push(math.mean(r1));
    	finalPoints.push(math.mean(r2));
    	finalPoints.push(math.mean(r3));
    	finalPoints.push(math.mean(r4));
    	finalPoints.push(binNum);


    	MongoClient.connect(url, function(err, db) {
                            if (err) {
                                console.log('Unable to connect to the mongoDB server. Error:', err);
                            } else {

                                db.collection('beacon').insert({
                                    "Router_Number": frame.data[1],
                                    "Time": getFormattedDate(),
                                    "RSSI": frame.data[0],
                                    "Bin_Number": binNum,
                                }, function(err, records) {
                                    if (err) console.log("dups"); // err;

                                    // console.log(pho_ID, ": inserted into collection");
                                });

 }
                        }); }

    } 
    else {
    	if (beconID == 1){
    		r1.push(rssiVal);
    	}
    	else if(beconID == 2){
    		r2.push(rssiVal);
    	}
    	else if(beconID == 3){
    		r3.push(rssiVal);
    	}
    	else if(beconID == 4){
    		r4.push(rssiVal);
    	}
    //	}

    }
  
});