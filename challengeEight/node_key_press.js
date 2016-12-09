var SerialPort = require("serialport");

//portName = '/dev/ttyACM0'; // uncomment this port for RPi
portName = '/dev/tty.usbmodem1411'; // port on my computer
portConfig = {
        baudRate: 115200,
        parser: SerialPort.parsers.readline('\n')
};

sp = new SerialPort.SerialPort(portName, portConfig);

// for getting input from keyboard
var stdin = process.stdin;
// without this, we would only get streams once enter is pressed
stdin.setRawMode( true );

// resume stdin in the parent process (node app won't quit all by itself
// unless an error or process.exit() happens)
stdin.resume();

// i don't want binary, do you?
stdin.setEncoding( 'utf8' );

// on any data into stdin
stdin.on( 'data', function( key ){
  // ctrl-c ( end of text )
  if ( key === '\u0003' ) {
    process.exit();
  }
  // write the key to the arduino
  sendKey(key);
});

// function for sending serial data to arduino
var sendKey =   function(key) {
      console.log('Writing key: ' + key);
       sp.write(key);
}

// on serial port open
sp.on('open', function(){
  console.log('Serial Port Opend');

  // this function receives messages back from arduino and prints them out.
  // right now show just echo back character sent
  sp.on('data', function(data){
      console.log('key returned: ' + data.toString());
  });
});
