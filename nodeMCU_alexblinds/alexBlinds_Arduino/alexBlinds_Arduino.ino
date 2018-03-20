// TO DO:
//
// - Add heartbeat status update for MQTT that includes uptime
// - Write a method for converting strings to byte messages, and vice versa
// - Save parameters (e.g. motor location) to memory and load them on restart
// - Make sure target_position is published to both on button press and when changed over MQTT
// - Add something to detect millis() wrap-around (every couple of days?) and correct time checking accordingly
// - Add a wear-levelling algorithm to where currentPosition is stored in EEPROM

// PubSubClient tutorial
// https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/


#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

// Current EEPROM address
int addr = 0;

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

// Other parameters
const int buttonPin = D5; // Push button
const int ledPin1 = D0;   // LED on the NodeMCU board
const int ledPin2 = D4;   // LED on the ESP8266 chip
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long lastHeartbeat = 0;  // the last time the heartbeat message was posted

// Initialize Wi-FI and MQTT
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
bool mqtt_status = false; // For recording publish status

// Instantiate variables for monitoring and controlling status
int currentPosition = 0; // Current position of the blinds
int targetPosition = 0; // Desired position of the blinds
int stepsToMove = 0;      // variable for holding how many steps to move the motor
int buttonState = HIGH;         // variable for reading the pushbutton status
int lastButtonState = HIGH;

void setup() {
  // initialize the serial port:
  Serial.begin(115200);
  Serial.println();
  // initialize the EEPROM
  EEPROM.begin(512);
  
  // Set up the LED on the ESP chip for blinking
  pinMode(ledPin2, OUTPUT);

  // Load from EEPROM the address of (also in EEPROM) the currentPosition variable
  // The program automatically moves around where currentPosition is stored
  // to prevent writing to the same part of memory over and over, wearing it out
  // (the ESP8266 EEPROM apparently has a life of ~100,000 cycles).
  EEPROM.get(0, addr);
  // If the read address is out of range, reset to minimum value.
  if (addr >= (512 - sizeof(int) - 1)) {
    addr = 0 + sizeof(int);
    Serial.print("ERROR: current memory address out of range.  Resetting.");
    EEPROM.put(0, addr);
  }
  Serial.print("Current EEPROM address: ");
  Serial.println(addr);

  // Read currentPosition from memory
  EEPROM.get(addr, currentPosition);
  targetPosition = currentPosition;
  Serial.print("(read from memory) currentPosition = ");
  Serial.println(currentPosition);

  // Connect to Wi-Fi
  setup_wifi();

  // Set MQTT connections
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // set the stepper speed:
  myStepper.setSpeed(stepperRPM);

  // General debugging printouts
  Serial.println("Waiting 2 seconds");
  delay(2000);
  Serial.print("Current position: ");
  Serial.println(currentPosition);
  Serial.print("Target position: ");
  Serial.println(targetPosition);
  Serial.print("Number of steps to move: ");
  Serial.println(targetPosition - currentPosition);
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
  
  // Alex's attempt to cast the byte array into an into an int
  int x = 0;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    // Byte-shifts from ASCII character mapping to 0-9
    // WARNING: Makes very big mistake if the string is not made up of 0-9 only!
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

  // Post a heartbeat message, if it's time to do so
  if ((millis() - lastHeartbeat) / 1000 > heartbeatInterval) {
    lastHeartbeat = millis();
    client.publish(topic_status, "still alive");
  }
  
  // BUTTON READ AND DEBOUNCING
  // --------------------------
  
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from HIGH to LOW), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  // If the debounce test passes, do something
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // BUTTON CONFIRMED PRESSED
      // Pressing the button pulls the digital input LOW
      if (buttonState == LOW) {
        // Turn on the LED
        digitalWrite(ledPin2, LOW);
        // Set the position of the blinds at random
        targetPosition = (int) random(0,200);
        // Write to EEPROM
        EEPROM.put(addr, targetPosition);
        EEPROM.commit();
        Serial.print("BUTTON PRESS.  Go to position: ");
        Serial.println(targetPosition);
        // Publish the new targetPosition to MQTT
        mqtt_status = client.publish(topic_position, static_cast<char*>(static_cast<void*>(&targetPosition)));
      }

      // BUTTON CONFIRMED RELEASED
      else if (buttonState == HIGH) {
        // Turn off the LED
        digitalWrite(ledPin2, HIGH);
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  // THE REST
  // --------
  
  // Calculates how many steps are needed to move the motor into
  // the target position
  stepsToMove = targetPosition - currentPosition;

  if (stepsToMove != 0) {
    // Move a single step in the required direction
    move(stepsToMove / abs(stepsToMove));
    currentPosition += (stepsToMove / abs(stepsToMove));
    // Print to terminal
    Serial.print("currentPosition: ");
    Serial.println(currentPosition);
  }
}
