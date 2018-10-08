#ifndef _State_H
#define _State_H

#include "Arduino.h" 
#include <avr/wdt.h>    // Watchdog Library (needs optiboot bootloader to work well)

#include <Scheduler.h>  // Scheduler class
#include "Project.h"       // global PROJECT defines
#include "Pin.h"        // Output Digital Pins Class
#include "IDManager.h"  // RFID Tag Manager - Read EEPROM - Write EEPROM

// CONTROL PIN
#define PIN_CONTROL         6          // # OF DIGITAL PIN TO CONTROL
#define PIN_CONTROL_DELAY   350L       // KEEP HIGH OUTPUT IN PIN_CONTROL FOR # MS
#define PIN_CONTROL_INIT    HIGH       // define control initial state

// LED
#define PIN_LED             4          // OUTPUT LED
#define PIN_LED_DELAY       400L       // MS TO BLINK LED 
#define PIN_LED_INIT        LOW        // define led initial state
// LED END

// BZR
#define PIN_BZR             5          // OUTPUT BUZZER
#define PIN_BZR_DELAY       400L       // MS TO MANAGE FREQUENCY BZR
#define PIN_BZR_INIT        HIGH       // define BZR initial state

// INTERVAL(ms) TO WAIT FOR OTHER TAGS TO BE (UN)REGISTERED, THEN SAVE TO EEPROM
#define ITV_EEPROM          20000      

// DEFINE SYSTEM STATES
enum StateSystem {ACCESS = 0, REGISTER, UNREGISTER, MASTER_CHG, MASTER_CONFIRM};     

class State {
public:    
    State(IDManager* manager);
    
    void begin();    
    void setMasterCodeChgMode();
    void execute(const RFID_CODE& code);

    void blinkLED(unsigned char div = 1);
    void bzrBeep(unsigned char div = 1);
private:
    Pin control, led, bzr; // out pins
    IDManager* manager;     // RFID EEPROM manager        
    Scheduler timer;       // timer to perform async actions
    unsigned char state;   // store system state

    void nextState();      
    void resetState();    
    void changeState(StateSystem st);
    void saveDataEEPROM();
};

#endif  // _State_H 
