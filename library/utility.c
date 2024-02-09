#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

u08 get_btn(void) {
   if (PINE & _BV(SW1_PIN))
      return 0;
   return 1;
}

u08 digital(u08 num) {
   switch(num) {
   case 0:
   case 1:
      if (PINE & _BV(num))
         return 1;
      break;
   case 2:
   case 3:
      if (PING & _BV(num+1))
         return 1;
      break;
   case 4:
      if (PINE & _BV(6))
            return 1;
      break;
   case 5:
   case 6:
      if (PINB & _BV(num))
         return 1;
      break;
   case 7:
   case 8:
   case 9:
      if (PINC & _BV(num-2))
         return 1;
      break;
   case 10:
      if (PINB & _BV(0))
         return 1;
      break;
   case 11:
   case 12:
      if (PINB & _BV(num-9))
         return 1;
      break;
   case 13:
      if (PINB & _BV(1))
         return 1;
      break;
   }

   return 0;
}

#ifndef BOOTLOADER

//set the direction for a digital pin
void digital_dir(u08 num, u08 dir) {
   switch(num) {
   case 0:
   case 1:
      if (dir) {
         DDRE |= _BV(num);
      } else {
         DDRE &= ~_BV(num);
      }
      break;
   case 2:
   case 3:
      if (dir) {
         DDRG |= _BV(num+1);
      } else {
         DDRG &= ~_BV(num+1);
      }
      break;
   case 4:
      if (dir) {
         DDRE |= _BV(6);
      } else {
         DDRE &= ~_BV(6);
      }
      break;
   case 5:
   case 6:
      if (dir) {
         DDRB |= _BV(num);
      } else {
         DDRB &= ~_BV(num);
      }
      break;
   case 7:
   case 8:
   case 9:
      if (dir) {
         DDRC |= _BV(num-2);
      } else {
         DDRC &= ~_BV(num-2);
      }
      break;
   case 10:
      if (dir) {
         DDRB |= _BV(0);
      } else {
         DDRB &= ~_BV(0);
      }
      break;
   case 11:
   case 12:
      if (dir) {
         DDRB |= _BV(num-9);
      } else {
         DDRB &= ~_BV(num-9);
      }
      break;
   case 13:
      if (dir) {
         DDRB |= _BV(1);
      } else {
         DDRB &= ~_BV(1);
      }
      break;
   }

   return;
}

//set the output value for a digital pin
void digital_out(u08 num, u08 out) {
   switch(num) {
   case 0:
   case 1:
      if (out) {
         PORTE |= _BV(num);
      } else {
         PORTE &= ~_BV(num);
      }
      break;
   case 2:
   case 3:
      if (out) {
         PORTG |= _BV(num+1);
      } else {
         PORTG &= ~_BV(num+1);
      }
      break;
   case 4:
      if (out) {
         PORTE |= _BV(6);
      } else {
         PORTE &= ~_BV(6);
      }
      break;
   case 5:
   case 6:
      if (out) {
         PORTB |= _BV(num);
      } else {
         PORTB &= ~_BV(num);
      }
      break;
   case 7:
   case 8:
   case 9:
      if (out) {
         PORTC |= _BV(num-2);
      } else {
         PORTC &= ~_BV(num-2);
      }
      break;
   case 10:
      if (out) {
         PORTB |= _BV(0);
      } else {
         PORTB &= ~_BV(0);
      }
      break;
   case 11:
   case 12:
      if (out) {
         PORTB |= _BV(num-9);
      } else {
         PORTB &= ~_BV(num-9);
      }
      break;
   case 13:
      if (out) {
         PORTB |= _BV(1);
      } else {
         PORTB &= ~_BV(1);
      }
      break;
   }

   return;
}

#endif

void led(u08 num, u08 state) {
   if (num == 0) {
      if (state == 1)
         sbi(PORTG,LED0_PIN);
      else
         cbi(PORTG,LED0_PIN);
   } else {
      if (state == 1)
         sbi(PORTG,LED1_PIN);
      else
         cbi(PORTG,LED1_PIN);
   }
}

void led_on(u08 num) {
   if (num == 0) {
      sbi(PORTG,LED0_PIN);
   } else {
      sbi(PORTG,LED1_PIN);
   }
}

void led_off(u08 num) {
   if (num == 0) {
      cbi(PORTG,LED0_PIN);
   } else {
      cbi(PORTG,LED1_PIN);
   }
}

void init(void) {
   //make TX pin output
   DDRD |= _BV(2);

   //make LED0 and LED1 outputs
   DDRG |= _BV(LED1_PIN) | _BV(LED0_PIN);

   //enable SW1 pull
   sbi(PORTE,SW1_PIN);

   //set i2c pins to outputs for testing
   //DDRE |= _BV(4) | _BV(5);

   init_adc();
   init_lcd();
#ifndef BOOTLOADER
   init_servo();
#endif
   init_motor();

   //initialize the accelerometer
#ifndef BOOTLOADER
   i2c_regwrite(0x1C,0x2A,0x1);  //change to WAKE mode
   _delay_ms(100);
   unlock_bus();
#endif
}
