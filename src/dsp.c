#include "dsp.h"
#include "capture.h"

// Private Functions ===========================================================

uint8_t volume(uint channel) {
  uint8_t min_sample = UINT8_MAX;
  uint8_t max_sample = 0;
  for (size_t i = 0; i < VU_BUFFERS * BUFFER_SIZE; ++i) {
    uint8_t sample = get_sample(channel, i);
    if (sample > max_sample) {
      max_sample = sample;
    }
    if (sample < min_sample) {
      min_sample = sample;
    }
  }
  return max_sample - min_sample;
}

// Public Functions ============================================================

vu_t vu_meter() {
  return (vu_t){
      .left = volume(LEFT_CHANNEL),
      .right = volume(RIGHT_CHANNEL),
  };
}
