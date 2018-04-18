
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

#include <avr/wdt.h>   // Watchdog Library (needs optiboot bootloader to work well)
#include <PowerMgr.h>  // PowerManager Library
#include <Wiegand.h>   // Wiegand RFID Library 

// UNCOMMENT FOR DEBUG MODE
// #define DEBUG_MODE 1

#include "State.h"     // System State Tracker
#include "IR.h"        // IR Controller

State stateManager;          // stateManager state tracker
WIEGAND rfid;          // WIEGAND protocol RFID reader
IR ir(stateManager);         // IR controller

void setup() {  
  // disable unused modules to reduce power consumption
  PowerMgr::setupModules((~POWER_EN_ALL) || POWER_EN_TIMER0 || POWER_EN_USART);    
  // debug mode begin
  #ifdef DEBUG_MODE
  Serial.begin(9600);            // INITIALIZE SERIAL PC COMMUNICATION    
  #endif
  // initialize devices and stateManager variables
  stateManager.begin();
  rfid.begin();                  
  ir.begin();               
  
  // initialize watchdog timer
  // wdt_reset();                   // reset watchdog  
  wdt_enable(WDTO_8S);           // watchdog configured to 8s
  wdt_reset();                   // reset watchdog  
}

void loop() {      
  wdt_reset();                   
  // get any IR signals  
  RFID_CODE code = ir.getRFIDCode();  
  wdt_reset();                       
  // check for rfid availability
  if (rfid.available()){          
    code = rfid.getCode();    
  }
  // run any stateManager code here
  if (code > 0) {
    stateManager.execute(code);
  }
}
