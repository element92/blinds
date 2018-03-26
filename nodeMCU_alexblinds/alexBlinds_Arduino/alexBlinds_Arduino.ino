// TO DO:
//
// - Add heartbeat status update for MQTT that includes uptime
// - Write a method for converting strings to byte messages, and vice versa
// - Make sure target_position is published to both on button press and when changed over MQTT
// - Add something to detect millis() wrap-around (every couple of days?) and correct time checking accordingly

// PubSubClient tutorial
// https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/

#include <Arduino.h>
#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

#include "AlexButton.h"
#include "EEPROM_int.h"

// Wi-Fi parameters
const char* ssid = "AccessibleBeige";
const char* password = "crafternooner";

// MQTT parameters
const char* mqtt_server = "m14.cloudmqtt.com";
const int mqtt_port = 19658;
const char* mqtt_user_id = "lwklhoog";
const char* mqtt_pwd = "c5iYiGyizaaa";
const char* device_id = "blinds_bedroom";
const int heartbeatInterval = 60; // seconds between heartbeat posts to "status" topic
// MQTT topics
const char* topic_status = "status";
const char* topic_set = "set";
const char* topic_position = "position";

// Stepper motor parameters
const int stepsPerRevolution = 200; // Depends on motor
const int stepperRPM = 8;
// Motor on NodeMCU pins D0, D1, D2, D3
Stepper myStepper(stepsPerRevolution, D0, D1, D2, D3);

// Initialize a new AlexButton 
// connected to digital pin D5
AlexButton button(D5);

// Blinds status and mode parameters
int operatingMode;  // -1 = startup/error
                    //  0 = normal
                    //  1 = calibration

// Other parameters
const int ledPin1 = D0;   // LED on the NodeMCU board
const int ledPin2 = D4;   // LED on the ESP8266 chip
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastHeartbeat = 0;  // the last time the heartbeat message was posted
unsigned long lastEEPROMWrite = 0; // the last time the parameters were saved to EEPROM
const int EEPROMWriteInterval = 5; // seconds between EEPROM writes (if there are changes only)


// Initialize Wi-FI and MQTT
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
bool mqtt_status = false; // For recording publish status

// Instantiate variables for monitoring and controlling status
int targetPosition; // Desired position of the blinds
int stepsToMove;      // variable for holding how many steps to move the motor
int calDirection;   // -1 = go up
                    //  0 = go nowhere
                    //  1 = go down

// Instantiate variable to be stored in EEPROM
EEPROM_int currentPosition(0x00, 0x1ef); // The current position of the blinds
                                         // e.g. number of motor steps from the top
EEPROM_int maxPosition(0x1f0, 0x1ff); // The maximum allowed position of the blinds 
                                // e.g. number of motor steps from the top
                                // corresponding to full blinds extention

void setup() {
  // initialize the serial port:
  Serial.begin(115200);
  // Connect to Wi-Fi
  setup_wifi();
  // initialize the EEPROM
  EEPROM.begin(512);
  // Load values from EEPROM
  currentPosition.retrieve();
  maxPosition.retrieve();
  // Give it some time to breathe
  delay(500);
  Serial.println();

  Serial.println("---------------------");
  Serial.println("Welcome to AlexBlinds");
  Serial.println("---------------------");

  // Put operatingMode into startup
  operatingMode = -1;

  // Set the calibration direction to 0 (go nowhere)
  calDirection = 0;
  
  // Set up the LEDs and turn them off.
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);

  // Set targetPosition to currentPosition
  stepsToMove = 0;
  targetPosition = currentPosition.get();

  // ONLY ENABLE ONCE, TO HARD-LOAD A MAX POSITION INTO EEPROM
  // Set maxPosition
  // RESPLACE WITH CALIBRATION ROUTINE
  //maxPosition.set(200);
  //maxPosition.save();
  
  // Print the current values loaded from memory
  Serial.print("(read from memory) currentPosition = ");
  Serial.println(currentPosition.get());
  Serial.print("(read from memory) maxPosition = ");
  Serial.println(maxPosition.get());

  // Set MQTT connections
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // set the stepper motor speed:
  myStepper.setSpeed(stepperRPM);

  // Time to breathe
  delay(500);  
  operatingMode = 0; // Normal mode
  Serial.println("...Ready...");
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Code for what happens if we receive something
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Alex's attempt to cast the byte array into an int
  int x = 0;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    // Byte-shifts from ASCII character mapping to 0-9
    // WARNING: Makes very big mistake if the string is not made up of 0-9 only!
    // i.e. it can't handle negative numbers
    x = (x * 10) + ((int)payload[i] - (int)'0');
  }
  Serial.println();
  Serial.print("Integer version of the input: ");
  Serial.println(x);
  // Set the target position to the input
  targetPosition = x;
  Serial.print("MQTT COMMAND.  Go to position: ");
  Serial.println(targetPosition);
  // Publish the new targetPosition to MQTT
  mqtt_status = client.publish(topic_position, static_cast<char*>(static_cast<void*>(&targetPosition)));
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(device_id, mqtt_user_id, mqtt_pwd)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic_status, "Rollerblind connected/reconnected to MQTT");
      // ... and resubscribe
      client.subscribe(topic_set);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Moves the motor
