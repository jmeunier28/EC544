var express = require('express');
app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var testing = 'M';

app.get('/', function(req, res) {
    res.sendFile('/Users/damiOr/Documents/Grad_School/ec544/git/challengeEight/Code/public/resize.html');
});

app.use(express.static(__dirname));


http.listen(4000, function() {
    console.log('listening on *:4000');
    console.log("hey");
});



//io.emit('wheelAngle', '90');

//io.emit('carSpeed', '70mps');

io.on("connection", function(socket) {
    console.log("you whatup");



    socket.emit('carMode',testing);

    socket.emit('wheelAngle', '90');

    socket.emit('carSpeed', '70');

    for (var i = 0; i <= 10000; i++) {
        if ((i%1000) === 0){
            socket.emit('carMode',testing);
        }
        else{

            socket.emit('carMode','A');
        }

    }

socket.on('goLeft', function(string){
                console.log("Moving wheels 10 degrees to the left...\n");
                //sp.write('l'); //send ASCII char l to arduino
                //then emit some OK message back to client
            });
            socket.on('goRight', function(string){
                console.log("Moving wheels 10 degrees to the right...\n");
                //sp.write('y'); // send ASCII char r to arduino
                //then emit some OK message to client
            });
            socket.on('increaseSpeed', function(string){
                console.log("Moving forward...\n");
                //sp.write('f'); //send ASCII char f to arduino
                //then emit some OK message back to client
            });
            socket.on('decreaseSpeed', function(string){
                console.log("Moving Backwards...\n");
                //sp.write('b'); // send ASCII char b to arduino
                //then emit some OK message to client
            });
            socket.on('stopCar', function(string){
                console.log("Stopping...\n");
                // sp.write('s'); // send ASCII char s to arduino
                //then emit some OK message to client
            });

            //Functions for starting/stopping manual control
            socket.on('startMan', function(string){
                console.log("Overriding Autonomy of car. starting manual control..\n");
                // sp.write('m'); // send ASCII char m to arduino
                //then emit some OK message to client
            });
            socket.on('stopMan', function(string){
                console.log("Overriding manual drive... Autonomous control..\n");
                // sp.write('a') // Send ASCII char a to arduino
                //then emit some OK message to client
            });

            });