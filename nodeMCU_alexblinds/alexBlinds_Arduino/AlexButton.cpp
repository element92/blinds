#include "AlexButton.h"
#include <Arduino.h>

// Constructor
AlexButton::AlexButton(int pin) {
  Serial.println("Initializing an instance of the AlexButton class");
  
  // Set the digital pin attached to the button
  buttonPin = pin;

  // Set the digital pin mode
  pinMode(buttonPin, INPUT);
  
  // Set the digital pin values associated with
  // the pressed and unpressed button. Depends on 
  // button wiring.
  bPRESSED = LOW;
  bUNPRESSED = HIGH;
  
  // Initialize the time counters, etc. to zero
  lastDebounceTime = 0;
  buttonStatus = 0;
  lastStatus = buttonStatus;
  buttonState = bUNPRESSED;
	
	// Set the debounce delay time
  // Increase if there's flicker
  debounceDelay = 50; // ms
  // Set the long press delay
  // If the button is held longer than this, triggers
  // a "long press".
  longPressDelay = 1000; // ms`
}

int AlexButton::status() {
  return buttonStatus;
}

bool AlexButton::update() {
  // If the button status is currently set to "...released", 
  // toggle back to neutral so that we don't accidentally 
  // trigger a release action twice.
  if (buttonStatus < 0) {
    buttonStatus = 0;
  }
  
  // Read the present value of the digital pin.
  reading = digitalRead(buttonPin);
  
  // If the reading has changed, due to pressing or noise
  if (reading != lastReading) {
    // reset the debounce timer
    lastDebounceTime = millis();
  }
  
  // If the debounce test passes, update the button state
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // If the button is currently "short pressed" 
    // (buttonStatus == 1), check if it's time to switch 
    // to a "long press".
    if (buttonStatus == 1) {
      if ((millis() - lastPressTime) > longPressDelay) {
        // Update status to "long press"
        buttonStatus = 2;
      }
    }
    
    // If the state of the button has changed
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == bPRESSED) {
        // Change the button status to "short pressed"
        buttonStatus = 1;
        // Reset the button press timer
        lastPressTime = millis();
      }
      else if (buttonState == bUNPRESSED) {
        // Toggle the button status to "released".
        // Conveniently defined so that the short and long press
        // release values are the negative of the corresponding
        // press values.
        buttonStatus = -1 * buttonStatus;
      }
    }
  }
  
  // Finally, update lastReading for the next time 
  // this function is called.
  lastReading = reading;
  
  if (buttonStatus != lastStatus) {
    lastStatus = buttonStatus;
    return true;
  }
  return false;
}
