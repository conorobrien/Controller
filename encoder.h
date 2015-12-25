#ifndef _ENCODER
#define _ENCODER

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <string.h>

#ifdef __AVR_ATtiny4313__
# define ENC_A (bool)(PIND & _BV(2))
# define ENC_B (bool)(PIND & _BV(3))
# define INTCR MCUCR
# define INMSK GIMSK
#elif defined (__AVR_ATmega32U4__)
# define ENC_A (bool)(PIND & _BV(0))
# define ENC_B (bool)(PIND & _BV(1))
# define INTCR EICRA
# define INMSK EIMSK
#else
# warning "device type not defined"
#endif



volatile int16_t _enc_count = 0;
int16_t _enc_tmp;

void encoder_setup(void) {
  // set INT0 and INT1 to trigger on rising and falling edges
  INTCR |= _BV(ISC00)|_BV(ISC10); 
  // unmask INT0 and INT1
  INMSK |= _BV(INT0)|_BV(INT1); 
  // turn on interrupts
  sei();
}

void encoder_read(int16_t* out) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    memcpy(out, (int16_t*)&_enc_count, sizeof(int16_t));
  }
}

ISR(INT0_vect) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // if A,B equal, moving in the positive direction
    if (ENC_A && ENC_B)
      _enc_count += 1;
    else
      _enc_count -= 1;
  }
}

ISR(INT1_vect) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // if A,B equal, moving in the negative direction
    if (ENC_A && ENC_B)
      _enc_count -= 1;
    else
      _enc_count += 1;
  }
}

#endif