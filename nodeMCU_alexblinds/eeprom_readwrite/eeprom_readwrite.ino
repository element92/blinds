/*
 * EEPROM Read Write
 *
 * Reads and writes values to the EEPROM 
 */

#include <EEPROM.h>

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;

// Global variables
int x = 0; // Our read/write variable

void setup()
{
  // initialize the EEPROM
  EEPROM.begin(512);
  // initialize the serial port:
  Serial.begin(115200);

  delay(500);
  Serial.println();
  
  Serial.print("Default value of x on startup: ");
  Serial.println(x);
  Serial.print("Address starting location: ");
  Serial.println(addr);
  
  // Reads values from the EEPROM
  EEPROM.get(addr, x);
  Serial.print("Value of x read from memory: ");
  Serial.println(x);
  
  // Generates a random number
  x = (int) random(0,200);
  Serial.print("New value of x: ");
  Serial.println(x);
  
  // Writes a random value to the EEPROM
  Serial.print("Size of x: ");
  Serial.println(sizeof(int));
  Serial.println("Saving x to EEPROM");
  EEPROM.put(addr, x);
  addr += sizeof(int);
  
  Serial.print("New address location: ");
  Serial.println(addr);
  
  // A commit command is necessary for the ESP8266
  EEPROM.commit();
}

void loop()
{
  delay(10);
}
