
/* 
 * Title:  RFID Door Access Control with MOSFET
 * Author: Andre Madureira
 * 
 * Details:   
 * Using pin 2 (int0) and 3 (int1) as DATA0 and DATA1 
 * of Wiegand RFID Protocol. Supports both 26 bit and
 * 34 bit protocols. Requires ATMEGA328 to work.
 * If RFID code is accepted, then activate control pin.
 * Otherwise, check for master code insertion.
 * 
 * Pass Master Tag 1 time and wait to access door
 * Pass it 2 times to register the next non-master card read
 * Pass it 3 times to unregister the next non-master card
 *    
 */

#include <avr/wdt.h>  //Watchdog Library (needs optiboot bootloader to work well)
#include <IRremote.h> //IR Library
#include <Wiegand.h>  //Wiegand RFID Library
#include "TYPES.h"    //Custom Project Constants and Types

RFID_CODE allow_code[ALLOW_CODE_SIZE]; //ALLOWED NON-MASTER ID'S
unsigned char allow_code_len;          //# OF IDS ALLOWED (MAX VALUE IS ALLOW_CODE_SIZE)

unsigned long prev_time;   //PREVIOUS TIME COUNTER
String IR_str;             //String to store IR received signals

WIEGAND rfid;
IRrecv ir(PIN_IR_RECV);

void setup() {
  wdt_enable(WDTO_8S);           //watchdog configured to 8s
  wdt_reset();                   //reset watchdog
  initialize_pins();             //SETUP PINS TO INITIAL VALUES AND SIGNAL DIRECTIONS
  Serial.begin(9600);            //INITIALIZE SERIAL PC COMMUNICATION
  rfid.begin();                  //INITIALIZE WIEGAND SERIAL-PARALLEL COMMUNICATION
  ir.enableIRIn();               //START IR RECEIVER
  read_codes_EEPROM();           //READ ID'S FROM EEPROM
  prev_time = 0;                 //INITIALIZE PREV_TIME
  Serial.println(F("RFID Door Access Control - READY"));
  print_n_acc_codes();           //PRINT # OF IDS REGISTERED
}

void loop() {  
  wdt_reset();                   //reset watchdog
  static MasterFunction num_master_fnc = ACCESS_NULL; //KEEP TRACK OF MASTER ID FUNCTION  
  if (rfid.available()) {                             //CHECK FOR RFID AVAILABILITY
    RFID_CODE code = rfid.getCode();                  //GET ID
    execute_rfid_fnc(code, num_master_fnc);
  }
  wdt_reset();                   //reset watchdog
  check_master_access(num_master_fnc);    
  check_ir_s_modes(num_master_fnc);
}




