

/*
 * Title: PIN
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Auxiliary function for initialization, and read/write to IO
 */

void initialize_pins(){
  pinMode(PIN_CONTROL,OUTPUT);   //INITIALIZE OUTPUT CONTROL PIN TO LOW
  pinMode(PIN_LED,OUTPUT);       //INITIALIZE OUTPUT LED PIN TO HIGH (LED ON)  
  pinMode(PIN_BZR,OUTPUT);       //INITIALIZE OUTPUT BZR PIN TO HIGH (BZR OFF)
  digitalWrite(PIN_CONTROL,LOW);  
  digitalWrite(PIN_LED,LED_INIT_STATE);
  digitalWrite(PIN_BZR,HIGH);
}

//PULSE PIN FOR TIME_ms
void digitalPulse(const char PIN, const unsigned short time_ms){  
  digitalWrite(PIN,1-digitalRead(PIN));  
  wdt_reset();                          //RESET WATCHDOG PRIOR TO DELAY
  delay(time_ms);
  wdt_reset();                          //RESET WATCHDOG AFTER TO DELAY
  digitalWrite(PIN,1-digitalRead(PIN));  
}

