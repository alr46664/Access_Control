
/*
 * Title: IR
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Control IR Receiver for Access, Registration and Unregistration
 */

#include <avr/pgmspace.h> //SAVE DATA INTO FLASH MEMORY
#include <IRremote.h> //IR Library

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
    if (ir_id == MODE) { //CHECK FOR NEED TO CHANGE MASTER CODE
      chg_master_code = 0x1;                
      //Serial.println(F("CHANGE MASTER"));
    }
    RFID_CODE code = get_ir_rfid(ir_id);   //GET RFID CODE IF AVAILABLE
    wdt_reset();                   //reset watchdog
    if (code != 0) {
      switch (num_master_fnc){
        case ACCESS_NULL:
          if (chg_master_code == 0x1) { //check if chg master mode is on
            if (code == master_code) chg_master_code = 0x3; //check for correct actual master code
            else                     chg_master_code = 0x0; //incorrect master code, chg mode off
          } else if (chg_master_code == 0x3) {
            master_code = code; //update master code
            //Serial.print(F("NEW MASTER = "));
            //Serial.println(master_code);
            save_master_code = true; //update master code in EEPROM          
            chg_master_code = 0; //reset change master
            digitalPulse(PIN_LED,LED_BLINK_MS); //BLINK LED               
          }
          break;
        case REGISTER:  
          chg_master_code = 0; //reset change master
          register_rfid(code);           
        case UNREGISTER:
          chg_master_code = 0; //reset change master
          unregister_rfid(code);           
          break;
        default:
          break;
      }                         
    }
  }  
}

//GET RFID CODE FROM IR RECEPTOR, IF CODE NOT CONFIRMED RETURN 0
RFID_CODE get_ir_rfid(IR_ID& ir_id){  
  if (ir_id >= 0 && ir_id <= 9){  //IS A NUMBER, THEN ADD TO IR_str
    if (IR_count+1 >= IR_STR_LEN){ //IF OVERFLOW, REPORT ERROR
      digitalPulse(PIN_BZR,BZR_BEEP_MS/2); //report error
      resetIRStr(); //reset IR string
      return 0;
    }
    IR_str[IR_count++] = ir_id+'0'; //set IR to correct char representation/position
  } else if (ir_id == OK) { //CONFIRM RFID        
    if (IR_count <= 0){ //IF CODE IS NULL, REPORT ERROR
      digitalPulse(PIN_BZR,BZR_BEEP_MS); //report error
      return 0;
    }
    //GET NON NULL RFID_CODE FORMAT FROM STRING
    RFID_CODE IR_RFID = strtoul(IR_str,NULL,10); //PARSE RFID_CODE FROM STRING         
    resetIRStr(); //reset IR string
    return IR_RFID;
  } else if (ir_id == USD){      
    resetIRStr(); //reset IR string
  }  
  return 0;
}

//RESET THE IR STRING TO UNSIGNED LONG 0
void resetIRStr(){  
  memset(IR_str,IR_count = 0,IR_STR_LEN);   //RESET IR STRING      
}

