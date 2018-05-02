#ifndef _Pin_H
#define _Pin_H

#include "Arduino.h" 
#include <avr/wdt.h>   // Watchdog Library (needs optiboot bootloader to work well)

#include "Project.h"       // global PROJECT defines

class Pin {
public:
    Pin(uint8_t pin, uint8_t initial_state, unsigned long delay_ms);
        
    void begin();
    void reset();
    void setState(uint8_t st);
    void digitalPulse(unsigned char div = 1);
private:    
    uint8_t pin, initial_state;
    unsigned long delay_ms;    
};

#endif  // _Pin_H