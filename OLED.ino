
// library reference: https://github.com/olikraus/u8g2/wiki/u8x8reference
// fonts reference:   https://github.com/olikraus/u8g2/wiki/fntlist8x8

#include <avr/wdt.h>  // Watchdog Library (needs optiboot bootloader to work well)
#include <avr/pgmspace.h> //SAVE DATA INTO FLASH MEMORY

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

unsigned char oled_row; // especifica o row do cursor - OLED
static char oled_buffer[OLED_COLS+1];

const static char oled_access_allowed[] PROGMEM = "Access Allowed";
const static char oled_unregistered[] PROGMEM = "Unregistered ID";
const static char oled_change_master[] PROGMEM = "Change Master";
const static char oled_new_master[] PROGMEM = "New Master";
const static char* const oled_strings[] PROGMEM = {
  oled_access_allowed, oled_unregistered, oled_change_master,
  oled_new_master
};

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE); // OLED display - 16 colums x 8 rows, 0 indexed

void init_oled(){
  u8x8.begin();
  u8x8.setPowerSave(OLED_POWERSAVE_OFF);    // deactivates powersave feature
  u8x8.setContrast(255);                    // range 0-255 (0 minimum, 255 maximum)
  u8x8.setFont(u8x8_font_victoriabold8_r);  // set font type    
  oled_set_cursor(0);                       // set oled beginning
}

void oled_set_cursor(unsigned char row){
  if (row == 0) {
     wdt_reset();
     u8x8.clearDisplay();
  } else {
    for (int i = row; i < OLED_ROWS; i++){
      wdt_reset();
      u8x8.clearLine(i);      
    }
  }
  u8x8.setCursor(0, oled_row = row);        // set the cursor of the OLED       
}

void oled_set_cursor_function(){
  oled_set_cursor(2);
}

void oled_set_cursor_code(){
  oled_set_cursor(3);
}

void oled_print_function(char *s){    
  u8x8.clearDisplay();
  u8x8.drawString(0, 0, s);
  oled_set_cursor_function();
}

void oled_println(char *s){
  u8x8.print(s);  
  oled_set_cursor(oled_row + 1);  
}

char* oled_get_string(int i){
  return strcpy_P(oled_buffer, (char*)pgm_read_word(&(oled_strings[i]))); 
}

char* oled_ltoa(unsigned long i){
  return ltoa(i, oled_buffer, 10);
}

