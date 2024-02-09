#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void set_motor(u08 num, signed char speed) {
    if (num == 0) {
        OCR2A = speed * 255 / 100;

        if (speed > 0) {
            PORTD |= _BV(MOTOR0_DIR0_PIN);
            PORTD &= ~_BV(MOTOR0_DIR1_PIN);
        } else {
            PORTD |= _BV(MOTOR0_DIR1_PIN);
            PORTD &= ~_BV(MOTOR0_DIR0_PIN);
        }
    } else  {
        OCR0A = speed * 255 / 100;

        if (speed > 0) {
            PORTD |= _BV(MOTOR1_DIR0_PIN);
            PORTD &= ~_BV(MOTOR1_DIR1_PIN);
        } else {
            PORTD |= _BV(MOTOR1_DIR1_PIN);
            PORTD &= ~_BV(MOTOR1_DIR0_PIN);
        }
    }
}

void init_motor(void) {
  set_motor(0,0);
  set_motor(1,0);

  DDRB |= _BV(MOTOR0_EN_PIN) | _BV(MOTOR1_EN_PIN);
  DDRD |= _BV(MOTOR0_DIR0_PIN) | _BV(MOTOR0_DIR1_PIN) | _BV(MOTOR1_DIR0_PIN) | _BV(MOTOR1_DIR1_PIN);

  //motor 0
  TCCR2A |= _BV(WGM21) | _BV(WGM20) | _BV(COM2A1); //fast PWM, non-inverting
  //TCCR2A |= _BV(CS22) | _BV(CS21) | _BV(CS20); //1024
  TCCR2A |= _BV(CS21); //1024

  //motor 1
  TCCR0A |= _BV(WGM01) | _BV(WGM00) | _BV(COM0A1); //fast PWM, non-inverting
  TCCR0A |= _BV(CS01);

  OCR0A = 10;
  OCR2A = 10;
}

void test_motor(void) {
   //test motor 0
   PORTD |= _BV(MOTOR0_DIR0_PIN);
   PORTD &= ~_BV(MOTOR0_DIR1_PIN);
   PORTB |= _BV(MOTOR0_EN_PIN);
   
   //test motor 1
   PORTD |= _BV(MOTOR1_DIR1_PIN);
   PORTD &= ~_BV(MOTOR1_DIR0_PIN);
   PORTB |= _BV(MOTOR1_EN_PIN);
}
