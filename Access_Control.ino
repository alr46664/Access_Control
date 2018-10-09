
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

#include "Arduino.h" 
#include <avr/wdt.h>   // Watchdog Library (needs optiboot bootloader to work well)
#include <Wiegand.h>   // Wiegand RFID Library 

#include <PowerMgr.h>  // PowerManager Library
#include "Project.h"      // project GLOBAL header
#include "IDManager.h"  // RFID Tag Manager - Read EEPROM - Write EEPROM
#include "State.h"     // System State Tracker
#include "Bluetooth.h"  // Bluetooth 4.0 Manager

IDManager manager;     // RFID EEPROM manager    
State stateManager(&manager);    // stateManager state tracker
Bluetooth bluetooth(&manager);   // bluetooth controller
WIEGAND rfid;            // WIEGAND protocol RFID reader

void setup() {  
  // disable unused modules to reduce power consumption
  PowerMgr::setupModules((~POWER_EN_ALL) || POWER_EN_TIMER0 || POWER_EN_TIMER1 || POWER_EN_TIMER2 || POWER_EN_USART);    
  // debug mode begin
  #ifdef DEBUG_MODE
  Serial.begin(9600);            // INITIALIZE SERIAL PC COMMUNICATION    
  #endif
  // initialize devices, IDManager and stateManager variables
  manager.begin();
  stateManager.begin();  
  bluetooth.begin(9600);
  rfid.begin();                             
  // initialize watchdog timer
  // wdt_reset();                // reset watchdog  
  wdt_enable(WDTO_8S);           // watchdog configured to 8s
  wdt_reset();                   // reset watchdog  
}

void loop() {      
  wdt_reset();    
  if (bluetooth.isConnected()){
    bluetooth.execute();
    wdt_reset();          
  } else if (rfid.available()){
    // state manager logic here 
    // (register, unregister or access)
    RFID_CODE code = rfid.getCode();    
    stateManager.execute(code);  
    #ifdef DEBUG_MODE
    Serial.println("RECV RFID");
    #endif
  }    
  #ifdef DEBUG_MODE    
  if (Serial.available()){
    delay(150);
    while(Serial.available()){
      Serial.read();
    }
    Serial.print(F("MASTER CARD - "));
    Serial.println(manager.getMasterCode());
    Serial.println();
    Serial.println(F("CARD LIST"));
    for (int i = 0; i < manager.getTotalCards(); i++) {
      Serial.println(manager.getCard(i));
    }    
  }
  #endif
}
