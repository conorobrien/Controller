#include "encoder.h"
#include "pid.h"
#include "m_general.h"
#include "m_usb.h"

#include <avr/io.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

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
}

volatile float disturbance = 10;

volatile float sys1_current = 0;
void set_sys1(float force) {
  static float sys1_vel = 0;
  float mass = 1;
  float dt = 0.01;

  sys1_current += sys1_vel*dt;
  sys1_vel += ((force+disturbance)/mass)*dt;

  if (sys1_current > 255) sys1_current = 255;
  if (sys1_current < 0) sys1_current = 0;

  set_led(sys1_current);
  usb_tx_char((char)sys1_current);
}

float read_sys1(void) {
  return sys1_current;
}

volatile float sys2_current = 100;
void set_sys2(float force) {
  static float sys2_vel = 0;

  float mass = 10;
  float dt = 0.01;

  sys2_current += sys2_vel*dt;
  sys2_vel += ((force + disturbance)/mass)*dt;
  if (sys2_current > 255) sys2_current = 255;
  if (sys2_current < 0) sys2_current = 0;
  usb_tx_char((char)sys2_current);
}

float read_sys2(void) {
  return sys2_current;
}

int main()
{ 
  CLKPR = (1<<CLKPCE);
  CLKPR = 0;

  m_disableJTAG();
  usb_init();
  while(!usb_configured());
  // while(!usb_rx_available());

//// PID TESTING
  init_led();

  // float target = 125;

  // // pid setup function, inits timer1 and needed variables
  // pid_setup(2, 100);

  // // add two pid systems (max 4)
  // pid_add(&read_sys1, &set_sys1); //0
  // pid_add(&read_sys2, &set_sys2); //1

  // // Set pid coefficients and targets
  // pid_set_coefs(0, 50, 100, 30);
  // pid_set_coefs(1, 50, 0, 30);
  // pid_set_target(0,target);
  // pid_set_target(1,target);

// ENCODER TESTING
  enc_setup();
  while (1) {
      _delay_ms(100);
      m_usb_tx_long(enc_read());
      m_usb_tx_string("\n");
      // target = (float)(rand()%256);
      // pid_set_target(0,target);
      // pid_set_target(1,target);
  }
}

ISR(TIMER0_COMPA_vect) {
  PORTE |= _BV(6);
}

ISR(TIMER0_OVF_vect) {
  PORTE &= ~_BV(6);
}