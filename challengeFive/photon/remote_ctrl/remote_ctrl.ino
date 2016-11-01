

TCPClient client;


unsigned int myChannelNumber = 172815;
const char * myWriteAPIKey = "KD2E4ZEG5996CJNY";
const char * name = "Dev1";

int control_IN = D0;


////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////

// Setup
void setup() {
 
 pinMode(control_IN, OUTPUT);
 Particle.function("transmission", ToggleTrans);
 //we can access the value of the Temp from the cloud
  Particle.variable("trans_ctrl", control_IN);
 digitalWrite(control_IN, LOW);
// Particle.publish("PowerON","Dev1",60,PRIVATE);
 
}

// MAIN LOOP
void loop()
{

}

int ToggleTrans(String command) {
    

    if (command=="on") {
        digitalWrite(control_IN,HIGH);
        //digitalWrite(led2,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(control_IN,LOW);
        //digitalWrite(led2,LOW);
        return 0;
    }
    else {
        return -1;
    }
}

