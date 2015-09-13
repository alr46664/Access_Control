
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

RFID_CODE master_code;                 //MASTER CODE RFID TAG
MasterFunction num_master_fnc = ACCESS_NULL; //KEEP TRACK OF MASTER ID FUNCTION  

unsigned long prev_time;   //PREVIOUS TIME COUNTER
String IR_str;             //String to store IR received signals

bool save_master_code;         //IF TRUE, SAVE MASTER CODE TO EEPROM
bool chg_master_code;          //IF TRUE, NEXT CODE INSERTED INTO IR WILL CHG MASTER_CODE
unsigned long save_to_EEPROM;  //SET WHICH POSITIONS NEED TO BE UPDATED INTO EEPROM,
                               //0 means do not save to EEPROM
                               
WIEGAND rfid;                  //WIEGAND protocol RFID reader
IRrecv ir(PIN_IR_RECV);        //IR sensor

void setup() {
  wdt_enable(WDTO_8S);           //watchdog configured to 8s
  wdt_reset();                   //reset watchdog
  prev_time = 0;                 //INITIALIZE PREV_TIME    
  initialize_pins();             //SETUP PINS TO INITIAL VALUES AND SIGNAL DIRECTIONS
  Serial.begin(9600);            //INITIALIZE SERIAL PC COMMUNICATION
  read_EEPROM();                 //READ DATA FROM EEPROM 
  rfid.begin();                  //INITIALIZE WIEGAND SERIAL-PARALLEL COMMUNICATION
  ir.enableIRIn();               //START IR RECEIVER    
  Serial.println(F("RFID Door Access Control - READY"));
  print_n_acc_codes();           //PRINT # OF IDS REGISTERED
}

void loop() {  
  wdt_reset();                   //reset watchdog
  if (rfid.available())              //CHECK FOR RFID AVAILABILITY    
    execute_rfid_fnc(rfid.getCode());  
  wdt_reset();                   //reset watchdog
  check_master_access();         //check master for its access mode  
  check_ir();                    //check IR btn pressed
  wdt_reset();                   //reset watchdog
  write_EEPROM();   //check if any codes need writing
}




