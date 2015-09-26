
/*
 * Title: REGISTERING
 * 
 * Author: Andre Madureira
 * 
 * Details:
 * Validation, Registration and Unregistration of RFID's
 */

// Return -1 if card is not found, otherwise return >0
short find_ID(const RFID_CODE code){
  for (short i = 0; i < allow_code_len; i++){
    if (code == allow_code[i])
      return i;      
  }
  return -1;
}

//REGISTER NEW ID IF WE HAVE STORAGE SPACE AVAILABLE
void register_rfid(const RFID_CODE code){
  if (find_ID(code) != -1){ //ID ALREADY REGISTERED  
    //Serial.print(F("Registration Warning - ID Already Registered: "));
    //Serial.println(code);    
    return;
  }
  if (allow_code_len+1 < ALLOW_CODE_SIZE){ //CHECK STORAGE SPACE AVAILABILITY
    allow_code[allow_code_len++] = code; //STORE NEW CODE        
    set_update_EEPROM();                 //UPDATE EEPROM WHEN TIME COMES
    //Serial.print(F("RFID Code: ")); 
    //Serial.print(code);
    //Serial.println(F(" - Registered"));    
    //print_n_acc_codes();
  } else {
    //Serial.print(F("Registration error - Exceeded maximum number of ID's: "));
    //Serial.println(ALLOW_CODE_SIZE);
    digitalPulse(PIN_BZR,BZR_BEEP_MS);  
  }
}

//UNREGISTER ID IF IT'S AN ALLOWED ONE
void unregister_rfid(const RFID_CODE code){
  const short ID_POS = find_ID(code);
  if (ID_POS != -1) {           //CHECK IF ID IS AN ALLOWED ID
    allow_code[ID_POS] = allow_code[--allow_code_len]; //UNREGISTER ID    
    set_update_EEPROM();                               //UPDATE EEPROM WHEN TIME COMES
    //Serial.print(F("RFID Code: "));
    //Serial.print(code);
    //Serial.println(F(" - Unregistered")); 
    //print_n_acc_codes();   
  } else {
    //Serial.print(F("Unregistration error - Could not find ID: "));
    //Serial.println(code);
    digitalPulse(PIN_BZR,BZR_BEEP_MS);    
  }  
}

//PRINT # OF IDS TO SERIAL
void print_n_acc_codes(){
  Serial.print(allow_code_len);  
  Serial.println(F(" Access Codes Registered"));
}
