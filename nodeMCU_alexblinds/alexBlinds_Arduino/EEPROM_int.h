/*
  EEPROM_int.h - Class for storing a value in EEPROM
  Created by Alexander Burns, 2018-03-24.
*/

#ifndef HEADER_EEPROM_int
  #define HEADER_EEPROM_int

#include "Arduino.h"
  
  class EEPROM_int {
	public:
    int value;
	  EEPROM_int(void);
	  EEPROM_int(int x1, int x2);
	  int get(void);
    void set(int x);
	  void retrieve(void);
	  void save (void);
	private:
	  int blockSize;
    int address;
    byte dirtyBit;
    int wearLevelStartAddr; // First byte allocated
    int wearLevelEndAddr; // Last byte allocated
  };
  
 #endif
