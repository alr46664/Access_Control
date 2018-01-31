
/* 
 * Title:  RFID Door Access Control with MOSFET
 * Author: Andre Madureira
 * 
 * Default Master: 24121
 * 
 * Details:   
 * Using pin 2 (int0) and 3 (int1) as DATA0 and DATA1 
 * of Wiegand RFID Protocol. Supports both 26 bit and
 * 34 bit protocols (its better if we use the 36 bit 
 * protocol). Requires ATMEGA328 to work.
 * If RFID code is accepted, then activate control pin.
 * Otherwise, check for master code insertion.
 * 
 * RFID FUNCITONALITY:
 * Pass Master Tag 1 time and wait to access door
 * Pass it 2 times to register the next non-master card
 * Pass it 3 times to unregister the next non-master card
 * 
 * IR FUNCTIONALITY:
 * Buttons 0-9 are numerical buttons and are parsed to unsigned 
 * long once confirmed. 
 * Buttons Two-Arrow-Left-Right and U/SD are OK and Cancel buttons 
 * respectively. So they confirm or cancel the operation/code.
 * 
 * TO CHANGE MASTER RFID TAG:
 * 1. In the normal operation mode (do not pass master rfid not 
 *    even once), using the IR, press MODE btn.
 * 2. Enter the actual MASTER RFID TAG and press OK.
 * 3. If incorrect, buzzer will ring. If code was correct, then
 *    type in the new MASTER RFID and press OK.
 * 4. If operation succeded, LED will flash.
 *    
 */

#include <avr/wdt.h>  // Watchdog Library (needs optiboot bootloader to work well)
#include "TYPES.h"    // Custom Project Constants and Types

RFID_CODE allow_code[ALLOW_CODE_SIZE]; // ALLOWED NON-MASTER ID'S
unsigned char allow_code_len;          // # OF IDS ALLOWED (MAX VALUE IS ALLOW_CODE_SIZE)

RFID_CODE master_code;                        // MASTER CODE RFID TAG
MasterFunction num_master_fnc = ACCESS_NULL;  // KEEP TRACK OF MASTER ID FUNCTION  

unsigned long prev_time;   // PREVIOUS TIME COUNTER

char IR_str[IR_STR_LEN];   // String to store IR received signals
unsigned char IR_count;    // counter to store IR_str current position

bool save_master_code;          // IF TRUE, SAVE MASTER CODE TO EEPROM
unsigned char chg_master_code;  // IF 0x3, NEXT CODE INSERTED INTO IR WILL CHG MASTER_CODE

bool save_to_EEPROM;       // SET IF EEPROM NEEDS UPDATE

void setup() {  
  setupModules(EN_MODULES);      // disable unused modules to reduce power consumption  
  initialize_pins();             // SETUP PINS TO INITIAL VALUES AND SIGNAL DIRECTIONS
  init_variables();              // SETUP INITIAL VALUES TO VARIABLES        
  
  #ifdef DEBUG_MODE
  Serial.begin(9600);            // INITIALIZE SERIAL COMMUNICATION  
  #endif
  
  read_EEPROM();                 // READ DATA FROM EEPROM 
  init_oled();                   // initialize oled
  init_rfid();                   // initialize rfid
  init_ir();                     // initialize ir
    
  // DEBUG   
  #ifdef DEBUG_MODE
  Serial.println(F("RFID Door Access Control - READY"));
  print_n_acc_codes();           //PRINT # OF IDS REGISTERED  
  #endif
    
  wdt_enable(WDTO_8S);           // watchdog configured to 8s
  wdt_reset();                   // reset watchdog  
}

void loop() {    
  wdt_reset();                   // reset watchdog
  // keep door closed at all times
  digitalWrite(PIN_CONTROL, CONTROL_INIT_STATE); 
  check_rfid();                  // check for rfid read
  wdt_reset();                   // reset watchdog
  check_elapsed_master_fnc();    // execute timed procedures (master function)
  check_ir();                    // check IR btn pressed
  wdt_reset();                   // reset watchdog
  write_EEPROM();                // write EEPROM codes if needed  
}

void init_variables(){
  prev_time = 0;                 // INITIALIZE PREV_TIME    
  reset_ir_str();                // RESET IR STRING    
  chg_master_code = 0;           // reset chg master code
  save_master_code = false;      // DO NOT SAVE/CHG MASTER CODE YET
  save_to_EEPROM = false;        // do not update eeprom yet
}


