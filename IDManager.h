#ifndef _IDManager_H
#define _IDManager_H

#include "Arduino.h" 
#include <EEPROM.h>
#include <avr/wdt.h>   // Watchdog Library (needs optiboot bootloader to work well)

#include "Project.h"       // global PROJECT defines

// SET DEFAULT MASTER CODE IN CASE EEPROM HAS NEVER BEING USED
#define DEFAULT_MASTER_CODE  26121L     
// INITIAL MEM POS TO SAVE MASTER ID TO EEPROM
#define EEPROM_MASTER_POS    1017       
// MAX # OF IDS STORABLE - LIMIT IS 255 (BYTE)
#define ALLOW_CODE_SIZE      50  

typedef unsigned long RFID_CODE;    

class IDManager {
public:    
    
    IDManager();  

    void begin();        
    int find(const RFID_CODE& code);

    int registerID(const RFID_CODE& code);
    int unregisterID(const RFID_CODE& code);

    RFID_CODE getMasterCode();
    void setMasterCode(const RFID_CODE& master_code);    

    bool hasUnsavedData();
    void write_EEPROM();
private:  
    // has unsaved IDS (EEPROM needs update)
    bool unsaved;
    // MASTER CODE RFID TAG
    RFID_CODE master_code;                        
    // ALLOWED NON-MASTER ID'S
    RFID_CODE allow_code[ALLOW_CODE_SIZE]; 
    // # OF IDS ALLOWED (MAX VALUE IS ALLOW_CODE_SIZE)
    unsigned char allow_code_len;     

    void read_EEPROM();    
    void write_EEPROM_master();

    #ifdef DEBUG_MODE
    void printNumberCodes();
    #endif
};


#endif  // _IDManager_H 
