#include <XBee.h>
#include <math.h>
#include <SoftwareSerial.h>


SoftwareSerial xbee(2,3); // Rx, Tx

// Initialize input pins
int red = 4;
int green = 5;
int blue = 6;
int switchState = 0;


// Global variables

boolean curLeader;
int nodeIdentity;
int myID;
int leaderID;
//int tempID;

// Time Variables

boolean timeout = false;
int electTime = 3; 
int isLeaderAlive = 3;
int leaderTimeout = 3;


// Intital Setup
void setup() {
  xbee.begin(9600);
  Serial.begin(9600);
  curLeader = false;

  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(8, INPUT);
  digitalWrite(green,LOW);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);

  nodeIdentity = getNodeId();
  myID = nodeIdentity;
  leaderID = -1;
  Serial.println("My ID : "+ String(nodeIdentity));
  xbee.flush();
  Serial.println("Setup Complete");
}

int getNodeId() {
  String s;

  // accessing command mode..... 1sec silence, 1sec '+++' then 1sec silence
  delay(1000);  
  xbee.write("+++");
  delay(1000);    
  xbee.write("\r");
  delay(100);

  // On entering the command mode, the module sends ‘OK\r’ 
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");

  // Get the preset Node IDs
  xbee.write("ATNI\r");
  delay(100);
  while (xbee.available() > 0) {
      s += char(xbee.read());
      Serial.println("TESTING!!!!: "+ String(s));
  }
  delay(100);

  // Exit configuration mode
  xbee.write("ATCN\r");
  delay(1000);

  // Flush Serial Buffer Before Start
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");
  delay(100);
  
  Serial.println("My id is: " + s);
  return s.toInt();
}

// Broadcasting a Message
void broadcastMsg(int curr){

  xbee.print("ID: " + String(curr) + "\n");
  Serial.print(char(xbee.read()));
  Serial.println("My FUCKING ID: " + String(curr));
  }



// Leader Election
void electLeader{
  
  
  }

// Loop Function
void loop(){

getNodeId();
broadcastMsg(myID);
}
