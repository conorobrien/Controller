#ifndef _MOTORS
#define _MOTORS

#include <stdint.h>
#include <avr/io.h>

#define FORWARD 0
#define REVERSE 1

volatile uint8_t* _dir_port[2][2];
uint8_t _dir_pin[2][2];

void motor_init(uint8_t n, volatile uint8_t* dport1, uint8_t dpin1,
  volatile uint8_t* dport2, uint8_t dpin2) {

  if (n > 1) 
    return;

  // Set DDRx, using port address
  switch ((int)dport1) {
    case (int)&PORTB:
      DDRB |= _BV(dpin1);
      break;
    case (int)&PORTC:
      DDRC |= _BV(dpin1);
      break;
    case (int)&PORTD:
      DDRD |= _BV(dpin1);
      break;
    case (int)&PORTE:
      DDRE |= _BV(dpin1);
      break;
    case (int)&PORTF:
      DDRF |= _BV(dpin1);
      break;
  }

  switch ((int)dport2) {
    case (int)&PORTB:
      DDRB |= _BV(dpin2);
      break;
    case (int)&PORTC:
      DDRC |= _BV(dpin2);
      break;
    case (int)&PORTD:
      DDRD |= _BV(dpin2);
      break;
    case (int)&PORTE:
      DDRE |= _BV(dpin2);
      break;
    case (int)&PORTF:
      DDRF |= _BV(dpin2);
      break;
  }

  if (TCCR0B == 0) {
    TCCR0B |= _BV(CS00); // turn timer0 on, 1 prescaler
    TCCR0A |= _BV(WGM00); // set pwm to mode1, phase correct
  }

  // Set PWM pins to output, unmask interrupt
  if (n == 0) {
    DDRB |= _BV(PD2); 
    TCCR0A |= _BV(COM0A1);
    OCR0A = 0;
  } else if (n == 1) {
    DDRD |= _BV(PD5);
    TCCR0A |= _BV(COM0B1);
    OCR0B = 0;
  }

  _dir_port[n][0] = dport1;
  _dir_port[n][1] = dport2;
  _dir_pin[n][0] = dpin1;
  _dir_pin[n][1] = dpin2;
}

void motor_set(uint8_t n, uint8_t dir, uint8_t duty) {
  if (dir == FORWARD) {
    *_dir_port[n][0] |= _BV(_dir_pin[n][0]);
    *_dir_port[n][1] &= ~_BV(_dir_pin[n][1]);
  } else {
    *_dir_port[n][0] &= ~_BV(_dir_pin[n][0]);
    *_dir_port[n][1] |= _BV(_dir_pin[n][1]);
  }

  if (n == 0)
    OCR0A = duty;
  else if (n == 1)
    OCR0B = duty;
}

#endif