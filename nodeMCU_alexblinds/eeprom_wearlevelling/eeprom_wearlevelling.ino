/*
 * EEPROM wear levelling algorithm
 *
 * Reads and writes values to the EEPROM.
 * 
 * By Alex Burns
 * 2018-03-22
 */

#include <EEPROM.h>

// EEPROM_int class
/* Stores a single int value in EEPROM by applying a 
 *  wear-levelling algorithm.
 *  
 *  public functions:
 *    get()  Returns the stored value
 *    retrieve()  Retrieves the value from EEPROM. Should be called after initializing.
 *    save(int x) Saves x to EEPROM
*/
class EEPROM_int {
  private:
    int value;
    int blockSize = sizeof(int) + 1; // data type of 'value' + 1 for the dirty bit
    int address;
    byte dirtyBit;
    int wearLevelStartAddr; // First byte allocated
    int wearLevelEndAddr; // Last byte allocated
    
  public:
    // Constructors
    EEPROM_int() {
      // Default values
      EEPROM_int (0x80, 0x1ff);
    }
    EEPROM_int(int x1, int x2) {
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
      // Can't retrieve() outside of the setup() block.
      //retrieve();
    }

    int get() {
      return value;
    }
    
    // Function to retrieve the value from EEPROM
    int retrieve() {
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
            // Back the address off a block and exit the loop.
            address -= blockSize;
            Serial.print("Found the last dirty bit in address 0x");
            Serial.println(address, HEX);
            break;
          }
          address += blockSize;
        }
        // Fetches the data
        EEPROM.get(address+1, value);
        Serial.println("Fetched value");
        Serial.print("0x");
        Serial.print(address, HEX);
        Serial.print(" --> ");
        Serial.println(value);
      }
    }

    // Saves the give value to EEPROM according to the defined
    // wear levelling routine, which uses a dirty bit (actually
    // a byte in this case) to designate the address of the 
    // current block.
    void save(int x) {
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
      // Save the data.
      value = x;
      EEPROM.write(address, dirtyBit);
      EEPROM.put(address + 1, x);
      EEPROM.commit();
      Serial.print(x);
      Serial.print(" --> 0x");
      Serial.println(address, HEX);
    } // end of function 'save'
    
} // End of class 'Memory'


// Allocate some EEPROM for an int.
myMemoryVal = EEPROM_int(0x70, 0x1fa);


void setup()
{
  // initialize the serial port:
  Serial.begin(115200);

  delay(2000);
  Serial.println();
  
  // initialize the EEPROM
  EEPROM.begin(512);
  myMemoryVal.retrieve();
  
  Serial.print("Default value on startup: ");
  Serial.println(myMemoryVal.get() );
}

void loop()
{
  delay(5000);
  int y = (int) random(0,200);
  myMemoryVal.save(y);
}
