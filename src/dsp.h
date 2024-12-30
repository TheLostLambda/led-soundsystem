#pragma once

#include <pico/types.h>
#include <stdint.h>

// Constant Parameters =========================================================

constexpr uint8_t VU_BUFFERS = 4;

// Public Types ================================================================

typedef struct {
  uint8_t left;
  uint8_t right;
} vu_t;

// Public Functions ============================================================

vu_t vu_meter();

