

/*
 * Title: POWER
 * 
 * Author: Andre Madureira
 * 
 * Details: 
 * Reduce power usage without interfering with normal uC operation
 */
 
#include <avr/power.h>

/*
 * Enter mask to setup how power will be managed by Arduino
 *    Bit - Module
 *     0      ADC
 *     1      TIMER0
 *     2      TIMER1
 *     3      TIMER2
 *     4      SPI
 *     5      TWI / I2C
 *     6      USART / HARD SERIAL
 */
void setupModules(unsigned char mask){
  //All SoC Modules - Disable or Enable
  if (mask & 0xFF == 0xFF) power_all_enable();
  else if (mask & 0xFF == 0) power_all_disable();
  else {  
    //Analog-Digital converter - Disable or Enable
    if (mask & 1 == 0) power_adc_disable();     
    else power_adc_enable();    
    
    //Timer0 - Disable or Enable
    if (mask & 2 == 0) power_timer0_disable(); 
    else power_timer0_enable();
    //Timer1 - Disable or Enable
    if (mask & 4 == 0) power_timer1_disable(); 
    else power_timer1_enable();
    //Timer2 - Disable or Enable
    if (mask & 8 == 0) power_timer2_disable(); 
    else power_timer2_enable();
    
    //SPI converter - Disable or Enable
    if (mask & 16 == 0) power_spi_disable(); 
    else power_spi_enable();   
    //TWI (I2C) - Disable or Enable
    if (mask & 32 == 0) power_twi_disable(); 
    else power_twi_enable();  
    
    //USART0 (Serial) - Disable or Enable
    if (mask & 64 == 0) power_usart0_disable(); 
    else power_usart0_enable();    
  }
}

/*  NOT WORKING YET FOR SYSTEM CLOCK
 * 
 * CHANGE clock speed to minimize power consumption and performance
 *    Values - Clock Speed (SysClock)
 *       1         /1
 *       2         /2
 *       4         /4
 *       8         /8
 *       16        /16
 *       32        /32
 *       64        /64
 *       128       /128
 *       256       /256
 */
 /*
void setupClock(unsigned short s_clock){
  clock_div_t clock_speed;
  switch(s_clock){    
    case 2:
      clock_speed = clock_div_2;
      break;
    case 4:
      clock_speed = clock_div_4;
      break;
    case 8:
      clock_speed = clock_div_8;
      break;
    case 16:
      clock_speed = clock_div_16;
      break;
    case 32:
      clock_speed = clock_div_32;
      break;
    case 64:
      clock_speed = clock_div_64;
      break;
    case 128:
      clock_speed = clock_div_128;
      break;
    case 256: 
      clock_speed = clock_div_256;
      break;
    default:
      clock_speed = clock_div_1;      
      break;
  }
  clock_prescale_set(clock_speed);
}
*/

