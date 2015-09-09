
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
  for (short i = 0; i < allow_code_len; i++){ //READ ALL IDS FROM EEPROM
    EEPROM.get(1+i*sizeof(RFID_CODE),allow_code[i]);
  }
}

//WRITE DATA TO EEPROM
void write_codes_EEPROM(const unsigned char POS, const RFID_CODE code){
   EEPROM.put(1+POS*sizeof(RFID_CODE),code); //SAVE NEW ID IN EEPROM
   EEPROM.put(0,allow_code_len);        //SAVE # OF IDS INTO 1ST BYTE OF EEPROM
}
