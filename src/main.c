#include <math.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>

#include "capture.h"
#include "dsp.h"
#include "leds.h"

void hue_wheel(uint t) {
  for (uint i = 0; i < PIXELS; ++i) {
    float hue = (float)i / PIXELS * 360;
    hue = fmod(hue + t, 360);
    led_hsv(hue, 1.0, 0.005);
    // led_rgbwc(255, 255, 255, 255, 255);
  }
}

int main() {
  stdio_init_all();

  capture_init();
  leds_init();

  uint t = 0;
  while (true) {
    hue_wheel(t);
    sleep_ms(1);
    ++t;
  }

  capture(true);

  while (true) {
    if (buffer_ready()) {
      vu_t volumes = vu_meter();
      printf("Left: %-3d; Right: %-3d\n", volumes.left, volumes.right);
      next_buffer();
    }
  }
}
