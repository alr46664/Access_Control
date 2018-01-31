
/*
 * Title: IR
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Control IR Receiver for Access, Registration and Unregistration
 */

#include <avr/pgmspace.h> //SAVE DATA INTO FLASH MEMORY
#include <IRremote.h> //IR Library (https://github.com/z3t0/Arduino-IRremote)

IRrecv ir(PIN_IR_RECV);    // IR sensor

                                                    //VARIABLE HAS TO BE STATIC OR GLOBAL FOR PROGMEM TO WORK
const static IR_IDCODE control_codes[] PROGMEM = {  //PROGMEM TO SAVE VARIABLE INTO FLASH
  {IR_PWR,0xffa25d}, {IR_MODE,0xff629d}, {IR_MUTE,0xffe21d},
  {IR_PLAY,0xff22dd}, {IR_PREV,0xff02fd}, {IR_NEXT,0xffc23d},
  {IR_EQ,0xffe01f}, {IR_MINUS,0xffa857}, {IR_PLUS,0xff906f},
  {IR_ZERO,0xff6897}, {IR_OK,0xff9867}, {IR_USD,0xffb04f},
  {IR_ONE,0xff30cf}, {IR_TWO,0xff18e7}, {IR_THREE,0xff7a85},
  {IR_FOUR,0xff10ef}, {IR_FIVE,0xff38c7}, {IR_SIX,0xff5aa5},
  {IR_SEVEN,0xff42bd}, {IR_EIGHT,0xff4ab5}, {IR_NINE,0xff52ad}
};

// START IR RECEIVER                    
void init_ir(){
  ir.enableIRIn();
}

// Return NO_BTN_PRESSED if no IR signal was received, otherwise return IR_ID
IR_ID read_ir(){
  decode_results ir_res;
  if (ir.decode(&ir_res)) {      //Check for IR code receive        
    ir.resume();    // IR Receive the next value after ir.decode()
    IR_IDCODE temp_ctrl;    
    for (int i = 0; i < NUM_CTRL_BTN; i++){
      memcpy_P(&temp_ctrl, control_codes+i, sizeof(IR_IDCODE)); //temp_ctrl = control_codes[i]
      if (temp_ctrl.code == ir_res.value)
        return temp_ctrl.id;
    }
  }
  return IR_NO_BTN_PRESSED;
}

// CHECK IR RECEPTION
void check_ir(){
  IR_ID ir_id = read_ir();                // REMOTE CONTROL BTN IDENTIFICATION
  if (ir_id == IR_NO_BTN_PRESSED) {          
    // REMOTE CONTROL NO BTN PRESSED, stop function
    return;
  }
  
  digitalPulse(PIN_LED,LED_BLINK_MS/2); // BLINK LED     
  wdt_reset();                          // reset watchdog
  if (ir_id == IR_MODE) {                  // CHECK FOR NEED TO CHANGE MASTER CODE
    if (chg_master_code == 0) {
      chg_master_code = CNG_MASTER;     // set master change mode
                                        // if mode was pressed more than once then change master fnc, 
                                        // and reset cng_master_code as we roll back        
      // DISPLAY CHANGE MASTER      
      oled_set_cursor_function();
      oled_println(oled_get_string(OLED_CHANGE_MASTER));
      
    } else if (chg_master_code == CNG_MASTER || chg_master_code == CNG_MASTER_NOCNG){ 
      // this restricts the change master code only in normal access mode
      chg_master_code = CNG_MASTER_NOCNG;      // do not change master code
      if (next_master_fnc() == ACCESS_NULL) {         
        chg_master_code = 0;                   // reset change master code
      }
      
      // clear the change master line      
      oled_set_cursor_function();
    }
    #ifdef DEBUG_MODE
    Serial.println(F("CHANGE MASTER"));
    #endif
  }
  
  RFID_CODE code = get_ir_rfid(ir_id);   // GET RFID CODE IF AVAILABLE
  wdt_reset();                           // reset watchdog  
  if (code == 0) {                       
    // no rfid has been read from IR, return
    return;
  }

  // display code screen
  oled_set_cursor_code();
  oled_println(oled_ltoa(code));
  
  switch (num_master_fnc){
    case ACCESS_NULL:
      if (chg_master_code == CNG_MASTER) {      // check if chg master mode is on
        if (code == master_code) {
          chg_master_code = CNG_MASTER_CONFIRM; // check PASS for correct actual master code
        } else {
          chg_master_code = 0x0;                // incorrect master code, chg mode off
          digitalPulse(PIN_BZR,BZR_BEEP_MS/2);  // report error
        }
      } else if (chg_master_code == CNG_MASTER_CONFIRM) {
        master_code = code;                     // update master code
        
        #ifdef DEBUG_MODE
        Serial.print(F("NEW MASTER = "));
        Serial.println(master_code);
        #endif
        
        oled_set_cursor_function();
        oled_println(oled_get_string(OLED_NEW_MASTER));
        oled_println(oled_ltoa(master_code));
                
        save_master_code = true;            // update master code in EEPROM          
        chg_master_code = 0;                // reset change master
        digitalPulse(PIN_LED,LED_BLINK_MS); // BLINK LED               
      }
      break;
    case REGISTER:  
      //chg_master_code = CNG_MASTER_NOCNG; // do not change master code
      register_rfid(code);           
    case UNREGISTER:
      //chg_master_code = CNG_MASTER_NOCNG; // do not change master code
      unregister_rfid(code);           
      break;
    default:
      break;
  }                         
  
}

// GET RFID CODE FROM IR RECEPTOR, IF CODE NOT CONFIRMED RETURN 0
RFID_CODE get_ir_rfid(IR_ID& ir_id){  
  if (ir_id >= 0 && ir_id <= 9){  
                 
    // IS A NUMBER, THEN ADD TO IR_str
    if (IR_count+1 >= IR_STR_LEN){         // IF OVERFLOW, REPORT ERROR
      digitalPulse(PIN_BZR,BZR_BEEP_MS/2); // report error

      // clear oled code screen
      oled_set_cursor_code(); 
      
      reset_ir_str(); //reset IR string
      return 0;
    }
    IR_str[IR_count++] = ir_id+'0';        // set IR to correct char representation/position
    
  } else if (ir_id == IR_OK) {                // CONFIRM RFID        
    
    if (IR_count <= 0){                    // IF CODE IS NULL, REPORT ERROR
      digitalPulse(PIN_BZR,BZR_BEEP_MS);   // report error

      // clear oled code screen
      oled_set_cursor_code();    
      
      return 0;
    }
    // GET NON NULL RFID_CODE FORMAT FROM STRING
    RFID_CODE IR_RFID = strtoul(IR_str,NULL,10); // PARSE RFID_CODE FROM STRING         
    reset_ir_str(); // reset IR string
    return IR_RFID;
    
  } 
  else if (ir_id == IR_USD){  
        
    reset_ir_str(); // reset IR string
    
  }  
  return 0;
}

// RESET THE IR STRING TO UNSIGNED LONG 0
void reset_ir_str(){  
  memset(IR_str,IR_count = 0,IR_STR_LEN);   // RESET IR STRING      
}

