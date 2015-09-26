
/*
 * Title: EEPROM
 * 
 * Author: Andre Madureira
 * 
 * Details:
 * Read/Write EEPROM Subroutines, Library and Protocol
 */

#include <EEPROM.h>

//READ EEPROM DATA 
void read_EEPROM(){    
  EEPROM.get(1017,master_code);  //READ MASTER CODE ID FROM [1017-1024]B OF EEPROM
  if (master_code == 0xFFFFFFFF) //EEPROM IS BEING USED FOR FIRST TIME
    master_code = DEFAULT_MASTER_CODE; //set master code to its default value      
  //Serial.print(F("MASTER CODE = "));
  //Serial.println(master_code);
  //START READING ALLOW_CODE ARRAY
  EEPROM.get(0,allow_code_len);
  if (allow_code_len > ALLOW_CODE_SIZE)  //CHECK IF IDS LEN IS VALID (EEPROM IS BEING USED FOR FIRST TIME)
    allow_code_len = 0;                  //LEN NOT VALID, MAKE IT DEFAULT VALUE
  for (short i = 0; i < allow_code_len; i++) //READ ALL IDS FROM EEPROM
    EEPROM.get(1+i*sizeof(RFID_CODE),allow_code[i]);  
}

//CHECK / WRITE RFID CODES TO EEPROM
void write_EEPROM(){
   //SAVE MASTER CODE TO EEPROM IF NEEDED
   if (save_master_code){   
     EEPROM.put(1017,master_code);  //SAVE MASTER CODE TO [1017-1024]B OF EEPROM   
     save_master_code = false;      //RESET NEED TO SAVE MASTER CODE     
   }
   //SAVE COMMON RFID TAGS TO EEPROM IF NEEDED
   if (save_to_EEPROM && elapsed_time(ITV_EEPROM)) { //WAIT TILL TIME COMES AND NEED TO SAVE DATA     
       EEPROM.put(0,allow_code_len);               //SAVE # OF IDS INTO 1ST BYTE OF EEPROM
       for (unsigned short i = 0; i < allow_code_len; i++)  //CHECK ALL IDS FOR THE NEED OF SAVING         
         EEPROM.put(1+i*sizeof(RFID_CODE),allow_code[i]);   //SAVE NEW ID IN EEPROM    
       save_to_EEPROM = false; //reset save to EEPROM       
   }  
}

//Update to EEPROM the RFID tags
void set_update_EEPROM(){
  prev_time = millis();    //SET ACTUAL TIME
  save_to_EEPROM = true;   //UPDATE EEPROM
}

