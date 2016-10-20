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


/* ----------- HTML Pages ------------ */

app.get('/', function(req, res) {
    res.sendFile('/Users/damiOr/Documents/Grad_School/ec544/git/challengeFour/final/public/index.html');
});
app.get('/current', function(req, res) {
    res.sendFile('/Users/damiOr/Documents/Grad_School/ec544/git/challengeFour/final/public/Current-Reading.html');
});
app.get('/historic', function(req, res) {
    res.sendFile('/Users/damiOr/Documents/Grad_School/ec544/git/challengeFour/final/public/Historic-View.html');
});

app.use(express.static(__dirname + '/public'));


/* ----------- global vars ------------ */

const token = process.env.particle_token;

var curTmp;
var pho_ID;
var document;
var devName;
var devCount = 0;

/*----------- Getting Current Date + Time in Right Format --------------- */

function getFormattedDate() {
    var date = new Date();
    var str = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
    return str;
}


/* ------------ Error Handling ---------------*/

//catch 404 and forward to error handler
app.use(function(req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use(function(err, req, res, next) {
        res.status(err.status || 500);
        res.render('error', {
            message: err.message,
            error: err
        });
    });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
        message: err.message,
        error: {}
    });
});



/* -----Run App on LocalHost Port 3000----- */

http.listen(3000, function() {
    console.log('listening on *:3000');
});

ON_DEATH(function(signal, err) {
        //var death_msg = "Q";
        //sp.write(death_msg);
        //sp.close();   
        console.log("\n\nSending reset signal to nodes.\n\n")
        //process.exit();
    })
    /*----------- Connecting to Mongo --------------- */

var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');
var ObjectId = require('mongodb').ObjectID;
var url = 'mongodb://localhost:27017/chal4';

/*----------- Login Particle Account--------------- */

particle.login({
    username: process.env.particle_username,
    password: process.env.particle_password
}).then(
    function(data) {
        //console.log('API call completed on promise resolve');
        setTimeout(getDevs, 2000);
    },
    function(err) {
        console.log('API call completed on promise fail: ', err);
    }
);

/*----------- List Current Active --------------- */

var getDevs = particle.listDevices({
    auth: token
}).then(
    function(devices) {

        devices.body.forEach(function(getInfo) {
            if (getInfo.connected) {
                devCount = devCount + 1;
                devName = getInfo.name;
                console.log('NUMBER of Devs:', devCount);
                console.log('Connected: ', devName)

                particle.getEventStream({
                    name: 'grp4_Temp',
                    auth: token
                }).then(function(stream) {
                    stream.on('event', function(data) {

                        curTmp = data.data;
                        pho_ID = data.coreid;

                        var numTemp = parseFloat(curTmp);
                        //console.log('TESTING', numTemp)


                        /*----------- Add to DB --------------- */

                        MongoClient.connect(url, function(err, db) {
                            if (err) {
                                console.log('Unable to connect to the mongoDB server. Error:', err);
                            } else {

                                db.collection('ch4_Temps').insert({
                                    "Dev_ID": pho_ID,
                                    "Time": getFormattedDate(),
                                    "Temp": numTemp
                                }, function(err, records) {
                                    if (err) console.log(); // err;

                                    // console.log(pho_ID, ": inserted into collection");
                                });

                                calAvg(db, function() {
                                    //db.close();
                                });
                            }
                        });

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

/*----------- Calculate Average --------------- */

var calAvg = function(db, callback) {

    db.collection('ch4_Temps').aggregate([{
        $group: {
            _id: "$Time",
            avgTemp: {
                $avg: "$Temp"
            }
        }
    }, {
        $out: "AverageTemp"
    }], function(err, result) {
        //console.log('TESTING', result)
        //db.close();
        //end of aggregate
    });
}

io.on("connection", function(socket) {

    console.log('a user connected');

    socket.on('buttonPress', function(string) {

        console.log("button pressed drawing graph..");
        MongoClient.connect(url, function(err, db) {
            //console.log("made mongo conn");
            var str = null;
            var cursor = db.collection('AverageTemp').find({});
            cursor.each(function(err, doc) { //get each data point in DB
                assert.equal(err, null);
                if (doc != null) {

                    str = [doc._id, doc.avgTemp];
                    console.log("not null " + str);
                    socket.broadcast.emit('msg', str);
                    socket.emit('msg', str);

                } else {
                    console.log(str);
                    str = null;
                    //socket.emit('msg',str);
                    console.log("null" + str);

                    //callback();
                }

            });

        }); //end mongo client

    }); //end socket.on
}); // end io.on