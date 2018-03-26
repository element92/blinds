/*
  AlexButton.h - Class for handling various
  features of a physical on-off button attached
  to a digital input pin.
  Created by Alexander Burns, 2018-03-25.
*/

#ifndef HEADER_AlexButton
  #define HEADER_AlexButton

#include "Arduino.h"

class AlexButton {
  public:
    // Constructors
    AlexButton(int pin); // Constructor. Pin is the digital in pin attached to the button.
    // Functions
    bool update(void); // Updates the status of the button.
					   // Call once every Arduino loop() cycle.
					   // Returns 1 if the status has changed since the last update().
	  int status(void); // Returns the current status of the button.
					  // -2 = Button has been released (long press released)
					  // -1 = Button has been released (short press released)
					  //  0 = Nothing to report
					  //  1 = Button is depressed (short press)
					  //  2 = Button is depressed (long press)
  private:
	int buttonPin; // Holds the pin number of the digital input.
	int buttonStatus; // Holds the current status of the button. Value table listed above.
	bool buttonState; // Holds the current state of the button (pressed or unpressed).
	bool reading; // Holds the instantaneous value of the input pin 
	bool lastReading;
  int lastStatus;
  bool bPRESSED;
  bool bUNPRESSED;
	unsigned long lastDebounceTime; // The last time the output pin was toggled
  unsigned long lastPressTime; // The last time the button was pressed
	int debounceDelay; // The debounce time. Increase if the output flickers.
	int longPressDelay; // The length of time considered a long press
};

#endif
