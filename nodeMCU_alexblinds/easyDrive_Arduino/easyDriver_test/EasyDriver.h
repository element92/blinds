/*
  EasyDriver.h - Class for controling an EasyDriver stepper motor controller
  Created by Alexander Burns, 2018-07-22.
*/

#ifndef HEADER_EasyDriver
  #define HEADER_EasyDriver

#include "Arduino.h"
  
  class EasyDriver {

  public:
    // Constants
    // Direction constants for setting the rotational direction.
    // Direction is from the motor body's point of view.
    // LOW = 0x0
    // HIGH = 0x1
    const static byte CLOCKWISE = 0x1; 
    const static byte COUNTERCLOCKWISE = 0x0;
    // Constructors
    EasyDriver(int pin_enable, int pin_step, int pin_dir, int pin_MS1, int pin_MS2); // Constructor.
    // Functions
    void enable();  // Enables the motor controller
    void disable(); // Disables the motor controller
    void move(int dir, int delay, int steps); // Move the motor

  private:
    // variables
    int _pin_enable;
    int _pin_step;
    int _pin_dir;
  
  };
  
 #endif
