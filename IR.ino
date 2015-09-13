
/*
 * Title: IR
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Control IR Receiver for Access, Registration and Unregistration
 */

#include <avr/pgmspace.h> //SAVE DATA INTO FLASH MEMORY
                                                    //VARIABLE HAS TO BE STATIC OR GLOBAL FOR PROGMEM TO WORK
const static IR_IDCODE control_codes[] PROGMEM = {  //PROGMEM TO SAVE VARIABLE INTO FLASH
  {PWR,0xffa25d}, {MODE,0xff629d}, {MUTE,0xffe21d},
  {PLAY,0xff22dd}, {PREV,0xff02fd}, {NEXT,0xffc23d},
  {EQ,0xffe01f}, {MINUS,0xffa857}, {PLUS,0xff906f},
  {ZERO,0xff6897}, {OK,0xff9867}, {USD,0xffb04f},
  {ONE,0xff30cf}, {TWO,0xff18e7}, {THREE,0xff7a85},
  {FOUR,0xff10ef}, {FIVE,0xff38c7}, {SIX,0xff5aa5},
  {SEVEN,0xff42bd}, {EIGHT,0xff4ab5}, {NINE,0xff52ad}
};

//Return NO_BTN_PRESSED if no IR signal was received, otherwise return IR_ID
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
  return NO_BTN_PRESSED;
}

//CHECK IR RECEPTION
void check_ir(){
  IR_ID ir_id = read_ir();       //REMOTE CONTROL BTN IDENTIFICATION
  if (ir_id != NO_BTN_PRESSED) { //REMOTE CONTROL BTN PRESSED
    digitalPulse(PIN_LED,LED_BLINK_MS/2); //BLINK LED 
    unsigned int str_len = IR_str.length()+1; //GET STRING SIZE    
    if (ir_id == MODE)  //CHECK FOR NEED TO CHANGE MASTER CODE
      chg_master_code = true;                
    RFID_CODE code = get_ir_rfid(ir_id,str_len);   //GET RFID CODE IF AVAILABLE
    wdt_reset();                   //reset watchdog
    if (code != 0) {
      switch (num_master_fnc){
        case ACCESS_NULL:
          if (chg_master_code) {
            master_code = code; //update master code
            save_master_code = true; //update master code in EEPROM          
            digitalPulse(PIN_LED,LED_BLINK_MS); //BLINK LED               
          }
          break;
        case REGISTER:  
          register_rfid(code);           
        case UNREGISTER:
          unregister_rfid(code);           
          break;
        default:
          break;
      }                   
      chg_master_code = false; //reset change master
    }
  }  
}

//GET RFID CODE FROM IR RECEPTOR, IF CODE NOT CONFIRMED RETURN 0
RFID_CODE get_ir_rfid(IR_ID& ir_id, unsigned int& str_len){  
  if (ir_id >= 0 && ir_id <= 9){  //IS A NUMBER, THEN ADD TO IR_str
    IR_str += String(ir_id);
  } else if (ir_id == OK && str_len > 0) { //CONFIRM RFID IF CODE IS NON NULL
    //GET RFID_CODE FORMAT FROM STRING
    char code_char[str_len];
    IR_str.toCharArray(code_char,str_len);                  
    IR_str = "";       
    return strtoul(code_char,NULL,10); //RETURN RFID_CODE      
  } else if (ir_id == USD){      
    IR_str = "";
  }  
  return 0;
}

