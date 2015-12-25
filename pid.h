#ifndef _PID
#define _PID

#include <stdint.h>
#include <avr/io.h>

#define PID_FREQ 100

void pid_setup(float (*pid_input)(void), void (*pid_output)(float));

void pid_set_coefs(float kp, float ki, float kd);

void pid_set_target(float target);

void pid_set_int_limits(float min, float max);

void pid_set_output_limits(float min, float max);

#endif