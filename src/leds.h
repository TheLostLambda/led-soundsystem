#pragma once

#include <pico/types.h>
#include <stddef.h>

// Constant Parameters =========================================================

constexpr size_t PIXELS = 50;
constexpr uint WS2805_PIN = 16;

// Public Functions ============================================================

void leds_init();

void led_rgbwc(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t c);
void led_rgb(uint8_t r, uint8_t g, uint8_t b);
void led_wc(uint8_t w, uint8_t c);

void led_hsvwc(float h, float s, float v, uint8_t w, uint8_t c);
void led_hsv(float h, float s, float v);
