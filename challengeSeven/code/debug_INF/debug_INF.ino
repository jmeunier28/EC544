#include <XBee.h>
#include <math.h>
#include <SoftwareSerial.h>

// Define Baud Rate here
#define BAUD 9600
// Create an xBee object
SoftwareSerial xbee(2, 3); // Rx, Tx

// Global Variables
boolean isLeader;
int leaderID;
int final_id;
int countPresses = 0;
int myIdentity;

// Initialize Time Variables
boolean timeoutFlag = false;
int timeCount = 0;
int electionTimeLength = 3;
int verifyLeaderTimeout = 5;
int leaderTimeout = 1;
int verifyLeaderTimer = 0;
int elecTimer = 0;
int leader_timer = 0;


// Initialize input pins
int RED = 4;
int GREEN = 5;
int BLUE = 6;
int BUTTON = 8;
int switchState = 0;


// Deboucning Test
long lastDebounceTime = 0;
long debounceDelay = 50;


// Get the 'NI: ' value for node range from 1-4
int getNodeId() {
  String s;

  // Enter configuration mode - Should return "OK" when successful.
  delay(1000);    // MUST BE 1000
  xbee.write("+++");
  delay(1000);    // MUST BE 1000
  xbee.write("\r");
  delay(100);

  // Get the OK and clear it out.
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");

  // Send "ATNI" command to get the NI value of xBee.
  xbee.write("ATNI\r");
  delay(100);
  while (xbee.available() > 0) {
    s += char(xbee.read());
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

  return s.toInt();
}


// How to respond to the messages we get from other nodes
void processResponse() {
  if (xbee.available()) {
    String msg = readRecievedMsg();
    String info = msg.substring(msg.indexOf(':') + 1);
    Serial.println("INFO:  " + info);
    int sendCure = msg.indexOf('C');
    int sendInfect = msg.indexOf('I');
    int id = msg.substring(0, msg.indexOf(':')).toInt();

    // Widespread Infection
    if (sendInfect == 0) {
      infect();
    }

    //Widespread Cure
    if (sendCure == 0) {
      cure();
    }

    if (info == "Leader") {
      isLeader = false;
      digitalWrite(BLUE, LOW);
      verifyLeaderTimer = 0;
      if (id == leaderID) {
        Serial.println("leader is alive");
        //        verifyLeaderTimer = 0;
      } else {
        election(id);
      }
    }

  } else {
    checkLeader();
  }
}

// Cure infected nodes
void cure() {
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
}

// Infect non-leader nodes
void infect() {
  if (!isLeader) {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
  }
}

void setup() {
  xbee.begin(BAUD);
  Serial.begin(BAUD);
  isLeader = false;

  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(GREEN, HIGH);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);


  myIdentity = getNodeId();
  final_id = myIdentity;
  leaderID = -1;
  xbee.flush();
  Serial.println("Setup Complete");
}

String readRecievedMsg() {
  String msg  = "";
  while (xbee.available() > 0) {
    char c = char(xbee.read());
    if (c == '\n') {
      break;
    }
    msg += c;
  }
  Serial.println(msg);
  return msg;
}

//rebroadcast leader id
void broadcastMsg(int id) {
  xbee.print(String(id) + ":Leader\n");
}

void leaderBroadcast() {
  xbee.print(String(myIdentity) + ":Leader\n");
  Serial.println("Current Leader:" + String(leaderID) + "\n");
  digitalWrite(BLUE, HIGH);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);

  isLeader = true;
}


void election(int id) {
  Serial.println("Election in progress...");
  if (checkElectionTimeOut()) {
    Serial.println("election over");
    return;
  }
  leaderID = -1;
  if (id > final_id) {
    final_id = id;
    elecTimer = 0;
    broadcastMsg(final_id);
    Serial.println("new node higher than me...\n");
  } else {
    if (elecTimer >= electionTimeLength) {
      elecTimer = 0;
      timeCount = 0;
      timeoutFlag = true;
      leaderID = final_id;
      final_id = myIdentity;
    } else {
      elecTimer++;
      broadcastMsg(final_id);
    }
  }
}

boolean checkElectionTimeOut() {
  if (timeoutFlag) {
    if (timeCount < 2) {
      timeCount++;
    } else {
      timeoutFlag = false;
      timeCount = 0;
    }
  }
  return timeoutFlag;
}

void checkLeader() {
  if (leaderID == myIdentity) {
    if (leader_timer >= leaderTimeout) {
      leader_timer = 0;
      leaderBroadcast();
    } else {
      leader_timer++;
    }
  } else if (verifyLeaderTimer >= verifyLeaderTimeout) {
    verifyLeaderTimer = 0;
    broadcastMsg(myIdentity);
    Serial.println("Leader " + String(leaderID) + " is dead.");
    leaderID = -1;
  } else {
    if (leaderID == -1) {
      verifyLeaderTimer = 0;
      if (elecTimer < electionTimeLength) {
        broadcastMsg(final_id);
        elecTimer++;
      } else {
        elecTimer = 0;
        leaderID = final_id;
        final_id = myIdentity;
      }
    } else {
      verifyLeaderTimer++;
    }
  }
}


void loop() {
  switchState = digitalRead(BUTTON);
  if (switchState == 1 ) {
    if ( (millis() - lastDebounceTime) > debounceDelay) {
      if (leaderID == myIdentity) {

        // Leader Logic
        // if button pushed clear infections

        xbee.println("Cure\n");
       // digitalWrite(RED, LOW);
        lastDebounceTime = millis();

      }
      else {

        // Follower Logic
        // if button pushed send out mass infections

        if (digitalRead(BUTTON) == HIGH) {
          countPresses += 1;
          Serial.println("HIGH!!!");
          if (countPresses == 1) {
            digitalWrite(GREEN, LOW);
            digitalWrite(RED, HIGH);
            digitalWrite(BLUE, LOW);
            xbee.println("Inf\n");
            lastDebounceTime = millis();
            countPresses = 0;
          }
        }
      }
    }
  }
  processResponse();
  delay(1000);
}

