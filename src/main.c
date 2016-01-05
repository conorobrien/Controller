// #define ENC_LED
#include "encoder.h"
#include "m_usb.h"
#include "motors.h"
#include "pid.h"
#include <util/delay.h>

float pid_in(void) {
  return enc_read();
}

void pid_out(float cmd) {
  if (cmd > 0)
    motor_set(0, FORWARD, (uint8_t) cmd);
  else
    motor_set(0, REVERSE, (uint8_t) -1*cmd);
  if (m_usb_isconnected())
    m_usb_tx_int((int) cmd);
}

int main(void) {
  CLKPR = (1<<CLKPCE); CLKPR=0;

  m_usb_init();
  while(!m_usb_isconnected());

  enc_setup();
  motor_init(0, &PORTE, 6, &PORTE, 2);

  pid_setup(1, 100);
  pid_add(&pid_in, &pid_out);
  pid_set_coefs(0, 1, 0, 0);
  pid_set_target(0, 0);
  pid_set_output_limits(0, -255, 255);

  pid_start();
  for (;;) {
  }

  return 0;
}