/*
 * Title: CONTROL
 * 
 * Author: Andre Madureira
 * 
 * Details:
 * Check for RFID, if valid, allow access to door 
 * (activate ac motor/relay)
 */

//PUT OUTPUT PIN_CONTROL HIGH FOR DELAY_CONTROL_MS MS
void activate_control(const RFID_CODE code){
  //Serial.print("ID Access Allowed - ");
  //Serial.println(code); 
  digitalPulse(PIN_CONTROL,DELAY_CONTROL_MS);
}

//CHECK IF ID IS REGISTERED AND VALID
void validate_rfid(const RFID_CODE code){  
  if (find_ID(code) != -1){ //FOUND ID, ACTIVATE CONTROL
    activate_control(code);
  } else {                  //ID NOT FOUND
    //Serial.print("Unregistered ID - ");        
    //Serial.println(code);   
    digitalPulse(PIN_BZR,BZR_BEEP_MS);
  }
}

//verify elapsed time
bool elapsed_time(unsigned long t){
  return ((unsigned long)(millis() - prev_time) >= t);
}

