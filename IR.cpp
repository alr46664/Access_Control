
#include "IR.h"

// ------------
//   PUBLIC
// ------------

IR::IR(State& state): ir(PIN_IR_RECV), state(state) {
    resetIRStr();
}

// enable IR receiver
void IR::begin(){
    ir.enableIRIn();               
}

RFID_CODE IR::getRFIDCode(){
    // REMOTE CONTROL BTN IDENTIFICATION
    IR_ID ir_id = this->read();             
    RFID_CODE code = 0;
    // REMOTE CONTROL BTN PRESSED
    if (ir_id != NO_BTN_PRESSED) {          
      state.blinkLED(2);
      // CHECK FOR NEED TO CHANGE MASTER CODE
      if (ir_id == MODE) {                  
        state.setMasterCodeChgMode();
      } else {
        code = getRFID_Str(ir_id);   // GET RFID CODE IF AVAILABLE
      }      
      wdt_reset();                           // reset watchdog      
    }  
    return code;
}

// ------------
//   PRIVATE
// ------------

// read raw IR signals
IR_ID IR::read(){
    decode_results ir_res;
    wdt_reset();
    if (ir.decode(&ir_res)) {      //Check for IR code receive        
        wdt_reset();
        ir.resume();    // IR Receive the next value after ir.decode()
        wdt_reset();
        IR_IDCODE temp_ctrl;    
        for (unsigned char i = 0; i < IR::NUM_CTRL_BTN; i++){
            wdt_reset();
            memcpy_P(&temp_ctrl, control_codes + i, sizeof(IR_IDCODE)); // temp_ctrl = control_codes[i]
            if (temp_ctrl.code == ir_res.value){
                return temp_ctrl.id;
            }
        }
    }
    return NO_BTN_PRESSED;
}

// read a string and parse it as RFID code
RFID_CODE IR::getRFID_Str(IR_ID& ir_id){
    // IS A NUMBER, THEN ADD TO rfidStr
    if (ir_id >= 0 && ir_id <= 9){         
        if (rfidStrLen + 1 >= IR::STR_LEN){ 
            // report error
            state.bzrBeep();
            resetIRStr(); 
            return 0;
        }
        // set IR to correct char representation/position
        rfidStr[rfidStrLen++] = ir_id + '0'; 
    } else if (ir_id == OK) { 
        // CONFIRM RFID        
        if (rfidStrLen <= 0) {
            // report error
            state.bzrBeep();
            resetIRStr(); 
            return 0;
        }
        // GET NON NULL RFID_CODE FORMAT FROM STRING
        RFID_CODE irRFID = strtoul(rfidStr, NULL, 10); 
        resetIRStr();
        return irRFID;
    } else if (ir_id == USD) {
        resetIRStr(); // reset IR string
    }
    return 0;
}

void IR::resetIRStr(){  
    memset(rfidStr, rfidStrLen = 0, IR::STR_LEN);   // RESET IR STRING      
}