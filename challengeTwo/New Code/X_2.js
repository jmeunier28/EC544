var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ON_DEATH = require('death');

// Set up serial port 
var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};
var sp = new SerialPort(portName, portConfig); // got rid of deprecation issue with sp4

// Respond with file when a GET request is made to the homepage: app.METHOD(PATH, HANDLER) '/'=homepage
app.get('/', function(req, res){
	res.sendfile('index.html');
});				

// Listen on the connection event for incoming sockets, and log it to the console.
io.on('connection', function(socket){
	console.log('a user connected');
	socket.on('disconnect', function(){
	});
	socket.on('chat message', function(msg){
		io.emit('chat message', msg);
		sp.write(msg + "\n");
	});
});

// Make the http server listen on port 3000
http.listen(3000, function(){
	console.log('listening on *:3000');
});

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//sp.flush(function(err,results){});
sp.on("open", function (err) {
	if (err) 
		return console.log('Error opening port: ', err.message);
	console.log('open');
	
	
	// INFO VARIABLES
	var nodeCount = 0;
	var pendingNodes = [];
	var connectedNodes = [];
	var buffer0;
	var nodeInfo;
	
	
	// Cleanup when termination signals are sent to process
	ON_DEATH(function(signal, err) {
		var death_msg = "Q";
		sp.write(death_msg);
		// zero out all variables
		pendingNodes = [];
		connectedNodes = [];
		buffer0 = [];
		nodeInfo = [];

		console.log("\n\nSending reset signal to nodes.\n\n")
		sp.flush(function(err,results){});
		process.exit();
	})
	
	// listen for data, grab time, delimit
	sp.on('data', function(data) {
	var time = new Date();
	buffer0 = data.split('\n'); // array of data till newline
	// Split again into either "#ID" into [#ID] (if new broadcast), or "#ID Data" into [#ID, Data] (preconnected node)
	nodeInfo = buffer0[0].split('   ');
	//console.log("     nodeInfo: " + nodeInfo);
	
	// DEBUGGING PRINT -- DELETE LATER
	//console.log("     pendingNodes: " + pendingNodes + "     connectedNodes: " + connectedNodes);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Receiving ID or [ID, Data] -- Testing all possible cases
	
	// if first char 
	if (nodeInfo[0][0] == "#") { // could check up here for && no temp data too 
		
		// Brand new node (ID not in pendingNodes or connectedNodes)
		if ((pendingNodes.indexOf(nodeInfo[0]) == -1) && (connectedNodes.indexOf(nodeInfo[0]) == -1)) {
			console.log("Brand new node: " + nodeInfo[0])
			pendingNodes.push(nodeInfo[0]);
			
		}
		
		
		// Pending node (ID in pendingNodes, but not in connectedNodes)
		else if ((pendingNodes.indexOf(nodeInfo[0]) != -1) && ((connectedNodes.indexOf(nodeInfo[0]) == -1))) {
			console.log("  Pending node: " + nodeInfo[0])
			
			// send back ID to confirm handshake
			sp.write(nodeInfo[0]);
			// Remove from pendingNodes
			var index = pendingNodes.indexOf(nodeInfo[0]);
			if (index > -1) {
				pendingNodes.splice(index, 1);
			}
			// Add to connectedNodes
			//var 
			connectedNodes.push(nodeInfo[0]);
			
			// ^^^^ add as touple instead, with temp data spot
			
			
			
			
			
		}
		// Connected node (ID in connectedNodes, but not in pendingNodes)
		else if  ((pendingNodes.indexOf(nodeInfo[0]) == -1) && (connectedNodes.indexOf(nodeInfo[0]) != -1)) {
			console.log("    Connected node: " + nodeInfo[0] + "   " + nodeInfo[1]);
	
			// Disconnect nodes with undefined temperatures to force restart
			if (nodeInfo[1] == 'undefined') {
				console.log("Undefined data for node: " + nodeInfo[0] + ". Dropping connection to this node.");
				var index = connectedNodes.indexOf(nodeInfo[0]);
				if (index > -1) {
					connectedNodes.splice(index, 1);
				}
				
			}
			
			//var t = time.getSeconds();
			//var t = time.getTime();
			
			
			
	
			// FIRST: fix bug -- temp showing as undefined			
			
			// Now:
			// Store data
			// Store connectedNode count
			// Average data
			// Detect drop-offs and remove nodes
			
			
			// **make sure all Arduino and Node data is zeroed out properly on quit**
			
			
		}
		// Error (ID in both pendingNodes and connectedNodes [should not happen])
		else {
			console.log("Error: node exists in both pending and connected.")
			
		}
		
		

		
	}
	else if (nodeInfo[0][0] != "#") {
		
		console.log("Error 2: incoming data does not start with '#'.")
		
	}
	
	
	
	var CalcAverage = function() {
		console.log("CalcAverage: * every 1 second(s) *");
	}
	
	var CheckNodes = function() {
		console.log("CheckNodes: * every 6 second(s) *");
	}
	
	
	
	
	//setInterval(function(){ CalcAverage() },2000);
	
	//setInterval(function(){ CheckNodes() },6000);
	
	
	
	/* NEED:
	// possibly function that runs every 10 seconds and clears stale connectedNodes elements
	If havent received data from connected node in a while, clear that node (use array of time objects)
	
	Or if temp is undefined (not sure when this is yet)
	
	*/
		
	}); // end data
	
	
}); // end open