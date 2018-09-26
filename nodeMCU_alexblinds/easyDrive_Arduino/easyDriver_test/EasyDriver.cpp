#include <Arduino.h>
#include "EasyDriver.h"

EasyDriver::EasyDriver(int pin_enable, int pin_step, int pin_dir, int pin_MS1, int pin_MS2) {
  
  // Record inputs
  _pin_enable = pin_enable;
  _pin_step = pin_step;
  _pin_dir = pin_dir;
  
  // Set up pins on the Arduino for output
  pinMode(pin_enable, OUTPUT);
  pinMode(pin_step, OUTPUT);
  pinMode(pin_dir, OUTPUT);

  // Set to full stepping mode
  digitalWrite(pin_MS1, LOW);
  digitalWrite(pin_MS2, LOW);
}

void EasyDriver::enable() {
  digitalWrite(_pin_enable, LOW);
}

void EasyDriver::disable() {
  digitalWrite(_pin_enable, HIGH);
}

void EasyDriver::move(int dir, int delaytime, int steps) {
  // Spins the motor forward
  
  // Set the motor direction
  digitalWrite(_pin_dir, dir);

  // Perform the indicated number of steps
  for (int i=0; i<=steps; i++) {
    digitalWrite(_pin_step, HIGH);
    delay(delaytime/2);
    //delayMicroseconds(delay_mod*1000);   // wait
    digitalWrite(_pin_step, LOW);
    delay(delaytime/2);   // wait
  }

}
