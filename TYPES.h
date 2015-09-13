

/*
 * Title: RFID.h
 * 
 * Author: Andre Madureira
 * 
 * Details:
 * Constants and custom types definitions of the project
 */

#ifndef TYPES_H
#define TYPES_H 

//Defining header guard

#define PIN_CONTROL         6          //# OF DIGITAL PIN TO CONTROL
#define DELAY_CONTROL_MS    600        //KEEP HIGH OUTPUT IN PIN_CONTROL FOR # MS

#define PIN_IR_RECV         9          //Pin connected to IR Receiver
#define NUM_CTRL_BTN        21         //# OF BUTTONS OF THE REMOTE CONTROL

#define PIN_LED             4          //OUTPUT LED
#define LED_BLINK_MS        400        //MS TO BLINK LED 
#define LED_INIT_STATE      LOW        //define led initial state

#define PIN_BZR             5          //OUTPUT BUZZER
#define BZR_BEEP_MS         400        //MS TO MANAGE FREQUENCY BZR

#define DEFAULT_MASTER_CODE 26121L     //SET DEFAULT MASTER CODE IN CASE EEPROM HAS NEVER BEING USED
#define ITV_MASTER_ACCESS   5000       //INTERVAL(ms) BETWEEN WHICH DOOR ACCESS IS ALLOWED FOR MASTER CODE
#define MAX_NUM_MASTER_FNC  3          //# OF FUNCTIONS OF THE MASTER ID

#define ALLOW_CODE_SIZE     50         //MAX # OF IDS STORABLE - LIMIT IS 255 (BYTE)

#define ITV_EEPROM          20000      //INTERVAL(ms) TO WAIT FOR OTHER TAGS TO BE (UN)REGISTERED, THEN SAVE TO EEPROM

typedef unsigned long RFID_CODE;   //RFID CODE VARIABLE TYPE DEF

enum MasterFunction {ACCESS_NULL = 0, ACCESS, REGISTER, UNREGISTER}; //FUNCTIONS OF MASTER ID

enum IR_ID {    //ID of IR code
  ZERO = 0,
  ONE,TWO,THREE,
  FOUR,FIVE,SIX,
  SEVEN,EIGHT,NINE,  
  OK,USD,
  PWR,MODE,MUTE,
  PLAY,PREV,NEXT,
  EQ,MINUS,PLUS,
  NO_BTN_PRESSED
};

typedef struct {
  IR_ID id;            //ID of the IR code
  unsigned long code;  //IR Code
} IR_IDCODE;

typedef struct {
  char pin;                //Pin to change
  unsigned short time_ms;  //Time interval (ms) to check
} PIN_PULSE;

//End of header guard

#endif TYPES_H 
