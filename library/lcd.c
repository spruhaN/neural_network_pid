#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define _HOME_ADDR      0x80
#define _LINE_INCR      0x40

#ifndef BOOTLOADER

#define SCROLLING 1

#endif

void e_Clk(void) {
  _delay_us(2);
  sbi(PORTC,LCD_E_PIN);
  _delay_us(2);
  cbi(PORTC,LCD_E_PIN);
  _delay_us(2);
}

void write_lcd(u08 data) {
  cli();
  LCD_DATA_PORT = data;
  e_Clk();
  sei();
}

void write_control(u08 data) {
  cbi(PORTF,LCD_RS_PIN); //set RS low
  write_lcd(data);
}

void write_data(u08 data) {
  sbi(PORTF,LCD_RS_PIN); //set RS high
  write_lcd(data);
}

void lcd_cursor(uint8_t col, uint8_t row)
{
   if (col >= 16 || row >= 2)
   {
      return;
   }

   u08 addr = _HOME_ADDR + row * _LINE_INCR + col;
   write_control(addr);
   _delay_us(37);
}

void init_lcd(void) {
   DDRC |= _BV(LCD_E_PIN);
   DDRF |= _BV(LCD_RS_PIN);
   DDRA = 0xFF; //make all the data pins output

//   write_control(0x38);  //function set
   _delay_ms(5);

   write_control(0x38);  //function set
   _delay_us(160);

   write_control(0x38);  //function set
   _delay_us(160);
   write_control(0x38);  //function set
   _delay_us(160);
   write_control(0x08);  //turn display off
   _delay_us(160);
   write_control(0x01);  //clear display
   _delay_us(4000);
   write_control(0x06);  //set entry mode
   _delay_us(160);//*/

   write_control(0x38); //function set
   _delay_us(100);
   write_control(0x0C);
   _delay_us(100);
   
}

void print_string(char* string) {
  u08 i=0;

  while(string[i] != 0) {
     write_data(string[i]);
     _delay_us(160);
     i++;
  }
}

void print_num(u16 number) {
   u08 i;
   u16 base = 9999;
   u08 leading = 1;
   u08 digit;

   if (number == 0) {
      write_data(48);
      _delay_us(160);
      return;
   }

   for (i=0;i<5;i++) {
      digit = number / (base+1);

      if (digit != 0)
         leading = 0;

      if (number > base) {
         if (!leading) {
            write_data(48 + digit);
            _delay_us(160);
         }
         number = number % (base+1);
      } else {
         if (!leading) {
            write_data(48 + digit);
            _delay_us(160);
         }
      }

      base = base / 10;
   }
}

void clear_screen(void) {
  write_control(0x01);  //clear display
  _delay_us(4000);
}
