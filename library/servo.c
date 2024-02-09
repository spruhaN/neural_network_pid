#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

u08 servo_state=0;
u16 servo_high_time[4];
u16 servo_low_time[4];

ISR(TIMER1_COMPA_vect) {
   if (servo_state & 1) {
      OCR1A += servo_low_time[servo_state >> 1];
      PORTC &= ~_BV(servo_state >> 1);
   } else {
      OCR1A += servo_high_time[servo_state >> 1];
      PORTC |= _BV(servo_state >> 1);
   }

   servo_state += 1;
   servo_state &= 7;
}

void set_servo(u08 servo_num, u08 position) {
   //position from 0 to 250
   servo_high_time[servo_num] = 2500 + 10 * position;
   servo_low_time[servo_num] = 12500 - servo_high_time[servo_num];
}

void init_servo(void) {
   u08 i;

   for(i=0;i<4;i++) {
      set_servo(i,0);
   }

   DDRC |= _BV(SERVO0_PIN) | _BV(SERVO1_PIN) | _BV(SERVO2_PIN) | _BV(SERVO3_PIN);
   TCCR1B = 0;
   TCCR1B |= _BV(CS11);
   TIMSK1 |= _BV(OCIE1A);
   sei();
}
