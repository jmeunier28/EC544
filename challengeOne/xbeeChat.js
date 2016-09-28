var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ON_DEATH = require('death');


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

  sp.on("open", function (err) {
    if (err) 
      return console.log('Error opening port: ', err.message);
    console.log('open');
    
    // INFO VARIABLES
    var connectedNodes = [];
    var buffer0;
    var nodeInfo; 
    var nodesArray = [[0,0,0]]; // remember that length is +1
    var numNodes = 0;
    
    // Cleanup when termination signals are sent to process
    ON_DEATH(function(signal, err) {
      var death_msg = "Q";
      sp.write(death_msg);
      sp.close();   
      console.log("\n\nSending reset signal to nodes.\n\n")
      process.exit();
    })
    
    
    // Check for and remove old nodes
    var CheckNodes = function() {
      var time = new Date();  
      var nodeCount = nodesArray.length - 1;
      for (var i = 1; i <= numNodes; i++) {
        var t = time.getTime();
        var timeDiff = t - nodesArray[i][2]; 
        if (timeDiff > 2200) {
            // remove node's subarray
            console.log("Removing stale node " + nodesArray[i][0])
            nodesArray.splice(i, 1);
            numNodes--;
            //console.log("numNodes: " + numNodes);
        }
      } }
    setInterval(function(){ CheckNodes() }, 600); 
    // 2100+300 (check) < 2500 (avg) > 2000 (xmit)
    // looks like these times work  

      var CalcAverage = function() {
      var tempSum = 0;
      var nodeCount = nodesArray.length - 1;
      for (var i = 1; i <= nodeCount; i++) {
        // Make sure value in temp index is a number
        if (!isNaN(parseFloat(nodesArray[i][1]))) {
          tempSum += parseFloat(nodesArray[i][1]);
        }
      }
      var tempAverage = tempSum / numNodes;
      //console.log("nodeCount: " + nodeCount);
      //console.log("tempSum: " + tempSum); 
      if (!isNaN(tempAverage)) {
        console.log("\nNode count: " + nodeCount + "   Average temperature: " + tempAverage.toFixed(2) + "F");
        io.emit('chat message',tempAverage);       
        for (var j = 1; j <= numNodes; j++) {
          console.log("  " + ("     " + nodesArray[j][0]).slice(-7) + " : " + nodesArray[j][1] + "F")   
        }
      }
      if (numNodes == 0) {
        console.log("No nodes currently connected. Waiting for connection . . .")
      } 
     
    }
    
    setInterval(function(){ CalcAverage() },2500); 
    
    
    // listen for data, grab temp & time, populate array
    sp.on('data', function(data) {
      sp.flush();
      var time = new Date();  
      buffer0 = data.split('\n'); // array of data till newline
      // Split again into either [#ID] or [#ID, Data] 
      nodeInfo = buffer0[0].split('   ');
      // Receiving good data ( ID or [ID, Data] )
      if (nodeInfo[0][0] == "#") { // could check up here for && no temp data too 
        
        // New node
        if (connectedNodes.indexOf(nodeInfo[0]) == -1) {
          CheckNodes();
          var dumNum = numNodes + 1;
          console.log("\n (" + dumNum + ") NEW CONNECTION: Sending handshake confirmation to: " + nodeInfo[0])
          connectedNodes.push(nodeInfo[0]);     
          sp.write(nodeInfo[0]);
        }
        
        // Preconnected node
        else if (connectedNodes.indexOf(nodeInfo[0]) != -1) {
          currentTime = time.getTime();
          //console.log("  Connected node: " + nodeInfo[0] + "   " + nodeInfo[1]);
      
          // If node has temp data: [ID, Temp]
          if (typeof nodeInfo[1] != 'undefined') {
        
            // Go through Nx3 array, see if this node ID is in nodesArray
            var foundAtIndex;
            for (var i = 0; i < nodesArray.length; i++)  {
              // If found ID
              if (nodesArray[i].indexOf(nodeInfo[0]) != -1) {
                foundAtIndex = i;
                break;
              } 
            }
            // Push [ID, Temp, Time] to nodesArray if ID doesn't exist within it
            if (typeof foundAtIndex === 'undefined') {
              numNodes++;
              //console.log("added to nodesArray") // debug print statement
              var idTempTime = [nodeInfo[0], nodeInfo[1], currentTime]; 
              nodesArray.push(idTempTime);
            } 
            // Update Temp, Time if ID already exists in nodesArray
            else {
              nodesArray[foundAtIndex][1] = nodeInfo[1];
              nodesArray[foundAtIndex][2] = currentTime;
              //console.log("  updated time/temp") // debug print statement
            }
          }
        } 
      }
      // Bad data coming in
      else if (nodeInfo[0][0] != "#") {
        // FOR DEBUGGING PURPOSES, DELETE LATER
        console.log("Corrupted node data. Push reconnect button if not already connected.")
      }
    // return the calulated average 
 
    }); // end data

  }); // end open
