var express = require('express');
app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);


app.get('/', function(req, res) {
    res.sendFile('/Users/damiOr/Documents/Grad_School/ec544/git/challengeEight/Code/testing/Default.html');
});

app.use(express.static(__dirname));


http.listen(4000, function() {
    console.log('listening on *:4000');
    console.log("hey");
});



io.on("connection", function(socket) {
    console.log("you whatup");

});