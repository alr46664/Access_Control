 
#include "Bluetooth.h"

// -------------
//    PUBLIC
// -------------

Bluetooth::Bluetooth(IDManager* manager): 
  manager(manager),
  bt(BLE_TX, BLE_RX),
  readTimer(BLE_RECV_TIMEOUT) {
    buffer[0] = 0;
    pinMode(BLE_TX, INPUT);
    pinMode(BLE_RX, OUTPUT);    
    pinMode(BLE_CONN, INPUT);
    setState(BLE_INITIAL_STATE);
}

void Bluetooth::begin(int baudrate){    
    // initialize bluetooth comm
    char command[BLE_MAX_BYTE];
    bt.begin(baudrate);
    // configure bluetooth    
    wdt_reset();   
    delay(600);
    wdt_reset();   
    send(F("AT+ROLE0"), false);  // set bluetooth as slave          
    wdt_reset();
    send(F("AT+UUIDFFE0"), false); // define custom service uuid    
    clearSerialBuffer();
    wdt_reset();
    send(F("AT+CHARFFE1"), false); // define custom text characteristic uuid    
    wdt_reset();
    clearSerialBuffer();
    
    // set dev name
    strcpy_P(command, PSTR("AT+NAME")); 
    strcat_P(command, PSTR(BLE_NAME));    
    wdt_reset();    
    send(command, false);     
    wdt_reset();
    send(F("AT+TYPE2"), false); // activate pin auth         
    wdt_reset();
    clearSerialBuffer();

    // set 6-number pin password    
    strcpy_P(command, PSTR("AT+PIN")); 
    strcat_P(command, PSTR(BLE_PASSWD));    
    wdt_reset();
    send(command, false);         
    wdt_reset();
    clearSerialBuffer();
}  

void Bluetooth::execute(){                
    while (bt.available()){
      Serial.write(bt.read());
    }
    while (Serial.available()){      
      bt.write(Serial.read());
    }
    return;

    // read bluetooth data
    if (!bt.available()){
      return;
    }
    wdt_reset();
    read();      
    wdt_reset();  
    #ifdef DEBUG_MODE              
    Serial.println(F("BLE - data received"));        
    #endif
    // check for bluetooth commands here    
    if (haveReceivedData(PSTR("list"))){          
        setState(LIST);
    } else if (haveReceivedData(PSTR("add"))){        
        setState(ADD);
    } else if (haveReceivedData(PSTR("delete"))){        
        setState(DELETE);
    } else if (haveReceivedData(PSTR("getmaster"))){        
        setState(GETMASTER);
    } else if (haveReceivedData(PSTR("setmaster"))){        
        setState(SETMASTER);
    } else if (haveReceivedData(PSTR("commit"))){                        
        setState(COMMIT);        
    } else if (haveReceivedData(PSTR("rollback"))){                
        setState(ROLLBACK);
    } else {
        processData();
    }
}

bool Bluetooth::isConnected(){
    bool BLEConnected = digitalRead(BLE_CONN);
    if ((!BLEConnected) && (state != BLE_INITIAL_STATE)){    
        #ifdef DEBUG_MODE
        Serial.println(state);
        #endif
        wdt_reset();
        runRollback();
        setState(BLE_INITIAL_STATE);
    }         
    return BLEConnected;
}

// -------------
//    PRIVATE
// -------------

void Bluetooth::setState(BLEStateSync newState){  
  wdt_reset();
  recvCounter = -1;
  state = newState;
  buffer[0] = 0;
  switch (state){
  case LIST:    
    sendReceiveACK();
    delay(BLE_TRANSMISSION_DELAY);
    #ifdef DEBUG_MODE
    Serial.println(F("state = list"));
    #endif
    send(manager->getTotalCards(), false);
    break;
  case ADD:    
    sendReceiveACK();
    #ifdef DEBUG_MODE
    Serial.println(F("state = add"));
    #endif
    break;
  case DELETE:
    sendReceiveACK();    
    #ifdef DEBUG_MODE
    Serial.println(F("state = delete"));
    #endif
    break;
  case GETMASTER:
    sendReceiveACK();
    #ifdef DEBUG_MODE
    Serial.println(F("state = get master"));
    #endif
    runGetMaster();
    break;
  case SETMASTER:
    sendReceiveACK();   
    #ifdef DEBUG_MODE
    Serial.println(F("state = set master"));
    #endif 
    break;
  case COMMIT:    
    #ifdef DEBUG_MODE
    Serial.println(F("state = commit"));
    #endif
    runCommit();
    break;
  case ROLLBACK:
    #ifdef DEBUG_MODE
    Serial.println(F("state = rollback"));
    #endif
    runRollback();
    break;
  case BLE_LOCK:
    #ifdef DEBUG_MODE
    Serial.println(F("state = locked"));
    #endif    
    break;
  case BLE_UNLOCK:    
    #ifdef DEBUG_MODE
    Serial.println(F("state = unlock = initial"));
    #endif
    clearSerialBuffer();
    break;
  default:
    break;
  }    
}

