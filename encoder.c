#ifndef _ENCODER
#define _ENCODER

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <string.h>

// #ifdef __AVR_ATtiny4313__
#define ENC_A (bool)(PIND & _BV(0))
#define ENC_B (bool)(PIND & _BV(1))

volatile long enc_count = 0;
long enc_tmp = 0;
void enc_setup(void) {
  // set INT0 and INT1 to trigger on rising and falling edges
  EICRA |= _BV(ISC00)|_BV(ISC10); 
  // unmask INT0 and INT1
  EIMSK |= _BV(INT0)|_BV(INT1); 
  // turn on interrupts
  sei();

  // Turn on LED
  DDRE |= _BV(2);
  DDRE |= _BV(6);
  // PORTE |= _BV(2);
}

long enc_read(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    enc_tmp = enc_count;
  }
  return enc_tmp;
}

void enc_set(long in) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    enc_count = in;
  }
}

ISR(INT0_vect) {
  // if A,B equal, moving in the positive direction
  if (ENC_A == ENC_B){
    enc_count += 1;
    PORTE ^= _BV(6);
}
  else {
    enc_count -= 1;
    PORTE ^= _BV(2);
  }
}

ISR(INT1_vect) {
  // if A,B equal, moving in the negative direction
  if (ENC_A == ENC_B)
    enc_count -= 1;
  else
    enc_count += 1;
  if (ENC_B)
    PORTE |= _BV(2);
  else    
    PORTE &= ~_BV(2);



}

#endif