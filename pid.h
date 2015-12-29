#ifndef _PID
#define _PID

#include <stdint.h>
#include <avr/io.h>

void pid_setup(uint8_t n_pids, uint16_t pid_freq);

void pid_add(float (*pid_input)(void), void (*pid_output)(float));

void pid_set_coefs(uint8_t n, float kp, float ki, float kd);

void pid_set_target(uint8_t n, float target);

void pid_set_int_limits(uint8_t n, float min, float max);

void pid_set_output_limits(uint8_t n, float min, float max);

void pid_destroy(void);

#endif