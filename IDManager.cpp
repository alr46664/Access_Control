 
#include "IDManager.h"

// -------------
//    PUBLIC
// -------------

IDManager::IDManager(): unsaved(false), master_code(0), allow_code_len(0) { }

void IDManager::begin(){
    unsaved = false;
    master_code = 0;
    allow_code_len = 0;
    // read eeprom data
    read_EEPROM();
}  

unsigned int IDManager::getTotalCards(){
    return allow_code_len;
}

RFID_CODE IDManager::getCard(unsigned int pos){
    return allow_code[pos];
}

int IDManager::find(const RFID_CODE& code){
    for (short i = 0; i < allow_code_len; i++){
        if (code == allow_code[i]){
            return i;      
        }
    }
    return -1;
}

int IDManager::registerID(const RFID_CODE& code){
    if (this->find(code) != -1){
        #ifdef DEBUG_MODE
        Serial.print(F("Registration Warning - ID Already Registered: "));
        Serial.println(code);    
        #endif
        return 0;
    }
    // CHECK STORAGE SPACE AVAILABILITY
    if (allow_code_len+1 < ALLOW_CODE_SIZE){ 
        allow_code[allow_code_len++] = code; // STORE NEW CODE       
        unsaved = true;
        #ifdef DEBUG_MODE
        Serial.print(F("RFID Code: ")); 
        Serial.print(code);
        Serial.println(F(" - Registered"));    
        printNumberCodes();
        #endif
        return 0;
    } else {
        #ifdef DEBUG_MODE
        Serial.print(F("Registration error - Exceeded maximum number of ID's: "));
        Serial.println(ALLOW_CODE_SIZE);
        #endif        
        return 1;                
    }
}

int IDManager::unregisterID(const RFID_CODE& code){
    const short ID_POS = this->find(code);
    // CHECK IF ID EXISTS
    if (ID_POS != -1) {           
        //UNREGISTER ID    
        allow_code[ID_POS] = allow_code[--allow_code_len];       
        unsaved = true;
        #ifdef DEBUG_MODE
        Serial.print(F("RFID Code: "));
        Serial.print(code);
        Serial.println(F(" - Unregistered")); 
        printNumberCodes();   
        #endif
        return 0;
    } else {
        #ifdef DEBUG_MODE
        Serial.print(F("Unregistration error - Could not find ID: "));
        Serial.println(code);
        #endif            
        return 1;
    }  
}

void IDManager::write_EEPROM(){
    unsaved = false;
    // SAVE # OF IDS INTO 1ST BYTE OF EEPROM
    EEPROM.put(0, allow_code_len);                        
    // CHECK ALL IDS FOR THE NEED OF SAVING         
    for (unsigned short i = 0; i < allow_code_len; i++){  
        wdt_reset();
        EEPROM.put(1 + i*sizeof(RFID_CODE), allow_code[i]);
    }    
    write_EEPROM_master();
    #ifdef DEBUG_MODE
    Serial.println(F("RFID TAGS SAVED TO EEPROM "));    
    #endif      
}

RFID_CODE IDManager::getMasterCode(){
    return master_code;
}

void IDManager::setMasterCode(const RFID_CODE& master_code){
    unsaved = true;
    this->master_code = master_code;    
    #ifdef DEBUG_MODE
    Serial.print(F("NEW MASTER = "));
    Serial.println(master_code);
    #endif    
}

bool IDManager::hasUnsavedData(){
    return unsaved;
}

// -------------
//    PRIVATE
// -------------

void IDManager::read_EEPROM(){
    // READ MASTER CODE ID FROM [EEPROM_MASTER_POS-EEPROM_MASTER_POS+7]B OF EEPROM
    EEPROM.get(EEPROM_MASTER_POS, master_code);           
    // EEPROM IS BEING USED FOR FIRST TIME
    if (master_code == 0xFFFFFFFF || master_code == 0x0){ 
        // set master code to its default value      
        setMasterCode(DEFAULT_MASTER_CODE);              
    }       

    // START READING ALLOW_CODE ARRAY
    EEPROM.get(0, allow_code_len);
    // CHECK IF IDS LEN IS VALID (EEPROM IS BEING USED FOR FIRST TIME)
    if (allow_code_len > ALLOW_CODE_SIZE){
        // LEN NOT VALID, MAKE IT DEFAULT VALUE
        allow_code_len = 0;               
    }   
    // READ ALL IDS FROM EEPROM
    for (short i = 0; i < allow_code_len; i++){         
        EEPROM.get(1 + i*sizeof(RFID_CODE), allow_code[i]);  
    }

    #ifdef DEBUG_MODE
    Serial.println(F("RFID Door Access Control - READY"));
    printNumberCodes();           
    Serial.print(F("MASTER CODE = "));
    Serial.println(master_code);    
    #endif
}

void IDManager::write_EEPROM_master(){
    // SAVE MASTER CODE TO [EEPROM_MASTER_POS-EEPROM_MASTER_POS+7]B OF EEPROM   
    EEPROM.put(EEPROM_MASTER_POS, master_code); 
    #ifdef DEBUG_MODE
    Serial.print(F("MASTER SAVED TO EEPROM "));
    Serial.println(master_code);
    #endif   
}

//PRINT # OF IDS TO SERIAL
#ifdef DEBUG_MODE
void IDManager::printNumberCodes(){
  Serial.print(allow_code_len);  
  Serial.println(F(" Access Codes Registered"));
}
#endif
