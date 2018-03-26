#include "EEPROM_int.h"
#include <Arduino.h>
#include <EEPROM.h>

EEPROM_int::EEPROM_int(void) {
  EEPROM_int (0x80, 0x1ff);
}

EEPROM_int::EEPROM_int(int x1, int x2) {
  blockSize = sizeof(int) + 1; // data type of 'value' + 1 for the dirty bit
  Serial.print("Initializing a block of EEPROM memory from (x1:x2): ");
  Serial.print(x1);
  Serial.print(":");
  Serial.println(x2);
  wearLevelStartAddr = x1;
  wearLevelEndAddr = x2;
  // Set default values until they are retrieved
  value = 0;
  address = wearLevelStartAddr;
  dirtyBit = 1;
}

int EEPROM_int::get(void) {
  return value;
}

void EEPROM_int::set(int x) {
//  Serial.print("Value set to ");
//  Serial.println(x);
  value = x;
}

void EEPROM_int::retrieve(void) {
  Serial.println("Retrieving saved value from memory.");
  // Let's start looking at the beginning
  // A very good place to start
  address = wearLevelStartAddr;
  // The first position in memory always contains the dirty bit.
  dirtyBit = EEPROM.read(wearLevelStartAddr);
  Serial.print("Current dirty bit: ");
  Serial.println(dirtyBit);
  if (dirtyBit != 1 && dirtyBit != 0) {
	  Serial.println("WARNING: Invalid dirty bit. Reinitializing memory");
	  address = wearLevelStartAddr;
	  dirtyBit = 1;
	  value = 0;
	  EEPROM.write(address, dirtyBit);
	  EEPROM.put(address+1, value);
	  EEPROM.commit();
  }
  else {
  	Serial.println("addr | dirty bit");
  	while (address <= wearLevelEndAddr - blockSize + 1) {
  	  Serial.print("0x");
  	  Serial.print(address, HEX);
  	  Serial.print(" | ");
  	  Serial.println(EEPROM.read(address) );
  	  if (EEPROM.read(address) != dirtyBit) {
    		// We've located the last save location.
    		break; // break out of while loop
	    } // end if
	  address += blockSize;
	  } // end while

    // Back the address off a block, since the while loop has
    // advanced us one block too far.
    address -= blockSize;
    Serial.print("Found the last dirty bit in address 0x");
    Serial.println(address, HEX);
    
  	// Fetches the data
  	EEPROM.get(address+1, value);
  	Serial.println("Fetched value");
  	Serial.print("0x");
  	Serial.print(address, HEX);
  	Serial.print(" --> ");
  	Serial.println(value);
  } // end fetching routine
} // end function "retrieve"

void EEPROM_int::save () {
  // Only saves the current value and increments the address counter
  // if the current value differs from the one in EEPROM
  // Uses temp, which must be the same type as value, because
  // get() automatically inserts the return value into temp,
  // so we can't use value or value will be overwritten.
  int temp;
  if(EEPROM.get(address+1, temp) != value) {
    // Increment the current address by the data size + 1.
    // (+1 is for the dirty bit)
    address += blockSize;
    // Wraps around to the start if we've overflowed
    if(address > wearLevelEndAddr - blockSize + 1) {
  	// Toggle the dirty bit.
  	if(dirtyBit == 1) {dirtyBit = 0;}
  	else{dirtyBit=1;}
  	// wrap around the current address to the start
  	address = wearLevelStartAddr;
  	Serial.println("REACHED END OF MEMORY.  Wrapping around");
  	Serial.print("New dirty bit: ");
  	Serial.println(dirtyBit);
    }
    // Save the data currently stored in value
    EEPROM.write(address, dirtyBit);
    EEPROM.put(address + 1, value);
    EEPROM.commit();
    Serial.print(value);
    Serial.print(" --> 0x");
    Serial.println(address, HEX);
  }
}
