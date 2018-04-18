#include "State.h"

// ------------
//   PUBLIC
// ------------

State::State(): 
    state(ACCESS),    
    timer(ITV_EEPROM),
    led(PIN_LED, PIN_LED_INIT, PIN_LED_DELAY), 
    control(PIN_CONTROL, PIN_CONTROL_INIT, PIN_CONTROL_DELAY),
    bzr(PIN_BZR, PIN_BZR_INIT, PIN_BZR_DELAY) 
    { }

void State::begin(){
    // initialize pins IN-OUT
    control.begin();
    led.begin();
    bzr.begin();
    // read eeprom and initialize data registers
    manager.begin();
    // reset system state
    resetState();
}

void State::setMasterCodeChgMode(){
    changeState(MASTER_CHG);
}

// run code based on System state
void State::execute(const RFID_CODE& code){
    int result = 0;         
    // keep door closed at all times
    control.reset();
    wdt_reset();        
    if (manager.getMasterCode() == code){
        // change to next state
        nextState();
    } else {
        // perform action
        switch(state){  
        case ACCESS:                 
            if (manager.find(code) != -1){            
                // allow access
                control.digitalPulse();
            } else {
                #ifdef DEBUG_MODE
                Serial.print("Unregistered ID - ");        
                Serial.println(code);   
                #endif
                // we have an error
                result = 1;
            }
            break;          
        case REGISTER:                  // REGISTER FUNCTION (PASS 1-TIME MASTER ID)
            timer.start();
            result = manager.registerID(code);
            break;          
        case UNREGISTER:                  // UNREGISTER FUNCTION (PASS 2-TIMES MASTER ID)
            timer.start();
            result = manager.unregisterID(code);
            break;  
        case MASTER_CHG:    
            // DO NOTHING - wait for master code validation (above)
            break;
        case MASTER_CONFIRM: 
            // we know the master code is validated 
            manager.setMasterCode(code);
            nextState();
            break;
        default:
            break;                        
        } 
    }   
    // we had a problem 
    if (!result){        
        bzr.digitalPulse();
    } 
    // timeout returns to reset state
    if (timer.hasTimeoutHappened()){                
        resetState();
    }
}

void State::blinkLED(unsigned char div){
    led.digitalPulse(div);
}

void State::bzrBeep(unsigned char div){
    bzr.digitalPulse(div);
}

// ------------
//   PRIVATE
// ------------

// return next state
void State::nextState(){
    switch(state) {
    case ACCESS:
        changeState(REGISTER);
        break;
    case REGISTER:    
        changeState(UNREGISTER);
        break;
    case UNREGISTER:
        changeState(ACCESS);
        break;
    case MASTER_CHG:        
        changeState(MASTER_CONFIRM);
        break;
    case MASTER_CONFIRM:    
        changeState(ACCESS);        
        break;
    default:
        changeState(ACCESS);
        break;
    }
}

// reset to initial state
void State::resetState(){
    changeState(ACCESS);
}

// change program state to new state
void State::changeState(StateSystem st){
    state = st;
    switch(state) {
    case ACCESS:
        wdt_reset();  
        timer.stop();  
        #ifdef DEBUG_MODE
        Serial.println(F("Access Mode"));
        #endif
        led.reset();        
        // save any pending data to eeprom
        saveDataEEPROM();
    case REGISTER:
        wdt_reset();
        timer.start();
        #ifdef DEBUG_MODE
        Serial.println(F("Registration Mode"));
        #endif        
        // LED OPPOSITE STATE OF INITIAL 
        led.setState(1-PIN_LED_INIT);
        break;
    case UNREGISTER:
        wdt_reset();  
        timer.start();      
        #ifdef DEBUG_MODE
        Serial.println(F("Unregistration Mode"));
        #endif
        // blink LED in OPOSITE STATE OF INITIAL
        led.setState(1-PIN_LED_INIT);
        led.digitalPulse();
        break;
    case MASTER_CHG:
        wdt_reset();
        timer.start();        
        #ifdef DEBUG_MODE
        Serial.println(F("Master Change"));
        #endif
        // LED OPPOSITE STATE OF INITIAL 
        led.setState(1-PIN_LED_INIT);
        break;
    case MASTER_CONFIRM:
        wdt_reset();     
        timer.start();   
        #ifdef DEBUG_MODE
        Serial.println(F("Master Change Confirm"));
        #endif
        // blink LED in OPOSITE STATE OF INITIAL
        led.setState(1-PIN_LED_INIT);
        led.digitalPulse();
        break;
    default:        
        break;
    }
}

// save data to eeprom if needed
void State::saveDataEEPROM(){
    if (manager.hasUnsavedData()){
        manager.write_EEPROM();
    }
}

