#pragma once

#include <hardware/pio.h>
#include <pico/types.h>
#include <stdint.h>

// Public Types ================================================================

typedef struct {
  PIO pio;
  uint sm;
} ws2805_t;

typedef uint64_t grbwc_t;

// Public Functions ============================================================

ws2805_t ws2805_init(uint pin);
void put_pixel(ws2805_t ws2805, grbwc_t pixel);
grbwc_t rgbwc(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t c);
