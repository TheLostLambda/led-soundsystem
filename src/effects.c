#include "leds.h"
#include <math.h>
#include <pico/time.h>

absolute_time_t hue_wheel_state() { return get_absolute_time(); }
void hue_wheel(uint pixel, absolute_time_t *timer) {
  constexpr float MS_PER_DEG = 5;

  int64_t ms = absolute_time_diff_us(*timer, get_absolute_time()) / 1000;
  float hue = (float)pixel / PIXELS * 360;
  hue = fmod(hue + ms / MS_PER_DEG, 360);
  led_hsv(hue, 1.0, 1.0);
}
