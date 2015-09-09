
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
  if (ir.decode(&ir_res)) {              //Check for IR code receive    
    ir.resume();                         // Receive the next value    
    IR_IDCODE temp_ctrl;    
    for (int i = 0; i < NUM_CTRL_BTN; i++){
      memcpy_P(&temp_ctrl, control_codes+i, sizeof(IR_IDCODE));
      if (temp_ctrl.code == ir_res.value)
        return temp_ctrl.id;
    }
  }
  return NO_BTN_PRESSED;
}

void check_ir_s_modes(MasterFunction& num_master_fnc){
  IR_ID ir_id = read_ir();                              //IR BTN IDENTIFICATION  
  //CHECK FOR SPECIAL MODES AND BUTTON PRESS
  if (num_master_fnc == REGISTER || num_master_fnc == UNREGISTER){    
    if (ir_id != NO_BTN_PRESSED) {
      digitalPulse(PIN_LED,LED_BLINK_MS/2); //BLINK LED 
      unsigned int str_len = IR_str.length()+1;
      if (ir_id >= 0 && ir_id <= 9){
        IR_str += String(ir_id);
      } else if (ir_id == OK && str_len > 0) {        
        char code_char[str_len];
        IR_str.toCharArray(code_char,str_len);      
        switch (num_master_fnc){
          case REGISTER:
            register_rfid(strtoul(code_char,NULL,10));
            break; 
          case UNREGISTER:
            unregister_rfid(strtoul(code_char,NULL,10));
            break;
          default:
            break;
        }      
        IR_str = "";
        reset_master_fnc(num_master_fnc);
      } else if (ir_id == USD){      
        IR_str = "";
      }
    }
  }   
}

