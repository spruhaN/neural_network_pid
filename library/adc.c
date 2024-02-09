#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init_adc(void) {
   ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
   ADMUX |= _BV(ADLAR) | _BV(REFS0);
   return;
}

u08 analog(u08 num) {
   ADMUX &= 0xF8;
   ADMUX |= num;

   ADCSRA |= _BV(ADSC);

   while (ADCSRA & _BV(ADSC)) {}

   return ADCH;
}

