#include "encoder.h"
#include "pid.h"
#include "m_general.h"
#include "m_usb.h"

#include <avr/io.h>
#include <math.h>
#include <stdint.h>
// #include "motors.h"

void init_led(void) {
  TCCR0B |= _BV(CS00); // turn timer0 on, 1 prescaler
  // TCCR0A |= _BV(WGM00); // set pwm to mode1, phase correct
  
  // set LED (E6) to output
  DDRE |= _BV(6); 
  PORTE |= _BV(6);
  OCR0A = 100;

  //turn on interrupts for ovf and match a
  TIMSK0 |= _BV(TOIE0)|_BV(OCIE0A);
  sei();
}

void set_led(float intensity) {
  if (intensity > 255)
    OCR0A = 255;
  else if (intensity < 1)
    OCR0A = 1;
  else
    OCR0A = (uint8_t)intensity;

  usb_tx_char(OCR0A);
}

volatile float intensity_current = 0;
void set_led_inertia(float intensity_force) {
  static float intensity_vel = 0;
  float mass = 1;
  float dt = 0.01;

  intensity_current += intensity_vel*dt;
  intensity_vel += ((intensity_force+10)/mass)*dt;

  set_led(intensity_current);
}

float read_led(void) {
  return intensity_current;
}

int main()
{ 
  CLKPR = (1<<CLKPCE);
  CLKPR = 0;

  m_disableJTAG();
  usb_init();
  while(!usb_configured());
  while(!usb_rx_available());


  init_led();

  pid_setup(&set_led_inertia, &read_led);
  pid_set_coefs(500, 50, 50);
  pid_set_target(125);

  while (1) {}
}

ISR(TIMER0_COMPA_vect) {
  PORTE |= _BV(6);
}

ISR(TIMER0_OVF_vect) {
  PORTE &= ~_BV(6);
}