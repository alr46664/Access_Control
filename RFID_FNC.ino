
/*
 * Title: RFID_FNC
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Auxiliary functions for RFID Functions
 */

void reset_master_fnc(){
  digitalWrite(PIN_LED,LED_INIT_STATE); // LED IN ITS INITIAL STATE
  num_master_fnc = ACCESS_NULL;         // RESET MASTER FUNCTION COUNT  
  resetIRStr(); // RESET IR string, when RFID is activated, IR should be cleared
  chg_master_code = 0; // STOP MASTER CODE CHANGE
}

MasterFunction next_master_fnc(){
  num_master_fnc = static_cast<MasterFunction>((num_master_fnc+1) % (MAX_NUM_MASTER_FNC+1)); // SET TO [1,MAX_NUM_MASTER_FNC]
  prev_time = millis();       // COUNT TIME  
  switch(num_master_fnc){
    case ACCESS_NULL:
      #ifdef DEBUG_MODE
      Serial.println(F("Access Null Mode"));
      #endif
      digitalWrite(PIN_LED,LED_INIT_STATE); // LED IN ITS INITIAL STATE
      break;
    case ACCESS:                              
      #ifdef DEBUG_MODE
      Serial.println(F("Access Mode"));
      #endif
      break;                  
    case REGISTER:
      #ifdef DEBUG_MODE
      Serial.println(F("Registration Mode"));
      #endif
      digitalWrite(PIN_LED,1-LED_INIT_STATE); // LED OPPOSITE STATE OF INITIAL          
      break;          
    case UNREGISTER:
      #ifdef DEBUG_MODE
      Serial.println(F("Unregistration Mode"));
      #endif
      digitalPulse(PIN_LED,LED_BLINK_MS); // BLINK LED              
      break;                             
  }
  return num_master_fnc;
}

void execute_rfid_fnc(RFID_CODE code){
  resetIRStr(); // RESET IR string, when RFID is activated, IR should be cleared
  chg_master_code = 0; // STOP MASTER CODE CHANGE
  if (code == master_code){    // CHECK FOR MASTER ID AND FUNCTION      
      next_master_fnc(); // CHANGE TO NEXT MASTER FUNCTION (ACCESS -> REGISTER -> UNREGISTER)      
    } else {                    // ID IS NON-MASTER      
      switch(num_master_fnc){   // CHECK FUNCTION 
        case ACCESS_NULL:                 // NO MASTER ID PASSED          
        case ACCESS:                 
          validate_rfid(code);
          break;          
        case REGISTER:                  // REGISTER FUNCTION (PASS 1-TIME MASTER ID)
          register_rfid(code);   // RECORD NEW ID          
          break;          
        case UNREGISTER:                  // UNREGISTER FUNCTION (PASS 2-TIMES MASTER ID)
          unregister_rfid(code);          
          break;                          
      }      
    }
}

// CHECK FOR MASTER FUNCTION ELAPSED TIMED PROCEDURES
void check_elapsed_master_fnc(){  
  if ((num_master_fnc == ACCESS) && elapsed_time(ITV_MASTER_ACCESS)) {
    reset_master_fnc();    // RESET COUNTERS 
    #ifdef DEBUG_MODE
    Serial.println(F("Access Allowed"));
    #endif
    activate_control(master_code);   // MASTER ID PASSED, ACTIVATE CONTROL          
  } else if ((num_master_fnc == REGISTER || num_master_fnc == UNREGISTER) \
            && elapsed_time(ITV_EEPROM)) {  
    reset_master_fnc();    // RESET COUNTERS 
  }  
}