void Bluetooth::processData(){
  wdt_reset();
  switch (state){
  case LIST:
    runList();
    break;
  case ADD:
    runAdd();
    break;
  case DELETE:
    runDelete();
    break;
  case GETMASTER:
    break;
  case SETMASTER:
    runSetMaster();
    break;
  case COMMIT:
    break;
  case ROLLBACK:
    break;
  default:  
    break;
  }  
}

void Bluetooth::runList(){   
    if (hasReceivedConfirmation() && ++recvCounter < manager->getTotalCards()){
      // send card
      send(manager->getCard(recvCounter), false);
    } else {      
      setState(BLE_LOCK);
    }        
}

void Bluetooth::runAdd(){  
    if (recvCounter == -1)  {
      recvCounter = (int) strtol(buffer, NULL, 10);           
    } else if (recvCounter-- > 0){
        RFID_CODE code = strtoul(buffer, NULL, 10);    
        if (code == 0 || code == ULONG_MAX){
            setState(ROLLBACK);
            return;
        }           
        manager->registerID(code);                
    }     
    if (recvCounter == 0) {
      setState(BLE_LOCK);
    }        
    sendReceiveACK();
}

void Bluetooth::runDelete(){
    if (recvCounter == -1)  {
      recvCounter = (int) strtol(buffer, NULL, 10);       
    } else if (recvCounter-- > 0){
        RFID_CODE code = strtoul(buffer, NULL, 10);    
        if (code == 0 || code == ULONG_MAX){
            setState(ROLLBACK);
            return;
        }           
        manager->unregisterID(code);                
    }   
    if (recvCounter == 0) {
      setState(BLE_LOCK);
    }        
    sendReceiveACK();
}

void Bluetooth::runGetMaster(){
    delay(BLE_TRANSMISSION_DELAY);
    send(manager->getMasterCode(), false);
    setState(BLE_LOCK);
}

void Bluetooth::runSetMaster(){
    RFID_CODE code = strtoul(buffer, NULL, 10);        
    if (code == 0 || code >= ULONG_MAX){
        setState(ROLLBACK);
        return;
    }        
    manager->setMasterCode(code);
    setState(BLE_LOCK);
    sendReceiveACK();
}

void Bluetooth::runCommit(){        
    if (manager->hasUnsavedData()){
        manager->write_EEPROM();
    }
    setState(BLE_INITIAL_STATE);
    sendReceiveACK();
}

void Bluetooth::runRollback(){
    wdt_reset();
    manager->begin();
    setState(BLE_INITIAL_STATE);
    sendReceiveACK();
}

size_t Bluetooth::read(){
    // read bluetooth recv data      
    char terminator[2];        
    size_t count = 0;
    buffer[0] = 0;
    strcpy_P(terminator, &BLE_RECV_TERMINATOR[0]);
    wdt_reset();    
    readTimer.start();
    while ((buffer[count++] = bt.read()) != terminator[0]) {
      wdt_reset();
      while(!bt.available()){ 
        wdt_reset();
        if (readTimer.hasTimeoutHappened()){          
          break;
        }
      }
      if (readTimer.hasTimeoutHappened() || count == BLE_MAX_BYTE){
        break;      
      }
    }    
    wdt_reset();
    return count;
}

void Bluetooth::sendReceiveACK(){
    send((const __FlashStringHelper *) &BLE_RECV_CONFIRM[0], false);    
}

void Bluetooth::send(const __FlashStringHelper * str, bool useTerminator){
    char command[BLE_MAX_BYTE];
    strcpy_P(command, (PGM_P) str);
    send(command, useTerminator);
}

void Bluetooth::send(RFID_CODE code, bool useTerminator){
    char code_str[BLE_MAX_BYTE];
    snprintf(code_str, BLE_MAX_BYTE, "%lu", code);
    send(code_str, useTerminator);
}

void Bluetooth::send(const char* str, bool useTerminator){    
    wdt_reset();
    int len = strlen(str);
    for (int i = 0; i < len; i++){
      bt.write(str[i]);      
      wdt_reset();      
    }
    bt.write('\r');    
    bt.write('\n');    
    wdt_reset();     
    bt.flush();    
    wdt_reset(); 
    delay(BLE_TRANSMISSION_DELAY);
}

bool Bluetooth::hasReceivedConfirmation(){
  char confirmation[BLE_MAX_BYTE];  
  strcpy_P(confirmation, &BLE_RECV_CONFIRM[0]);
  strcat_P(confirmation, &BLE_RECV_TERMINATOR[0]);  
  wdt_reset();                  
  return strstr(buffer, confirmation) != NULL;
}

void Bluetooth::clearSerialBuffer(){
    while (bt.available()) {
        bt.read();
        wdt_reset();             
    } 
}
