#include <Arduino.h>
#include "AlexButton.h"

// Initialize a new AlexButton 
// connected to digital pin D5
AlexButton button(D5);

// Set the pins of the two LEDs
const int ledPin1 = D0;   // LED on the NodeMCU board
const int ledPin2 = D4;   // LED on the ESP8266 chip

void setup() {
  // Initialize the serial port
  Serial.begin(115200);
  delay(500);
  Serial.println();
    
  Serial.println("Welcome to the button pressing test program");
  
  // Set up the LEDs and turn them off.
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  
  delay(500);
  Serial.println("...Ready...");
}

void loop() {
  // Do something if the button status has changed
  if(button.update() == true) {
    int x = button.status();
    // Short press.  Turn on LED 1.
    if (x == 1) {
      digitalWrite(ledPin1, LOW);
      Serial.println("Short press");
    }
    // Long press. Turn on LED 2.
    else if (x == 2) {
      digitalWrite(ledPin2, LOW);
      Serial.println("Long press");
    }
    // Short press release.  Turn off LEDs.
    else if (x == -1) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      Serial.println("Short press release");
    }
    // Long press release.  Turn off LEDs.
    else if (x == -2) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      Serial.println("Long press release");
    }
  }
}
