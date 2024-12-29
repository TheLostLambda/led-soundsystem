#include <pico/stdio.h>
#include <stdio.h>

#include "capture.h"
#include "dsp.h"

int main() {
  stdio_init_all();

  capture_init();
  capture(true);

  while (true) {
    if (buffer_ready()) {
      vu_t volumes = vu_meter();
      printf("Left: %-3d; Right: %-3d\n", volumes.left, volumes.right);
      next_buffer();
    }
  }
}
