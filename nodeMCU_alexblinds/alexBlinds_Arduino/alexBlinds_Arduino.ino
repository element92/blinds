#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi parameters
const char* ssid = "AccessibleBeige";
const char* password = "crafternooner";

// MQTT parameters
const char* mqtt_server = "m14.cloudmqtt.com";
const int mqtt_port = 19658;
const char* mqtt_user_id = "lwklhoog";
const char* mqtt_pwd = "c5iYiGyizaaa";
const char* device_id = "blinds_bedroom";

// Initialize stepper motor on NodeMCU pins D0, D1, D2, D3
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
const int stepperRPM = 8;
Stepper myStepper(stepsPerRevolution, D0, D1, D2, D3);

// Initialize Wi-FI and MQTT
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
bool mqtt_status = false; // For recording publish status

void setup() {
  // initialize the serial port:
  Serial.begin(115200);

  // Connect to Wi-Fi
  setup_wifi();

  // Set MQTT connections
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // set the stepper speed:
  myStepper.setSpeed(stepperRPM);
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
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(device_id, mqtt_user_id, mqtt_pwd)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  // Check MQTT connection
  if (!client.connected()) {
    Serial.println("No MQTT connection. Reconnecting.");
    reconnect();
  }
  client.loop();

  // step three revolutions in one direction:
  Serial.println("clockwise");
  mqtt_status = client.publish("outTopic", "Clockwise");
  Serial.print("MQTT publish status:");
  Serial.println(mqtt_status);
  Serial.print("MQTT client state: ");
  Serial.println(client.state());
  for(int i=0; i<stepsPerRevolution*3; ++i){
    myStepper.step(1);
    delay(1); // Gives the ESP8266 time to do its thing
  }
  
  delay(500);

  // step three revolution in the other direction:
  Serial.println("counterclockwise");
  mqtt_status = client.publish("outTopic", "Counter-clockwise");
  Serial.print("MQTT publish status:");
  Serial.println(mqtt_status);
  Serial.print("MQTT client state: ");
  Serial.println(client.state());
  for(int i=0; i<stepsPerRevolution*3; ++i){
    myStepper.step(-1);
    delay(1); // Gives the ESP8266 time to do its thing
  }
  
  delay(500);  

}
