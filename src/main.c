#include <pico/stdio.h>
#include <pico/time.h>
#include <stddef.h>
#include <stdio.h>

#include "capture.h"
#include "dsp.h"
#include "leds.h"

void env_update(float *val, float new) {
  if (new < 3) {
    new = 0;
  } else {
    new *= 2;
  }
  float old = *val;
  float keep = 0.5;
  if (new > old) {
    *val = new;
  } else {
    *val = keep * old + (1 - keep) * new;
  }
}

int main() {
  stdio_init_all();

  capture_init();
  leds_init();

  capture(true);

  float left = 0;
  float right = 0;

  while (true) {
    if (buffer_ready()) {
      vu_t volumes = vu_meter();
      float halfway = PIXELS / 2.0;
      env_update(&left, (float)volumes.left / UINT8_MAX * halfway);
      env_update(&right, (float)volumes.right / UINT8_MAX * halfway);
      for (size_t pixel = 0; pixel < PIXELS; ++pixel) {
        if (pixel > (halfway - right) && pixel < (halfway + left)) {
          if (pixel < 5 || pixel > PIXELS - 5) {
            led_rgb(6, 0, 0);
          } else if (pixel < 10 || pixel > PIXELS - 10) {
            led_rgb(6, 4, 0);
          } else {
            led_rgb(0, 6, 0);
          }
        } else {
          led_off();
        }
      }
      // printf("Left: %-3f; Right: %-3f\n", left, right);
      next_buffer();
    }
  }
}
