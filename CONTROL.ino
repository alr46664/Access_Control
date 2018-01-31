/*
 * Title: CONTROL
 * 
 * Author: Andre Madureira
 * 
 * Details:
 * Check for RFID, if valid, allow access to door 
 * (activate ac motor/relay)
 */

// DIGITAL PULSE PIN_CONTROL FOR DELAY_CONTROL_MS MS
void activate_control(const RFID_CODE& code){
  #ifdef DEBUG_MODE
  Serial.print(F("ID Access Allowed - "));
  Serial.println(code); 
  #endif    
  
  // print ACCESS ALLOWED
  oled_set_cursor_function();
  oled_println(oled_get_string(OLED_ACCESS_ALLOWED));  
  oled_println(oled_ltoa(code));
  
  digitalPulse(PIN_CONTROL,DELAY_CONTROL_MS);
}

// CHECK IF ID IS REGISTERED AND VALID
void validate_rfid(const RFID_CODE& code){  
  if (find_ID(code) != -1){ // FOUND ID, ACTIVATE CONTROL
    activate_control(code);
  } else {                  // ID NOT FOUND
    #ifdef DEBUG_MODE
    Serial.print(F("Unregistered ID - "));        
    Serial.println(code);   
    #endif
    
    oled_set_cursor_function();        
    oled_println(oled_get_string(OLED_UNREGISTERED));  
    oled_println(oled_ltoa(code));

    digitalPulse(PIN_BZR,BZR_BEEP_MS);
  }
}

//verify elapsed time
bool elapsed_time(unsigned long t){
  return ((unsigned long)(millis() - prev_time) >= t);
}

