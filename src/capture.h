#pragma once

#include <pico/types.h>
#include <stddef.h>
#include <stdint.h>

// Constant Parameters =========================================================

// GPIO & ADC Pins
constexpr uint ADC_GPIO_START = 26;
constexpr uint LEFT_CHANNEL = 0;
constexpr uint RIGHT_CHANNEL = 1;

// Audio Capture Settings
constexpr uint8_t CHANNELS_BITS = 1;
constexpr uint CHANNELS = 1 << CHANNELS_BITS;
constexpr uint SAMPLE_RATE = 44100; // Hz

// Audio Buffer Settings
constexpr uint8_t BUFFER_SIZE_BITS = 9;
constexpr uint8_t BUFFER_COUNT_BITS = 3;
constexpr uint8_t FULL_RING_SIZE_BITS =
    CHANNELS_BITS + BUFFER_COUNT_BITS + BUFFER_SIZE_BITS;
constexpr size_t BUFFER_SIZE = 1 << BUFFER_SIZE_BITS;
constexpr size_t BUFFER_COUNT = 1 << BUFFER_COUNT_BITS;
constexpr size_t FULL_RING_SIZE = 1 << FULL_RING_SIZE_BITS;

// Public Functions ============================================================

void capture_init();
void capture(bool enabled);
bool buffer_ready();
uint8_t get_sample(uint channel, size_t index);
void next_buffer();
