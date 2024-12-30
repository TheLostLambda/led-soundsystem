#include "leds.h"
#include "ws2805.h"
#include <math.h>
#include <stdint.h>

// Private State ===============================================================

static ws2805_t ws2805 = {};

// Private Functions ===========================================================

static inline grbwc_t hsvwc(float h, float s, float v, uint8_t w, uint8_t c) {
  float x = v * s;
  float y = x * (1 - fabs(fmod(h / 60, 2) - 1));
  float z = v - x;
  float r, g, b;
  if (h < 60) {
    r = x, g = y, b = 0;
  } else if (h < 120) {
    r = y, g = x, b = 0;
  } else if (h < 180) {
    r = 0, g = x, b = y;
  } else if (h < 240) {
    r = 0, g = y, b = x;
  } else if (h < 300) {
    r = y, g = 0, b = x;
  } else {
    r = x, g = 0, b = y;
  }
  uint8_t red = (uint8_t)round((r + z) * 255);
  uint8_t green = (uint8_t)round((g + z) * 255);
  uint8_t blue = (uint8_t)round((b + z) * 255);
  return rgbwc(red, green, blue, w, c);
}

// Public Functions ============================================================

void leds_init() { ws2805 = ws2805_init(WS2805_PIN); }

void led_rgbwc(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t c) {
  put_pixel(ws2805, rgbwc(r, g, b, w, c));
}

void led_rgb(uint8_t r, uint8_t g, uint8_t b) { led_rgbwc(r, g, b, 0, 0); }

void led_wc(uint8_t w, uint8_t c) { led_rgbwc(0, 0, 0, w, c); }

void led_hsvwc(float h, float s, float v, uint8_t w, uint8_t c) {
  put_pixel(ws2805, hsvwc(h, s, v, w, c));
}

void led_hsv(float h, float s, float v) {
  put_pixel(ws2805, hsvwc(h, s, v, 0, 0));
}

void led_off() { put_pixel(ws2805, 0); }
