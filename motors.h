#ifndef _MOTORS
#define _MOTORS

#include <stdint.h>
#include <avr/io.h>

void motor_init(uint8_t n) {
  if ((TCCR0A & _BV(WGM01)) == 0) {
    TCCR0B |= _BV(CS00); // turn timer0 on, 1 prescaler
    TCCR0A |= _BV(WGM0); // set pwm to mode1, phase correct
  }

  if (n == 1) {
    DDRB |= _BV(PD2); 
    TCCR0A |= _BV(COM0A1);
    OCR0A = 0;
  } else if (n == 2) {
    DDRD |= _BV(PD5);
    TCCR0A |= _BV(COM0B1);
    OCR0B = 0;
  }
}

void motor_set(uint8_t n, uint8_t duty) {
  if (n == 1) {
    OCR0A = duty;
  } else if (n == 2) {
    OCR0B = duty;
  }
}

#endif