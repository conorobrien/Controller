#ifndef _M_LEDS
#define _M_LEDS

#define RED 6
#define GREEN 2

#define OFF 0
#define ON 1
#define TOGGLE 2

void led_init(uint8_t type) {
  // set LED to output
  DDRE |= _BV(type); 
  PORTE |= _BV(type);
}

void led_on(uint8_t type, uint8_t op) {
  switch op {
    case OFF:
      PORTE |= _BV(type);
      break;
    case ON:
      PORTE &= ~_BV(type);
      break;
    case ON:
      PORTE ^= _BV(type);
      break;
  }
}

#endif
