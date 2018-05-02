#ifndef _IR_H
#define _IR_H

#include "Arduino.h" 
#include <avr/pgmspace.h>  // SAVE DATA INTO FLASH MEMORY
#include <avr/wdt.h>       // Watchdog Library (needs optiboot bootloader to work well)
#include <IRremote.h>      // IR Library (https://github.com/z3t0/Arduino-IRremote)

#include "Project.h"          // global PROJECT defines
#include "State.h"         // Pin object
#include "Pin.h"           // Pin object
#include "IDManager.h"     // IDManager (RFID_CODE)

// IR OPTIONS
#define PIN_IR_RECV         9          // Pin connected to IR Receiver
// IR OPTIONS END

enum IR_ID { // ID of IR code
    ZERO = 0,
    ONE,TWO,THREE,
    FOUR,FIVE,SIX,
    SEVEN,EIGHT,NINE,  
    OK,USD,
    PWR,MODE,MUTE,
    PLAY,PREV,NEXT,
    EQ,MINUS,PLUS,
    NO_BTN_PRESSED
};

typedef struct {
    IR_ID id;            // ID of the IR code
    unsigned long code;  // IR Code
} IR_IDCODE;

// VARIABLE HAS TO BE STATIC OR GLOBAL FOR PROGMEM TO WORK
const static IR_IDCODE control_codes[] PROGMEM = {  // PROGMEM TO SAVE VARIABLE INTO FLASH
    {PWR,0xffa25d}, {MODE,0xff629d}, {MUTE,0xffe21d},
    {PLAY,0xff22dd}, {PREV,0xff02fd}, {NEXT,0xffc23d},
    {EQ,0xffe01f}, {MINUS,0xffa857}, {PLUS,0xff906f},
    {ZERO,0xff6897}, {OK,0xff9867}, {USD,0xffb04f},
    {ONE,0xff30cf}, {TWO,0xff18e7}, {THREE,0xff7a85},
    {FOUR,0xff10ef}, {FIVE,0xff38c7}, {SIX,0xff5aa5},
    {SEVEN,0xff42bd}, {EIGHT,0xff4ab5}, {NINE,0xff52ad}
};

class IR {
public:    
    const static unsigned char STR_LEN   = 11;       // maximum number of digits of RFID CODE +1
    const static unsigned char NUM_CTRL_BTN = 21;       // # OF BUTTONS OF THE REMOTE CONTROL    
    
    IR(State& state);

    void begin();
    RFID_CODE getRFIDCode();        
private:
    IRrecv ir;    // IR sensor
    State& state; // system state

    char rfidStr[IR::STR_LEN];   // String to store IR received signals
    unsigned char rfidStrLen;    // counter to store IR_str current position

    IR_ID read();
    RFID_CODE getRFID_Str(IR_ID& ir_id);
    void resetIRStr();
};

#endif  // _IR_H