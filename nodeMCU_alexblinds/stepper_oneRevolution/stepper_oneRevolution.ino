
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, D0, D1, D2, D3);

void setup() {
  // set the speed at 8 rpm:
  myStepper.setSpeed(8);
  // initialize the serial port:
  Serial.begin(115200);
}

void loop() {
  // step one revolution in one direction:
  Serial.println("clockwise");
  for(int i=0; i<stepsPerRevolution; ++i){
    myStepper.step(1);
    delay(1); // Gives the ESP8266 time to do its thing
  }
  
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  for(int i=0; i<stepsPerRevolution; ++i){
    myStepper.step(-1);
    delay(1); // Gives the ESP8266 time to do its thing
  }

}

