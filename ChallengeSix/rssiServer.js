/* Produces 1 training point in ~10 seconds
Run this code 5-10 times at unique locations within each cluster.
Set argv[2] to the cluster number for current training point.
When finished, copy produced .csv file into new .xlsx file for use with matlab classifier.
*/

// requires
var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require('fs');

// xbee
var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
api_mode: 2
});
var port = '/dev/cu.usbserial-AD01SSIK'
var portName = port

// cluster/bin
var binNum = process.argv[2];
var numSamples = 3; // 10
console.log('\nTraining point for Bin #', binNum)
console.log(' Running till each beacon gets at least', numSamples ,'rssi values to average.')

// variables
var sampleDelay = 2000;
var r1 = [];
var r2 = [];
var r3 = [];
var r4 = [];
var finalPoints = [];

// serial port
portConfig = {
baudRate: 9600,
parser: XBeeAPI.rawParser()
};
var sp;
sp = new SerialPort(portName, portConfig);

// create a packet to be sent to all other XBEE units on the PAN.
var RSSIRequestPacket = {
type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
destination64: "000000000000ffff",
broadcastRadius: 0x01,
options: 0x00,
data: "test" // value doesn't matter, not used
}

// main function
var requestRSSI = function(){
sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

// loop
sp.on("open", function () {
	// start main function, set interval
	requestRSSI();
	setInterval(requestRSSI, sampleDelay);
	// input
	XBeeAPI.on("frame_object", function(frame) {
		if (frame.type == 144){
			// split input, sample number
			var beaconID = frame.data[1];
			var rssiVal = frame.data[0];
			// check if finished data collection	
			if ((r1.length >= numSamples) && (r2.length >= numSamples) && 
			    (r3.length >= numSamples) && (r4.length >= numSamples)){
			//if (r4.length >= numSamples) { // for testing, use above with all 4 beacons
				// Average each of the four beacons' values
				var r1avg = 0;
				for (var i = 0; i < r1.length; i++)
					r1avg += r1[i];
				r1avg /= r1.length;
				finalPoints.push(r1avg);
				var r2avg = 0;
				for (var i = 0; i < r2.length; i++)
					r2avg += r2[i];
				r2avg /= r2.length;
				finalPoints.push(r2avg);
				var r3avg = 0;
				for (var i = 0; i < r3.length; i++)
					r3avg += r3[i];
				r3avg /= r3.length;
				finalPoints.push(r3avg);
				var r4avg = 0;
				for (var i = 0; i < r4.length; i++)
					r4avg += r4[i];
				r4avg /= r4.length;
				finalPoints.push(r4avg);
				// add cluster number to end for matlab code
				finalPoints.push(binNum);
				// print		
				console.log('  Training point values and bin: ', finalPoints)
				// write to file and exit
				fs.appendFile('rssi_values.csv', finalPoints + '\n', 'utf8', function(err) {
					if (err) {
						console.log('error occurred writing to file.\n')
					}
					else {
						console.log('  Appended new training point to file. Ready to rerun.\n')
						process.exit();
					}
				});
				
			}
			// populate beacon arrays up to sample number, error check, print data and totals
			else if ((rssiVal != 0) && (rssiVal != 255)){
				console.log("  Beacon ID: " + beaconID + ", RSSI: " + rssiVal);
				console.log('   Pushing value to beacon #', beaconID)
				if (beaconID == 1){
				r1.push(rssiVal);
				}
				else if(beaconID == 2){
				r2.push(rssiVal);
				}
				else if(beaconID == 3){
				r3.push(rssiVal);
				}
				else if(beaconID == 4){
				r4.push(rssiVal);
				}
				console.log("    Totals::  r1:", r1.length, "  r2:", r2.length, "  r3:", r3.length, "  r4:", r4.length)
			}
		}
	});
});
