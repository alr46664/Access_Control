
#include "Pin.h"

// ------------
//   PUBLIC
// ------------

Pin::Pin(uint8_t pin, uint8_t initial_state, unsigned long delay_ms) : pin(pin), initial_state(initial_state), delay_ms(delay_ms) { }

void Pin::begin(){
    pinMode(pin, OUTPUT);     
    reset();
}

void Pin::reset(){
    setState(initial_state);
}

void Pin::setState(uint8_t st){
    digitalWrite(pin, st);  
}

void Pin::digitalPulse(unsigned char div){
    setState(1-digitalRead(pin));    
    wdt_reset();                          // RESET WATCHDOG PRIOR TO DELAY
    delay(delay_ms / div);
    wdt_reset();                          // RESET WATCHDOG AFTER TO DELAY
    setState(1-digitalRead(pin));            
}

// ------------
//   PRIVATE
// ------------