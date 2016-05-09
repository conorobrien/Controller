#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pid.h"

uint8_t pids_max = 0;
uint8_t pid_n = 0;
uint16_t pid_freq = 100;

// struct containing everything for each pid controller
typedef struct {
  // Function pointers for input and output
  void (*out)(float);
  float (*in)(void);
  // PID set point
  float target;
  // Previous input (for derivative)
  float input_last;
  // Coefficients
  float kp, ki, kd;
  // Integral error accumulator
  float err_i;
  // Limits for output and integral error
  float i_max, i_min, out_max, out_min;
} pid_t;

pid_t* pids = NULL;

void pid_default(pid_t* input) {
  // Set function pointers before calling this
  input->target = input->input_last = input->in();

  // Coefficients to zero
  input->kp = input->kd = input->ki = 0;

  // Integral error set to zero
  input->err_i = 0;

  // Limits set to the max
  input->i_max = input->out_max = 1E37;
  input->i_min = input->out_min = -1E37;
}

uint8_t pid_setup(uint8_t n, uint16_t pfreq) {
  pids_max = n;
  pid_freq = pfreq;

  if (!pids) {
    pids = calloc(pids_max, sizeof(pid_t));
  } else {
    pid_t *pids_temp = realloc(pids, pids_max*sizeof(pid_t));
    if (pids_temp)
      pids = pids_temp;
  }

  return (pids != NULL) ? 0 : -1; // return 0 for success, -1 for fail

}

void pid_start(void) {
  // set to pid_freq Hz
  OCR1A = F_CPU/64/pid_freq;
  // Timer 1 to CTC, TOP is OCR1A
  TCCR1B |= _BV(WGM12);
  // Unmask timer interrupt
  TIMSK1 |= _BV(OCIE1A);
  // Turn on timer 1, prescale 64
  TCCR1B |= _BV(CS11)|_BV(CS10);
  // Turn on interrupts
  sei();
}

void pid_stop(void) {
  TCCR1B &= (uint8_t)~(_BV(CS11)|_BV(CS10));
}

void pid_add(float (*pid_input)(void), void (*pid_output)(float)) {
  if (pids && pid_n < pids_max) {
    pids[pid_n].out = pid_output;
    pids[pid_n].in = pid_input;
    pid_default(&pids[pid_n]);

    pid_n++;
  }
}

void pid_set_coefs(uint8_t n, float kp_in, float ki_in, float kd_in) {
  if (n < pid_n && pids) {
    pids[n].kp = kp_in;
    pids[n].kd = kd_in*pid_freq;
    pids[n].ki = ki_in/pid_freq;
  }
}

void pid_set_target(uint8_t n, float target_in) {
    pids[n].target = target_in;
}

void pid_set_int_limits(uint8_t n, float min, float max) {
  if (n < pid_n && pids) {
  pids[n].i_min = min;
  pids[n].i_max = max;
  }
}

void pid_set_output_limits(uint8_t n, float min, float max) {
  if (n < pid_n && pids) {
    pids[n].out_min = min;
    pids[n].out_max = max;
  }
}

void pid_destroy(void) {
  if (pids)
    free(pids);
}

ISR(TIMER1_COMPA_vect, ISR_NOBLOCK) {
  float input, error, err_d, output;

  for (uint8_t n = 0; n < pid_n; n++) {
    input = pids[n].in();
    error = pids[n].target - input;

    pids[n].err_i += pids[n].ki*error;

    if (pids[n].err_i > pids[n].i_max) pids[n].err_i = pids[n].i_max;
    if (pids[n].err_i < pids[n].i_min) pids[n].err_i = pids[n].i_min;

    err_d = input - pids[n].input_last;
    pids[n].input_last = input;

    output = pids[n].kp*error - pids[n].kd*err_d + pids[n].err_i;

    if (output > pids[n].out_max) output = pids[n].out_max;
    if (output < pids[n].out_min) output = pids[n].out_min;

    pids[n].out(output);
  }
}
