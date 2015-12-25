#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pid.h"

#define PID_FREQ 100

float target, input, error,
      err_i, err_d, input_last,
      output, kp, ki, kd;

float pid_i_max, pid_out_max;
pid_i_max = pid_out_max = 1E37;
float pid_i_min, pid_out_min
pid_i_min = pid_out_min = -1E37;

void (*pid_out)(float);
float (*pid_in)(void);

void pid_setup(float (*pid_input)(void), void (*pid_output)(float)) {
  // set function for pid output
  pid_out = pid_output;
  // set function for getting pid input
  pid_in = pid_input;

  // Init variables
  target = input_last = pid_in();
  kp = kd = ki = 0;

  // Timer 1 to CTC, TOP is OCR1A
  TCCR1B |= _BV(WGM12);
  // Turn on timer 1, prescale 64
  TCCR1B |= _BV(CS11)|_BV(CS10);
  // set to 100Hz
  OCR1A = 16E6/64/PID_FREQ;
  // Unmask timer interrupt
  TIMSK1 |= _BV(OCIE1A);
  // Turn on interrupts
  sei();
}

void pid_set_coefs(float kp_in, float ki_in, float kd_in) {
  kp = kp_in;
  kd = kd_in*PID_FREQ;
  ki = ki_in/PID_FREQ;
}

void pid_set_target(float target_in) {
  target = target_in;
}

void pid_set_int_limits(float min, float max) {
  pid_i_min = min;
  pid_i_max = max;
}

void pid_set_output_limits(float min, float max) {
  pid_out_min = min;
  pid_out_max = max;
}

ISR(TIMER1_COMPA_vect) {
  input = pid_in();
  error = target - input;

  err_i += ki*error;

  if (err_i > pid_i_max) err_i = pid_i_max;
  if (err_i < pid_i_min) err_i = pid_i_min;

  err_d = input - input_last;
  input_last = input;

  output = kp*error - kd*err_d + err_i;

  if (output > pid_out_max) output = pid_out_max;
  if (output < pid_out_min) output = pid_out_min;

  pid_out(output);
}
