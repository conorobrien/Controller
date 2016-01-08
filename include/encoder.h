#pragma once

// define ENC_LED to have direction and freqency LEDs

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>

#define ENC_A (bool)(PIND & _BV(1))
#define ENC_B (bool)(PIND & _BV(2))

volatile long _enc_count = 0;
long _enc_tmp = 0;

void enc_setup(void) {
  // set INT0 and INT1 to trigger on rising and falling edges
  EICRA |= _BV(ISC00)|_BV(ISC10); 
  // unmask INT0 and INT1
  EIMSK |= _BV(INT0)|_BV(INT1); 
  // turn on interrupts
  sei();

  #ifdef ENC_LED
    // Turn on MAEVARM internal LEDs
    DDRE |= _BV(2);
    DDRE |= _BV(6);
  #endif
}

long enc_read(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _enc_tmp = _enc_count;
  }
  return _enc_tmp;
}

void enc_set(long in) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _enc_count = in;
  }
}


ISR(INT1_vect) {
  // if A,B equal, moving in the positive direction
  if (ENC_A == ENC_B){
    _enc_count += 1;
    #ifdef ENC_LED
      PORTE ^= _BV(6);
      PORTE |= _BV(2);
    #endif
  } else {
    _enc_count -= 1;
    #ifdef ENC_LED
      PORTE |= _BV(6);
      PORTE ^= _BV(2);
    #endif
  }
}

ISR(INT2_vect) {
  // if A,B equal, moving in the negative direction
  if (ENC_A == ENC_B) {
    _enc_count -= 1;
    #ifdef ENC_LED
      PORTE |= _BV(6);
      PORTE ^= _BV(2);
    #endif  
  } else {
    _enc_count += 1;
    #ifdef ENC_LED
      PORTE ^= _BV(6);
      PORTE |= _BV(2);
    #endif 
  }
}