// If steps is positive, move clockwise
// If steps is negative, moes counter-clockwise
void move(int steps) {
  int dir = 1;
  if (steps < 0) {dir = -1;}
  for(int i=0; i<abs(steps); ++i){
    myStepper.step(dir);
    delay(1); // Gives the ESP8266 time to do its thing
  }
}

void loop() {

  // MQTT CLIENT
  // -----------
  
  // Check MQTT connection
  if (!client.connected()) {
    Serial.println("No MQTT connection. Reconnecting.");
    reconnect();
  }
  client.loop();

  // SCHEDULED TASKS
  // ---------------
  
  // Post a heartbeat message, if it's time to do so
  if ((millis() - lastHeartbeat) / 1000 > heartbeatInterval) {
    Serial.println("...Heartbeat...");
    lastHeartbeat = millis();
    client.publish(topic_status, "still alive");
  }

  // Save currentPosition to EEPROM, if it's time to do so
  if ((millis() - lastEEPROMWrite) / 1000 > EEPROMWriteInterval) {
//    Serial.println("...EEPROM save attempt...");
    lastEEPROMWrite = millis();
    currentPosition.save();
  }

  // PUSH BUTTON
  // -----------
    // Do something if the button status has changed
  if(button.update() == true) {
    int x = button.status();
    
    // Short press.  Turn on LED 1.
    if (x == 1) {
      digitalWrite(ledPin1, LOW);
      Serial.println("BUTTON: Short press");
    }
    
    // Long press. Turn on LED 2.
    else if (x == 2) {
      digitalWrite(ledPin2, LOW);
      Serial.println("BUTTON: Long press");
    }
    
    // Short press release
    // Lower the blinds to the bottom.  
    // If they're already lowered, raise them instead.
    // Turn off LEDs.
    // If we're in calibration mode, do the calibration routine instead.
    else if (x == -1) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      Serial.println("BUTTON: Short press release");
      if (operatingMode == 0) { // Normal mode
        if (currentPosition.get() < maxPosition.get() ){ // If blinds aren't fully down, go down
          Serial.println("GOING DOWN");
          targetPosition = maxPosition.get();
        }
        else { // If the blinds are already fully down, go up
          Serial.println("GOING UP");
          targetPosition = 0;
        }
      } // end "normal" operating mode
      else if (operatingMode == 1) { // calibration mode
        if (calDirection == 0) { // start calibration
          calDirection = 1;
        }
        else if (calDirection == 1) { // record max position and reverse direction
          maxPosition.set(currentPosition.get() );
          calDirection = -1;
        }
        else { // shift the zero point to the new calibration and save
          maxPosition.set(maxPosition.get() - currentPosition.get() );
          maxPosition.save();
          currentPosition.set(0);
          currentPosition.save();
          calDirection = 0;
          targetPosition = 0;
          operatingMode = 0;
        }
      } // end of "calibration" mode
    } // end "short press"
    
    // Long press release.  Turn off LEDs and start calibration routine.
    else if (x == -2) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      Serial.println("BUTTON: Long press release");
      Serial.println("...CALIBRATING...");
      // INSERT CALIBRATION ROUTINE
      operatingMode = 1; // Put the device into calibration mode
      calDirection = 1; // Start by calibrating in the down direction
      // OVERRIDE - randomize the position
      //Serial.println("...OVERRIDE: going to random position instead.");
      // // Go to a random position between 0-200
      //targetPosition = (int) random(0,maxPosition.get() );
      //Serial.print("...MOVING to position ");
      //Serial.println(targetPosition);
     // // Publish the new targetPosition to MQTT
     // mqtt_status = client.publish(topic_position, static_cast<char*>(static_cast<void*>(&targetPosition)));
    }
  }

  // CALIBRATION ROUTINE
  // -------------------
  if (operatingMode == 1) {
    // During calibration, advances one step at a time indefinitely,
    // until the button is pressed again.
    targetPosition = currentPosition.get() + calDirection;
  }

  // THE REST
  // --------
  
  // Calculates how many steps are needed to move the motor into
  // the target position
  stepsToMove = targetPosition - currentPosition.get();

  if (stepsToMove != 0) {
    // Move a single step in the required direction
    move(stepsToMove / abs(stepsToMove));
    currentPosition.set(currentPosition.get() + (stepsToMove / abs(stepsToMove)));
    // Print to terminal
    Serial.print("currentPosition: ");
    Serial.println(currentPosition.get());
  }
}
