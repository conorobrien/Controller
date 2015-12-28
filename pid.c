#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pid.h"

#ifndef N_PIDS
  #define N_PIDS 4
#endif

#ifndef PID_FREQ
  #define PID_FREQ 100
#endif

float target[N_PIDS], err_i[N_PIDS], input_last[N_PIDS],
      kp[N_PIDS], ki[N_PIDS], kd[N_PIDS];


float pid_i_max[N_PIDS], pid_out_max[N_PIDS];
float pid_i_min[N_PIDS], pid_out_min[N_PIDS];

uint8_t pid_n = 0;

void (*pid_out[N_PIDS])(float);
float (*pid_in[N_PIDS])(void);

void pid_setup(void) {

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

  for (uint8_t n = 0; n < N_PIDS; n++) {
    pid_i_max[n] = pid_out_max[n] = 1E37;
    pid_i_min[n] = pid_out_min[n] = -1E37;
    kp[n] = kd[n] = ki[n] = 0;
  }
}

void pid_add(float (*pid_input)(void), void (*pid_output)(float)) {
  if (pid_n < N_PIDS) {
    pid_out[pid_n] = pid_output;
    pid_in[pid_n] = pid_input;

    input_last[pid_n] = pid_in[pid_n]();

    pid_n++;
  }
}

void pid_set_coefs(uint8_t n, float kp_in, float ki_in, float kd_in) {
  if (n < pid_n) {
    kp[n] = kp_in;
    kd[n] = kd_in*PID_FREQ;
    ki[n] = ki_in/PID_FREQ;
  }
}

void pid_set_target(uint8_t n, float target_in) {
  if (n < pid_n)
    target[n] = target_in;
}

void pid_set_int_limits(uint8_t n, float min, float max) {
  if (n < pid_n) {
  pid_i_min[n] = min;
  pid_i_max[n] = max;
  }
}

void pid_set_output_limits(uint8_t n, float min, float max) {
  if (n < pid_n) {
    pid_out_min[n] = min;
    pid_out_max[n] = max;
  }
}

ISR(TIMER1_COMPA_vect) {
  float input, error, err_d, output;

  for (uint8_t n = 0; n < pid_n; n++) {
    input = pid_in[n]();
    error = target[n] - input;

    err_i[n] += ki[n]*error;

    if (err_i[n] > pid_i_max[n]) err_i[n] = pid_i_max[n];
    if (err_i[n] < pid_i_min[n]) err_i[n] = pid_i_min[n];

    err_d = input - input_last[n];
    input_last[n] = input;

    output = kp[n]*error - kd[n]*err_d + err_i[n];

    if (output > pid_out_max[n]) output = pid_out_max[n];
    if (output < pid_out_min[n]) output = pid_out_min[n];

    pid_out[n](output);
  }
}
