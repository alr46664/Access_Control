
/*
 * Title: EEPROM
 * 
 * Author: Andre Madureira
 * 
 * Details:
 * Read/Write EEPROM Subroutines, Library and Protocol
 */

#include <EEPROM.h>

//READ DATA FROM EEPROM
void read_codes_EEPROM(){
  EEPROM.get(0,allow_code_len);
  if (allow_code_len >= ALLOW_CODE_SIZE) //CHECK IF IDS LEN IS VALID
    allow_code_len = 0;                  //LEN NOT VALID, MAKE IT DEFAULT VALUE
  for (short i = 0; i < allow_code_len; i++) //READ ALL IDS FROM EEPROM
    EEPROM.get(1+i*sizeof(RFID_CODE),allow_code[i]);  
}

//CHECK / WRITE DATA TO EEPROM
void write_codes_EEPROM(){
   if ((save_to_EEPROM != 0) && ((unsigned long)(millis() - prev_time) >= ITV_EEPROM)) { //WAIT TILL TIME COMES AND NEED TO SAVE DATA     
     EEPROM.put(0,allow_code_len);                //SAVE # OF IDS INTO 1ST BYTE OF EEPROM
     for (short i = 0; i < allow_code_len; i++)  //CHECK ALL IDS FOR THE NEED OF SAVING
       if ((save_to_EEPROM >> i) & 1 == 1)      //if true, this id needs to be updated         
         EEPROM.put(1+i*sizeof(RFID_CODE),allow_code[i]);   //SAVE NEW ID IN EEPROM        
     save_to_EEPROM = 0;    //all codes saved, stop EEPROM SAVING
     reset_master_fnc();    //reset system mode to access     
   }
}

//Update to EEPROM the RFID tags marked by the MASK, using an OR operation
void set_update_EEPROM(unsigned long mask){
  prev_time = millis();    //SET ACTUAL TIME
  save_to_EEPROM |= mask;  //ADD NEW ID TO UPDATE
}

