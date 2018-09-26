#include <Arduino.h>
#include "EasyDriver.h"

// Initialize a stepper motor
EasyDriver myStepper(D5,D6,D7,D1,D2);

void setup() {
  // initialize the serial port:
  Serial.begin(115200);
  
  // Give it some time to breathe
  delay(500);
  Serial.println();
  
  Serial.println("---------------------");
  Serial.println("Welcome to EasyDriver test script");
  Serial.println("---------------------");
}

void loop() {
  // put your main code here, to run repeatedly:
  myStepper.enable();

  // try moving 100 steps forward, then 50 steps backward, both on 50 ms delay between steps
  Serial.println("Moving clockwise");
  myStepper.move(EasyDriver::CLOCKWISE, 10, 1000);
  Serial.println("Moving counter-clockwise");
  myStepper.move(EasyDriver::COUNTERCLOCKWISE, 10, 1000);
}
