

/*
 * Title: PIN
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Auxiliary function for initialization, and read/write to IO
 */

void initialize_pins(){
  pinMode(PIN_CONTROL, OUTPUT);                 
  pinMode(PIN_LED,     OUTPUT);                 
  pinMode(PIN_BZR,     OUTPUT);                 
  digitalWrite(PIN_CONTROL, CONTROL_INIT_STATE);  
  digitalWrite(PIN_LED,     LED_INIT_STATE);
  digitalWrite(PIN_BZR,     BZR_INIT_STATE);
}

// PULSE PIN FOR TIME_ms
void digitalPulse(const char PIN, const unsigned short time_ms){  
  digitalWrite(PIN, 1-digitalRead(PIN));  
  wdt_reset();                          // RESET WATCHDOG PRIOR TO DELAY
  delay(time_ms);
  wdt_reset();                          // RESET WATCHDOG AFTER TO DELAY
  digitalWrite(PIN, 1-digitalRead(PIN));  
}

