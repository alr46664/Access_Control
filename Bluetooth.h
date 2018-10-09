#ifndef _Bluetooth_H
#define _Bluetooth_H

#include "Arduino.h" 

#include <avr/wdt.h>       // Watchdog Library (needs optiboot bootloader to work well)
#include <AltSoftSerial.h> // software serial UART
#include <Scheduler.h>  // Scheduler class
#include <avr/pgmspace.h>  // read flash as datA  

#include "Project.h"   // global PROJECT defines
#include "IDManager.h"  // RFID Tag Manager - Read EEPROM - Write EEPROM

// bluetooth pins below
#define  BLE_TX     8
#define  BLE_RX     9
#define  BLE_CONN  10

// door bluetooth name (maximum 9 chars)
#define BLE_NAME    "hart3"
// bluetooth password (needs to be 6 numbers)
#define BLE_PASSWD  "001921"

// MAXIMUM BYTES ALLOWED TO BE SEND/RECV AT ONCE
#define  BLE_MAX_BYTE   20

// bluetooth delay required to receive/send data 
#define  BLE_TRANSMISSION_DELAY  150

// bluetooth timeout for data receival (in ms)
#define  BLE_RECV_TIMEOUT  5000

// initial ble state
#define  BLE_INITIAL_STATE (BLE_UNLOCK)

// MACROS
#define haveReceivedData(command_p) (strstr_P(buffer, command_p) != NULL)

// indicates that the receiver confirmed the msg
static const char BLE_RECV_CONFIRM[] PROGMEM = "OK";
// terminator character that indicates end of transmission
static const char BLE_RECV_TERMINATOR[] PROGMEM = "-";

enum BLEStateSync {BLE_UNLOCK = 0, BLE_LOCK, LIST, ADD, DELETE, GETMASTER, SETMASTER, COMMIT, ROLLBACK};     

class Bluetooth {
public:        
    Bluetooth(IDManager* manager);  

    void begin(int baudrate = 9600);  
    void execute();
    bool isConnected();
private:  
    Scheduler readTimer;      
    // bluetooth serial interface
    AltSoftSerial bt;
    // EEPROM RFID local storage 
    IDManager* manager;
    // buffer for communications
    char buffer[BLE_MAX_BYTE];
    // system ongoing operation 
    BLEStateSync state;
    // counter of received data
    int recvCounter;

    // main callbacks here
    void setState(BLEStateSync newState);
    void processData();

    // specific commands here
    void runList();
    void runAdd();
    void runDelete();
    void runGetMaster();
    void runSetMaster();
    void runCommit();
    void runRollback();

    // read bluetooth data
    size_t read();

    // indicates if the last data recv is an ACK
    bool hasReceivedConfirmation();
    
    //ack the receival of data
    void sendReceiveACK();
    
    // send data and flush it
    void send(const __FlashStringHelper * str, bool useDelay);    
    void send(RFID_CODE code, bool useDelay);
    void send(const char* str, bool useDelay);      
    
    // clear serial buffer
    void clearSerialBuffer();
    
};


#endif  // _Bluetooth_H 
